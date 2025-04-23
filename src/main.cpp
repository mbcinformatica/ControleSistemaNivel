#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <Ultrasonic.h>
#include <neotimer.h>

WiFiClient wifiClient;
char *site_url = "http://192.168.2.51:8080/api.php";

#include <device.h>
#include <sensor.h>

IPAddress staticIP(192, 168, 2, 222);
IPAddress gateway(192, 168, 2, 100);
IPAddress subnet(255, 255, 255, 0);
IPAddress webServer(192, 168, 2, 51);

Neotimer previousMillisHistorico;
Neotimer previousMillisSensor;

ESP8266WebServer server(80);

// Define os pinos para as bombas
#define pinBombaEntrada 4
#define pinBombaSaida 5
#define pinAlarme 16
#define pinBotaoResetAlarme 2
#define pinBotaoLigaBombaSaida 15
#define pinAlarmeNivelAlto 14

int botaoResetaAlarme = 0;
int alarmeLigado = 0;
int botaoLigaBombaSaida = 0;
int zerarContadorNivel = 0;
float nivel;

void handleRoot()
{
    server.sendHeader("Location", "/index.html");
    server.send(302, "text/plain", "");
}

void handleComando()
{
    if (server.hasArg("plain"))
    {
        String body = server.arg("plain");
        DynamicJsonDocument jsonBuffer(1024);
        DeserializationError error = deserializeJson(jsonBuffer, body);
        if (error)
        {
            server.send(400, "application/json", "{\"status\":\"erro\",\"mensagem\":\"JSON inválido\"}");
            return;
        }
        int pino = jsonBuffer["pino"];
        for (int i = 0; i < nArrayDispositivos; i++)
        {
            if (deviceList[i].pin == pino)
            {
                if (pino == 2)
                {
                    botaoResetaAlarme = 1;
                }
                toggleDeviceState(&deviceList[i]);
                inserirHistoricoDispositivo(&deviceList[i]);
            }
        }
        server.send(200, "application/json", "{\"status\":\"sucesso\",\"mensagem\":\"Comando recebido\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"status\":\"erro\",\"mensagem\":\"Requisição sem corpo\"}");
    }
}

void handleDispositivos()
{
    DynamicJsonDocument jsonBuffer = getDeviceData();
    String response;
    serializeJson(jsonBuffer, response);
    server.send(200, "application/json", response);
}

void handleSensores()
{
    String response;
    DynamicJsonDocument jsonBuffer = getSensorData();
    serializeJson(jsonBuffer, response);
    server.send(200, "application/json", response);
}

void ReconfiguraWiFi();
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);
void atualizaHistoricoSensor();
void tratamentoSensorDispositivos();

void setup()
{
    Serial.begin(115200);

    pinMode(pinBombaEntrada, OUTPUT);
    pinMode(pinBombaSaida, OUTPUT);
    pinMode(pinAlarme, OUTPUT);

    pinMode(pinBotaoLigaBombaSaida, INPUT_PULLUP);
    pinMode(pinAlarmeNivelAlto, INPUT_PULLUP);
    pinMode(pinBotaoLigaBombaSaida, INPUT_PULLUP);
    pinMode(pinBotaoResetAlarme, INPUT_PULLUP);

    if (!SPIFFS.begin())
    {
        Serial.println("Erro ao montar o sistema de arquivos SPIFFS");
        return;
    }

    WiFi.config(staticIP, gateway, subnet);
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(60);
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.autoConnect("ESP8266_AP", "123456789");

    if (!MDNS.begin("dispositivos"))
    {
        Serial.println("Erro ao configurar mDNS responder!");
        while (1)
        {
            delay(100);
        }
    }
    Serial.println("Conectado à rede WiFi!");
    Serial.println("mDNS configurado. Acesse http://dispositivos.local");
    MDNS.addService("http", "tcp", 80);

    digitalWrite(pinAlarme, LOW);              // Desliga Alarme
    digitalWrite(pinBotaoLigaBombaSaida, LOW); // Desliga Alarme
    digitalWrite(pinBotaoResetAlarme, LOW);    // Desliga Alarme

    previousMillisHistorico.set(10000);
    previousMillisSensor.set(1500);

    server.on("/", handleRoot);
    server.on("/api/sensores", HTTP_GET, handleSensores);
    server.on("/api/dispositivos", HTTP_GET, handleDispositivos);
    server.on("/api/comando", HTTP_POST, handleComando);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
    Serial.println("Servidor HTTP iniciado");

    setupDevices();    
    setupSensor();
}

void loop()
{
    MDNS.update();
    if (WiFi.status() == WL_CONNECTED)
    {
        server.handleClient();
    }
    else
    {
        ReconfiguraWiFi();
    }

    int btnResetAlarme = digitalRead(pinBotaoResetAlarme);
    botaoLigaBombaSaida = digitalRead(pinBotaoLigaBombaSaida);
    alarmeLigado = digitalRead(pinAlarme);

    if (btnResetAlarme == 0)
    {
        botaoResetaAlarme = 1;
    }
    if (alarmeLigado && botaoResetaAlarme)
    {
        digitalWrite(pinAlarme, LOW); // Desliga Alarme
        Serial.println("Alarme Desligado!");
        atualizaHistoricoDispositivos(pinAlarme, 0);
        handleDispositivos(); // Envia atualização para o servidor
    }
    if (botaoLigaBombaSaida)
    {
        botaoLigaBombaSaida = 1;
    }
    if (previousMillisHistorico.repeat())
    {
        atualizaHistoricoSensor();
    }
    if (previousMillisSensor.repeat() || botaoLigaBombaSaida || nivel == 0)
    {
        tratamentoSensorDispositivos();
    }
    delay(100);
}

void ReconfiguraWiFi()
{
    WiFiManager wifiManager;
    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED)
    {
        if (WiFi.status() == WL_NO_SSID_AVAIL || WiFi.status() == WL_CONNECT_FAILED)
        {
            wifiManager.setConfigPortalTimeout(60);
            if (!wifiManager.startConfigPortal("ESP8266_AP", "123456789"))
            {
                Serial.println("Falha ao iniciar o portal de configuração");
            }
        }
    }
}

void saveConfigCallback()
{
    Serial.println("Configuração Salva");
    Serial.println(WiFi.softAPIP());
}

void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entrou no Modo de Configuração!!!");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

void tratamentoSensorDispositivos()
{
    // Lógica para controlar as bombas e o alarme com base na leitura do sensor de nível
    nivel = getAlturaNivel().toFloat();
    int bombaEntradaLigada = digitalRead(pinBombaEntrada);
    int bombaSaidaLigada = digitalRead(pinBombaSaida);
    if (nivel == 0)
    {
        zerarContadorNivel++;
        if (zerarContadorNivel >= 10 && !alarmeLigado && !botaoResetaAlarme)
        {
            digitalWrite(pinAlarme, HIGH); // Liga Alarme
            atualizaHistoricoDispositivos(pinAlarme, 1);
            handleDispositivos(); // Envia atualização para o servidor
        }
    }
    else
    {
        zerarContadorNivel = 0;
        if (!bombaEntradaLigada && !bombaSaidaLigada && !alarmeLigado)
        {

            if (nivel >= 12 && nivel <= 14)
            {
                digitalWrite(pinBombaEntrada, HIGH); // Liga Bomba Entrada
                Serial.println("Bomba de Entrada Ligada!");
                atualizaHistoricoDispositivos(pinBombaEntrada, 1);
                handleDispositivos(); // Envia atualização para o servidor
            }
            else if (nivel <= 4 && nivel >= 3 || botaoLigaBombaSaida == 1)
            {
                digitalWrite(pinBombaSaida, HIGH); // Liga Bomba Saída
                Serial.println("Bomba de Saída Ligada!");
                atualizaHistoricoDispositivos(pinBombaSaida, 1);
                handleDispositivos(); // Envia atualização para o servidor
            }
        }
        else if (bombaEntradaLigada && nivel <= 5)
        {
            digitalWrite(pinBombaEntrada, LOW); // Desliga Bomba Entrada
            Serial.println("Bomba de Entrada Desligada!");
            atualizaHistoricoDispositivos(pinBombaEntrada, 0);
            handleDispositivos(); // Envia atualização para o servidor
        }
        else if (bombaSaidaLigada && nivel >= 12 || botaoLigaBombaSaida == 1)
        {
            digitalWrite(pinBombaSaida, LOW); // Desliga Bomba Saída
            Serial.println("Bomba de Saída Desligada!");
            atualizaHistoricoDispositivos(pinBombaSaida, 0);
            handleDispositivos(); // Envia atualização para o servidor
        }

        if (!alarmeLigado && !botaoResetaAlarme)
        {
            if ((nivel >= 14) || (nivel <= 2))
            {
                digitalWrite(pinAlarme, HIGH); // Liga Alarme
                Serial.println("Alarme Ligado!");
                if (bombaEntradaLigada)
                {
                    digitalWrite(pinBombaEntrada, LOW); // Desliga Bomba Entrada
                    Serial.println("Bomba de Entrada Desligada!");
                    atualizaHistoricoDispositivos(pinBombaEntrada, 0);
                    handleDispositivos(); // Envia atualização para o servidor
                }
                else if (bombaSaidaLigada)
                {
                    digitalWrite(pinBombaSaida, LOW); // Desliga Bomba Saída
                    Serial.println("Bomba de Saída Desligada!");
                    atualizaHistoricoDispositivos(pinBombaSaida, 0);
                    handleDispositivos(); // Envia atualização para o servidor
                }
                atualizaHistoricoDispositivos(pinAlarme, 1);
                handleDispositivos(); // Envia atualização para o servidor
            }
        }
        else if ((alarmeLigado || botaoResetaAlarme) && (nivel <= 12 && nivel >= 3))
        {
            if (alarmeLigado)
            {
                digitalWrite(pinAlarme, LOW); // Desliga Alarme
                Serial.println("Alarme Desligado!");
                atualizaHistoricoDispositivos(pinAlarme, 0);
                handleDispositivos(); // Envia atualização para o servidor
            }
            botaoResetaAlarme = 0;
        }
    }
}

void pauseUntilEnter()
{
    Serial.println("Pressione Enter para continuar...");
    while (true)
    {
        if (Serial.available() > 0)
        {                           // Verifica se há dados disponíveis na entrada serial
            char c = Serial.read(); // Lê o caractere da entrada serial
            if (c == '\n')
            {          // Verifica se o caractere é Enter (nova linha)
                break; // Sai do loop
            }
        }
    }
}