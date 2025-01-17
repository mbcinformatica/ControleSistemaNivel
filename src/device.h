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
    int showbutton;      // Estado atual do dispositivo (ligado/desligado)
};

const int nArrayDispositivos = 4; // Número de dispositivos

// Declaração das funções
void setupDevices();
void inserirDispositivo(Device *device);
void inserirHistoricoDispositivo(Device *device);
void atualizaHistoricoDispositivos();

// Lista de dispositivos com suas propriedades
Device deviceList[nArrayDispositivos] = {
    //{Pino-Arduino, Estado-Pino, "Nome Dispositivo", "Imagem Disp. On", "Imagem Disp. Off", Largura, Altura, "Preenchimento"}
    {4, false, "Status Bomba Entrada", "img/Bomba_On.gif", "img/Bomba_Off.gif", 100, 100, "5px -3px", "bomba_entrada", "OUTPUT", true},
    {5, false, "Status Bomba Saida", "img/Bomba_On.gif", "img/Bomba_Off.gif", 100, 100, "5px -3px", "bomba_saida", "OUTPUT", true},
    {2, false, "Botão Reset", "", "", 0, 0, "", "botao_reset", "INPUT", false},
    {16, false, "Status Operac. Sistema", "img/Alarme_On.gif", "img/Alarme_Off.gif", 100, 100, "11px 2px 3px 44px", "alarme_sistema", "OUTPUT", false}};

// Função para obter os dados dos dispositivos em formato JSON
DynamicJsonDocument getDeviceData()
{
    DynamicJsonDocument jsonBuffer(1024);                                  // Cria um buffer JSON
    JsonArray dispositivos = jsonBuffer.createNestedArray("dispositivos"); // Cria um array JSON para os dispositivos

    for (int i = 0; i < nArrayDispositivos; i++)
    {
        JsonObject dispositivo = dispositivos.createNestedObject(); // Cria um objeto JSON para cada dispositivo
        dispositivo["pino"] = deviceList[i].pin;
        dispositivo["estado"] = deviceList[i].state;             // Adiciona o estado do dispositivo
        dispositivo["nome"] = deviceList[i].name;                // Adiciona o nome do dispositivo
        dispositivo["imageon"] = deviceList[i].imgon;            // Adiciona a imagem quando o dispositivo está ligado
        dispositivo["imageoff"] = deviceList[i].imgoff;          // Adiciona a imagem quando o dispositivo está desligado
        dispositivo["largura"] = deviceList[i].widthimg;         // Adiciona a largura da imagem
        dispositivo["altura"] = deviceList[i].heightimg;         // Adiciona a altura da imagem
        dispositivo["preenchimento"] = deviceList[i].paddingimg; // Adiciona o preenchimento da imagem
        dispositivo["tipopino"] = deviceList[i].pintype;
        dispositivo["mostrabotao"] = deviceList[i].showbutton;
        dispositivo["identificador"] = deviceList[i].identifier;
    }
    return jsonBuffer; // Retorna o buffer JSON
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
            // Caracteres seguros
            encoded += c;
        }
        else if (c < 0)
        {
            // Codifica caracteres UTF-8
            encoded += "%";
            encoded += String((unsigned char)c, HEX);
        }
        else
        {
            // Para caracteres especiais
            encoded += "%";
            encoded += String((unsigned char)c, HEX);
        }
    }
    return encoded;
}

// Função para alternar o estado de um dispositivo
void toggleDeviceState(Device *device)
{
    device->state = !device->state;           // Alterna o estado do dispositivo
    digitalWrite(device->pin, device->state); // Escreve o novo estado no pino do dispositivo
}

// Função para configurar os dispositivos
void setupDevices()
{
    for (int i = 0; i < nArrayDispositivos; i++)
    {
        if (deviceList[i].pintype == "OUTPUT")
        {
            pinMode(deviceList[i].pin, OUTPUT);                   // Define o pino do dispositivo
            digitalWrite(deviceList[i].pin, deviceList[i].state); // Define o estado inicial do dispositivo
        }
        else if (deviceList[i].pintype == "INPUT")
        {
            pinMode(deviceList[i].pin, INPUT_PULLUP);                   // Define o pino do dispositivo
            digitalWrite(deviceList[i].pin, deviceList[i].state); // Define o estado inicial do dispositivo
        }
        inserirDispositivo(&deviceList[i]); // Insere o dispositivo na lista
    }
}

void inserirDispositivo(Device *device)
{

    HTTPClient http; // Cria um objeto HTTPClient para realizar a requisição HTTP
    // Monta a URL para inserir o dispositivo, incluindo todos os parâmetros necessários
    String urlDispositivo = String(site_url) + "?action=inserir-dispositivo&name=" + String(urlencode(device->name)) +
                            "&pin=" + (device->pin) + "&imgon=" + String(urlencode(device->imgon)) + "&imgoff=" + String(urlencode(device->imgoff)) +
                            "&widthimg=" + (device->widthimg) + "&heightimg=" + (device->heightimg) + "&paddingimg=" + String(urlencode(device->paddingimg)) + "&identifier=" + String(device->identifier) + "&pintype=" + String(urlencode(String(device->pintype))) + "&showbutton=" + (device->showbutton);

    http.begin(wifiClient, urlDispositivo);             // Inicia a conexão HTTP com a URL especificada
    http.addHeader("Content-Type", "application/json"); // Adiciona o cabeçalho de conteúdo JSON
    int httpResponseCode = http.GET();                  // Envia a requisição GET e armazena o código de resposta

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Obtém a resposta do servidor
        Serial.println(httpResponseCode);   // Imprime o código de resposta no console
        Serial.println(response);           // Imprime a resposta do servidor no console
    }
    else
    {
        Serial.print("Erro ao enviar: "); // Imprime uma mensagem de erro no console
        Serial.println(httpResponseCode); // Imprime o código de erro no console
    }

    // Finaliza a conexão HTTP
    http.end();
}
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
void inserirHistoricoDispositivo(Device *device)
{
    HTTPClient http; // Cria um objeto HTTPClient para realizar a requisição HTTP
    // Monta a URL para inserir o histórico do dispositivo, incluindo o identificador e o estado do dispositivo
    String urlDispositivo = String(site_url) + "?action=inserir-historico-dispositivo&identifier=" + String(device->identifier) + "&state=" + (device->state);

    http.begin(wifiClient, urlDispositivo);             // Inicia a conexão HTTP com a URL especificada
    http.addHeader("Content-Type", "application/json"); // Adiciona o cabeçalho de conteúdo JSON
    int httpResponseCode = http.GET();                  // Envia a requisição GET e armazena o código de resposta

    // Verifica a resposta do servidor
    if (httpResponseCode > 0)
    {
        String response = http.getString(); // Obtém a resposta do servidor
        Serial.println(httpResponseCode);   // Imprime o código de resposta no console
        Serial.println(response);           // Imprime a resposta do servidor no console
    }
    else
    {
        Serial.print("Erro ao enviar: "); // Imprime uma mensagem de erro no console
        Serial.println(httpResponseCode); // Imprime o código de erro no console
    }

    // Finaliza a conexão HTTP
    http.end();
}
