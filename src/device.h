#include <ArduinoJson.h> // Inclui a biblioteca ArduinoJson para manipulação de JSON

// Estrutura para armazenar informações dos dispositivos
struct Device
{
    int pin;             // Pino do Arduino ao qual o dispositivo está conectado
    int state;           // Estado atual do dispositivo (ligado/desligado)
    const char *name;    // Nome do dispositivo
    const char *imgon;   // Caminho da imagem quando o dispositivo está ligado
    const char *imgoff;  // Caminho da imagem quando o dispositivo está desligado
    int widthimg;        // Largura da imagem
    int heightimg;       // Altura da imagem
    String paddingimg;   // Preenchimento da imagem
    String identifier;   // Identificador do dispositivo
    const char *pintype; // Tipo do pino (OUTPUT, INPUT, etc.)
    bool showbutton;     // Indica se o botão deve ser exibido
};

const int nArrayDispositivos = 4; // Número de dispositivos

// Declaração das funções
void setupDevices();
void inserirDispositivo(Device *device);
void inserirHistoricoDispositivo(Device *device);
void atualizaHistoricoDispositivos(int pino, bool estado);

// Lista de dispositivos com suas propriedades
Device deviceList[nArrayDispositivos] = {
    {4, 0, "Status Bomba Entrada", "img/Bomba_On.gif", "img/Bomba_Off.gif", 100, 100, "5px -3px", "bomba_entrada", "OUTPUT", true},
    {5, 0, "Status Bomba Saida", "img/Bomba_On.gif", "img/Bomba_Off.gif", 100, 100, "5px -3px", "bomba_saida", "OUTPUT", true},
    {2, 0, "Botao Reset", "", "", 0, 0, "", "botao_reset", "INPUT", false},
    {16, 0, "Status do Sistema/Alarme", "img/Alarme_On.gif", "img/Alarme_Off.gif", 100, 100, "11px 2px 3px 44px", "alarme_sistema", "OUTPUT", false}};

// Função para obter os dados dos dispositivos em formato JSON
DynamicJsonDocument getDeviceData()
{
    DynamicJsonDocument jsonBuffer(1024);
    JsonArray dispositivos = jsonBuffer.createNestedArray("dispositivos");

    for (int i = 0; i < nArrayDispositivos; i++)
    {
        JsonObject dispositivo = dispositivos.createNestedObject();
        dispositivo["pino"] = deviceList[i].pin;
        dispositivo["estado"] = deviceList[i].state;
        dispositivo["nome"] = deviceList[i].name;
        dispositivo["imageon"] = deviceList[i].imgon;
        dispositivo["imageoff"] = deviceList[i].imgoff;
        dispositivo["largura"] = deviceList[i].widthimg;
        dispositivo["altura"] = deviceList[i].heightimg;
        dispositivo["preenchimento"] = deviceList[i].paddingimg;
        dispositivo["tipopino"] = deviceList[i].pintype;
        dispositivo["mostrabotao"] = deviceList[i].showbutton;
        dispositivo["identificador"] = deviceList[i].identifier;
    }
    return jsonBuffer;
}

// Função para codificar uma string em URL
String urlencode(const String &str)
{
    String encoded = "";
    for (size_t i = 0; i < str.length(); i++)
    {
        char c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded += c;
        }
        else
        {
            encoded += '%';
            encoded += String((unsigned char)c, HEX);
        }
    }
    return encoded;
}

// Função para alternar o estado de um dispositivo
void toggleDeviceState(Device *device)
{
    device->state = !device->state;
    digitalWrite(device->pin, device->state);
}

// Função para configurar os dispositivos
void setupDevices()
{
    for (int i = 0; i < nArrayDispositivos; i++)
    {
        if (strcmp(deviceList[i].pintype, "OUTPUT") == 0)
        {
            pinMode(deviceList[i].pin, OUTPUT);
            digitalWrite(deviceList[i].pin, deviceList[i].state);
        }
        else if (strcmp(deviceList[i].pintype, "INPUT") == 0)
        {
            pinMode(deviceList[i].pin, INPUT_PULLUP);
        }
        inserirDispositivo(&deviceList[i]);
    }
}

// Função para inserir um dispositivo no servidor
void inserirDispositivo(Device *device)
{
    HTTPClient http;
    String urlDispositivo = String(site_url) + "?action=inserir-dispositivo&name=" + urlencode(device->name) +
                            "&pin=" + String(device->pin) + "&imgon=" + urlencode(device->imgon) +
                            "&imgoff=" + urlencode(device->imgoff) + "&widthimg=" + String(device->widthimg) +
                            "&heightimg=" + String(device->heightimg) + "&paddingimg=" + urlencode(device->paddingimg) +
                            "&identifier=" + urlencode(device->identifier) + "&pintype=" + urlencode(device->pintype) +
                            "&showbutton=" + String(device->showbutton);

    http.begin(wifiClient, urlDispositivo);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        Serial.print("Dispositivo incluído com sucesso.");
        Serial.println(urlDispositivo);
    }
    else
    {
        Serial.print("Erro ao incluir dispositivo: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

// Função para atualizar o histórico de dispositivos
void atualizaHistoricoDispositivos(int pino, bool estado)
{
    for (int i = 0; i < nArrayDispositivos; i++)
    {
        if (deviceList[i].pin == pino)
        {
            deviceList[i].state = estado;
            inserirHistoricoDispositivo(&deviceList[i]);
        }
    }
}

// Função para inserir o histórico de um dispositivo no servidor
void inserirHistoricoDispositivo(Device *device)
{
    HTTPClient http;

    String urlDispositivo = String(site_url) + "?action=inserir-historico-dispositivo&identifier=" +
                            urlencode(device->identifier) + "&state=" + String(device->state);

    http.begin(wifiClient, urlDispositivo);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        Serial.println("Histórico do dispositivo registrado com sucesso.");
        Serial.println(urlDispositivo);
    }
    else
    {
        Serial.print("Erro ao registrar histórico do dispositivo: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}