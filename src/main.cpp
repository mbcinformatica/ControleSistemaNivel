#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <Ultrasonic.h>
#include <neotimer.h>
#include "config.h" 
#include <FS.h>


WiFiClient wifiClient;
IPAddress staticIP;
IPAddress gateway;
IPAddress subnet;
IPAddress webServer;

#include <device.h>
#include <sensor.h>

Neotimer previousMillisHistorico(5000);
Neotimer previousMillisSensor(1500);

ESP8266WebServer server(80);

// Define os pinos para as bombas e alarmes
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
float nivelAnterior = 0;

void configurarRede();
void configurarPinos();
void configurarServidor();
void tratamentoSensorDispositivos();
void ReconfiguraWiFi();
void saveConfigCallback();
void configModeCallback(WiFiManager *myWiFiManager);
void handleRoot();
void handleComando();
void handleDispositivos();
void handleSensores();

void setup()
{
    Serial.begin(115200);

    configurarPinos();
    configurarRede();
    configurarServidor();

    Serial.println("Sistema iniciado com sucesso!");
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

    if ((previousMillisHistorico.repeat()) && (nivel != nivelAnterior))
    {
        if (nivel != nivelAnterior)
        {
            atualizaHistoricoSensor();
            nivelAnterior = nivel;
        }
    }

    if (previousMillisSensor.repeat() || botaoLigaBombaSaida || nivel == 0)
    {
        tratamentoSensorDispositivos();
    }
}

void configurarRede()
{
    // Converte strings de IP para IPAddress
    staticIP.fromString(vStaticIP);
    gateway.fromString(vGateway);
    subnet.fromString(vSubnet);
    webServer.fromString(vWebServer);

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
            delay(100);
    }

    Serial.println("Conectado à rede WiFi!");
    Serial.println("mDNS configurado. Acesse http://dispositivos.local");
    MDNS.addService("http", "tcp", 80);
}

void configurarPinos()
{
    pinMode(pinBombaEntrada, OUTPUT);
    pinMode(pinBombaSaida, OUTPUT);
    pinMode(pinAlarme, OUTPUT);
    pinMode(pinBotaoLigaBombaSaida, INPUT_PULLUP);
    pinMode(pinAlarmeNivelAlto, INPUT_PULLUP);
    pinMode(pinBotaoResetAlarme, INPUT_PULLUP);

    digitalWrite(pinAlarme, LOW);
    digitalWrite(pinBombaEntrada, LOW);
    digitalWrite(pinBombaSaida, LOW);
}

void configurarServidor()
{

    previousMillisHistorico.set(5000);
    previousMillisSensor.set(1500);

    setupSensor();  // Configura os sensores
    setupDevices(); // Configura os dispositivos

    server.on("/", handleRoot);
    server.on("/api/sensores", HTTP_GET, handleSensores);
    server.on("/api/dispositivos", HTTP_GET, handleDispositivos);
    server.on("/api/comando", HTTP_POST, handleComando);
    server.serveStatic("/", SPIFFS, "/");

    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

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
                if (pino == pinBotaoResetAlarme)
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
    DynamicJsonDocument jsonBuffer = getSensorData();
    String response;
    serializeJson(jsonBuffer, response);
    server.send(200, "application/json", response);
}

void ReconfiguraWiFi()
{
    WiFiManager wifiManager;
    if (WiFi.status() != WL_CONNECTED)
    {
        wifiManager.setConfigPortalTimeout(60);
        if (!wifiManager.startConfigPortal("ESP8266_AP", "123456789"))
        {
            Serial.println("Falha ao iniciar o portal de configuração");
        }
    }
}

void saveConfigCallback()
{
    Serial.println("Configuração salva");
    Serial.println(WiFi.softAPIP());
}

void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entrou no modo de configuração!");
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
            else if ((nivel <= 4 && nivel >= 3) || botaoLigaBombaSaida == 1)
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
        else if (((bombaSaidaLigada) && (nivel >= 12)) || (botaoLigaBombaSaida == 1))
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