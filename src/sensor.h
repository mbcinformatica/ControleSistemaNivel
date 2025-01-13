#include <ArduinoJson.h>
#include <Ultrasonic.h>

#define pinUltrasonTrig 12
#define pinUltrasonEcho 13

Ultrasonic ultrasonic(pinUltrasonTrig, pinUltrasonEcho);

struct DeviceSensor
{
    const char *idtipo;
    const char *name;
    const char *unit;
    const char *imgon;
    const char *imgoff;
    String (*getValue)();
    String identifier;
};

const int nArraySensor = 1;
void inserirSensor(DeviceSensor *devicesensor);
void inserirHistoricoSensor(DeviceSensor *devicesensor);

String getAlturaNivel();

DeviceSensor DeviceSensorList[nArraySensor] = {
    {"sensorNivel", "Status Sensor Nivel", "CM", "img/CaixaVazia.gif", "img/CaixaCheia.gif", getAlturaNivel, "sensor_nivel"}};

String getAlturaNivel()
{
    float sensorNivel = ultrasonic.read();
    return String(sensorNivel);
}

const char *getImagem(const char *valor, const char *identificador)

{
    static String imagemSensor;
    if (strcmp(identificador, "sensor_nivel") == 0)
    {
        if (atof(valor) <= 5 && atof(valor) >= 4)
        {
            imagemSensor = "img/CaixaCheia.gif";
        }
        else if (atof(valor) >= 7 && atof(valor) <= 9)
        {
            imagemSensor = "img/Caixa75.gif";
        }
        else if (atof(valor) >= 9 && atof(valor) <= 11)
        {
            imagemSensor = "img/Caixa50.gif";
        }
        else if (atof(valor) >= 11 && atof(valor) <= 12)
        {
            imagemSensor = "img/Caixa25.gif";
        }
        else if (atof(valor) >= 12 && atof(valor) <= 14)
        {
            imagemSensor = "img/CaixaVazia.gif";
        }
        else if (atof(valor) >= 14 || atof(valor) <= 3)
        {
            imagemSensor = "img/Alarme_On.gif";
        }
    }
    else
    {

        imagemSensor = "img/SemImagem.gif";
    }
    return imagemSensor.c_str();
}

DynamicJsonDocument getSensorData()
{
    DynamicJsonDocument jsonBuffer(1024);
    JsonArray sensor = jsonBuffer.createNestedArray("sensor");
    for (int i = 0; i < nArraySensor; i++)
    {
        JsonObject Sensor = sensor.createNestedObject();
        Sensor["id"] = DeviceSensorList[i].idtipo;
        Sensor["nome"] = DeviceSensorList[i].name;
        Sensor["unidade"] = DeviceSensorList[i].unit;
        Sensor["valor"] = DeviceSensorList[i].getValue();
        Sensor["identificador"] = DeviceSensorList[i].identifier;
        Sensor["imagen"] = getImagem(Sensor["valor"], Sensor["identificador"]);
    }
    return jsonBuffer;
}

void setupSensor()
{
    pinMode(pinUltrasonTrig, OUTPUT);
    pinMode(pinUltrasonEcho, INPUT);
    for (int i = 0; i < nArraySensor; i++)
    {
        inserirSensor(&DeviceSensorList[i]);
    }
}

void atualizaHistoricoSensor()
{
    for (int i = 0; i < nArraySensor; i++)
    {

        inserirHistoricoSensor(&DeviceSensorList[i]);
    }
}

void inserirSensor(DeviceSensor *devicesensor)
{
    HTTPClient http;
    String urlSensor = String(site_url) + "?action=inserir-sensor&name=" + String(urlencode(devicesensor->name)) +
                       "&unit=" + String(urlencode(devicesensor->unit)) +
                       "&imgon=" + String(urlencode(devicesensor->imgon)) +
                       "&imgoff=" + String(urlencode(devicesensor->imgoff)) +
                       "&identifier=" + String(devicesensor->identifier);
    http.begin(wifiClient, urlSensor);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    }
    else
    {
        Serial.print("Erro ao enviar: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void inserirHistoricoSensor(DeviceSensor *devicesensor)
{
    HTTPClient http;

    String urlSensor = String(site_url) + "?action=inserir-historico-sensor&identifier=" + String(devicesensor->identifier) + "&valor=" + String(urlencode(devicesensor->getValue()));
    http.begin(wifiClient, urlSensor);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    }
    else
    {
        Serial.print("Erro ao enviar: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}