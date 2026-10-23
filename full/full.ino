#include <Arduino.h>
#include "SoftwareSerial.h"
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// GSM
#define TINY_GSM_MODEM_A6
#include <TinyGsmClient.h>
SoftwareSerial SerialAT(13, 15); // RX, TX
TinyGsm modem(SerialAT);

/* Datos de Soft ap */
IPAddress apIP(192, 168, 4, 1); // Ip de ap
IPAddress netMsk(255, 255, 255, 0); // Submascara de ap
boolean apConnected = false;
unsigned long apStarted = 0;

ESP8266WebServer server(80); // Servidor web
File fsUploadFile; // Archivo usado en filebrowser upload/create, etc.

const int SENSOR = 5; // Pin conectado a reed switch nivel 1
const int MOTOR = 12;// Boton de input
long alertInterval = 300000; // Tiempo de espera entre alertas (5 min)
unsigned long alertPrevMillis = 0; // Variable de ultimo tiempo en que se tomo el nivel
String readString;
String atComm;

struct ConfigSettingsStruct
{
  String apSsid;
  String apPass;
  long apTimeout;
  String phoneno;
  String phoneno2;
  String alertmsg;
};

ConfigSettingsStruct ConfigSettings;

void setup() {
  Serial.begin(115200);
  SerialAT.begin(115200);
  getConfigData("/config.json");
  setupSensor();
  setupServer();
  startAp();
  modem.init();
}

void setupSensor() {
  // Setear modo de pins de reed switchs
  pinMode(SENSOR, INPUT_PULLUP);
  pinMode(MOTOR, OUTPUT);
  digitalWrite(MOTOR, LOW);
}

/*
 * Chekear estado de AP y desconectar si pasaron N minutos
 */
void checkAp(){
  if(apConnected){
    unsigned long currentMills = millis();
    if(apStarted > 0 && (unsigned long)(currentMills - apStarted) >= ConfigSettings.apTimeout){
      stopAp();
    }
  }else{
  }
}

/*
   Iniciar softAP
*/
void startAp() {
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ConfigSettings.apSsid.c_str(), ConfigSettings.apPass.c_str()); // Dejar _apPass para que sea abierto
  delay(500);
  apConnected = true;
  apStarted = millis();
  Serial.println("AP Iniciado");
  Serial.println("Host: " + String(ConfigSettings.apSsid) + " - Pass: " + String(ConfigSettings.apPass) );
}
/*
 * Desconectar AP
 */
void stopAp(){
  Serial.println("AP desconectado");
  WiFi.softAPdisconnect(true);
  apConnected = false;
  apStarted = 0;
}

/*
   servidor HTTP
*/
void setupServer() {
  server.on("/", HTTP_GET, []() { handleFileRead("/index.html"); });
  server.on("/config", HTTP_GET , handleGetConfig);
  server.on("/config", HTTP_POST, handlePostConfig);
  server.on("/reset", handleResetDefault);
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin(); // Iniciar servidor web
}

/*

   Obtener tipo de archivo para abrir.
*/
String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

String listDir(String path) {

  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";

  return output;
}

/*
   Abrir archivo
*/
bool handleFileRead(String path) {

  // Si el nombre es "/" abrir index.
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";

  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {

    if (SPIFFS.exists(pathWithGz)){
      path += ".gz";
    }

    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

/*
   Devuelve info de gsm como String Json
*/

String getGsmInfo() {

  String json = "{";
  if (modem.init()) {
    json += " \"cop\" : \"" + modem.getOperator() + "\"";
    json += ", \"csq\" : \"" + String(modem.getSignalQuality()) + "\"";
    json += ", \"battLevel\" : \"" + String(modem.getBattPercent()) + "\"";
  }
  json += "}";
  
  return json;
}

/*
  Ruta POST para obtener config
  Json
*/
void handleGetConfig() {

  String json = "{";
  json += "\"apip\" : \"" + toStringIp(apIP) + "\"";
  json += ", \"config\" : " + getConfigAsString();
  json += ", \"gsm\" : " + getGsmInfo();
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "Content-Type: application/json;charset=utf-8", json);
}

/*
   Ruta POST para guardar datos en config.json
*/
void handlePostConfig() {

  ConfigSettings.apPass = server.arg("appass");
  ConfigSettings.apSsid = server.arg("apssid");
  ConfigSettings.apTimeout = server.arg("aptimeout").toInt();
  ConfigSettings.phoneno = server.arg("phoneno");
  ConfigSettings.phoneno2 = server.arg("phoneno2");
  ConfigSettings.alertmsg = server.arg("alertmsg");

  String res = saveConfig();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send ( 200, "Content-Type: application/json;charset=utf-8", res);
  stopAp();
  startAp();
}

String resetConfigDefault(){
  getConfigData("/config.default.json");
  String res = saveConfig();
  return res;
}

String handleResetDefault(){
  String res = resetConfigDefault();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send ( 200, "Content-Type: application/json;charset=utf-8", res);
}

String saveConfig() {
  String res; // Respues a enviar

  String json = "{";
  json += "\"appass\" : \"" + String(ConfigSettings.apPass) + "\"";
  json += ", \"apssid\" : \"" + String(ConfigSettings.apSsid) + "\"";
  json += ", \"aptimeout\" : \"" + String(ConfigSettings.apTimeout) + "\"";
  json += ", \"phoneno\" : \"" + String(ConfigSettings.phoneno) + "\"";
  json += ", \"phoneno2\" : \"" + String(ConfigSettings.phoneno2) + "\"";
  json += ", \"alertmsg\" : \"" + String(ConfigSettings.alertmsg) + "\"";
  json += "}";

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    res = "{ \"error\" : true , \"message\" : \"Error al abrir archivo de configuracion.\" }";
  } else {
    configFile.print(json);
    res = "{ \"success\" : true , \"message\" : \"Configuraacion guardada.\" }";
  }

  configFile.close();
  return res;
}

/*
   Convertir IP a String
*/
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

/*
   Leer info de config.json
*/
bool getConfigData(String sourceFile) {

  if (!SPIFFS.begin()) {
    Serial.println("[ERROR] - bool getConfigData() -> SPIFFS.begin()");
    Serial.println("        - Fallo al montar SPIFFS");
    return false;
  }

  File configFile = SPIFFS.open(sourceFile, "r");
  if (!configFile) {
    Serial.println("[ERROR] - bool getConfigData() -> SPIFFS.open(\"config.json\", \"r\")");
    Serial.println("        - Fallo al abrir el archivo de configuracion");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("");
    Serial.println("[ERROR] - bool getConfigData() -> configFile.size()");
    Serial.println("        - Archivo de configuracion demasiado grande.");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> json;
  // JsonObject& json = jsonBuffer.parseObject(buf.get());

  // if (!json.success()) {
    // Serial.println("Failed to parse config file");
    // return false;
  // }

  DeserializationError error = deserializeJson(json, buf.get());
  if (error){
    Serial.println("Failed to parse config file");
    return false;
  }

  char _apssid_[30];
  strcpy(_apssid_, json["apssid"]);
  ConfigSettings.apSsid = String(_apssid_);

  char _appass_[30];
  strcpy(_appass_, json["appass"]);
  ConfigSettings.apPass = String(_appass_);
  
  char _aptimeout_[30];
  strcpy(_aptimeout_, json["aptimeout"]);
  ConfigSettings.apTimeout = atoi(_aptimeout_);

  char _phoneno_[30];
  strcpy(_phoneno_, json["phoneno"]);
  ConfigSettings.phoneno = String(_phoneno_);

  char _phoneno2_[30];
  strcpy(_phoneno2_, json["phoneno2"]);
  ConfigSettings.phoneno2 = String(_phoneno2_);

  char _alertmsg_[100];
  strcpy(_alertmsg_, json["alertmsg"]);
  ConfigSettings.alertmsg = String(_alertmsg_);

  return true;

}

/*
    Devuelve contenido de config.json en String Json
*/
String getConfigAsString() {

  String json = "{";
  json += "\"appass\" : \"" + String(ConfigSettings.apPass) + "\"";
  json += ", \"apssid\" : \"" + String(ConfigSettings.apSsid) + "\"";
  json += ", \"aptimeout\" : \"" + String(ConfigSettings.apTimeout) + "\"";
  json += ", \"phoneno\" : \"" + String(ConfigSettings.phoneno) + "\"";
  json += ", \"phoneno2\" : \"" + String(ConfigSettings.phoneno2) + "\"";
  json += ", \"alertmsg\" : \"" + String(ConfigSettings.alertmsg) + "\"";
  json += "}";

  return json;

}

void checkLevel() {

  unsigned long currentMillis = millis();
  if (digitalRead(SENSOR) == HIGH) // If the pin reads low, the switch is closed.
  {
    digitalWrite(MOTOR, HIGH);
    if ( ((unsigned long)(currentMillis - alertPrevMillis) >= alertInterval) || alertPrevMillis == 0 ) {
      sendLevel();
      alertPrevMillis = currentMillis;
    }
  } else {
    digitalWrite(MOTOR, LOW);
  }

}

void sendLevel() {
  
  Serial.println("NOTIFICACION 1");
  modem.sendSMS(ConfigSettings.phoneno, "Usuario 1: "  + ConfigSettings.alertmsg);
  if (ConfigSettings.phoneno2 && ConfigSettings.phoneno2 != "") {
    Serial.println("NOTIFICACION 2");
    modem.sendSMS(ConfigSettings.phoneno, "Usuario 2: "  + ConfigSettings.alertmsg);
  }
}

void showHelp(){
  Serial.println("--------------------------------------------------------");
  Serial.println("Comandos disponibles: ");
  Serial.println("--------------------------------------------------------");
  Serial.println("conf          - Ver configuracion Actual");
  Serial.println("confreset     - Reestablecer datos de configuracion");
  Serial.println("files         - Listar archivos de SPIFF");
  Serial.println("gsm           - Ver informacion GSM");
  Serial.println("gprs          - Ver estado de GRPS");
  Serial.println("--------------------------------------------------------");
  Serial.println(" ");
}

void loop() {

  checkLevel();
  checkAp();
  
  server.handleClient();

  while (Serial.available()) {
    delay(3);
    char c = Serial.read();
    
    if(readString == "AT+"){
      atComm += c;
    }else{
      readString += c;
    }
  }
  
  readString.trim();
  if (readString.length() > 0) {
    if (readString == "help") {
      showHelp();
    } else if (readString == "conf") {
      Serial.println("-------------------");
      Serial.println(getConfigAsString());
    } else if (readString == "confreset") {
      Serial.println("-------------------");
      Serial.println("Reestableciendo configuracion");
      resetConfigDefault();
    }else if (readString == "files") {
      Serial.println("-------------------");
      Serial.println(listDir("/"));
      Serial.println("-------------------");
    } else if (readString == "gsm") {
      Serial.println("-------------------");
      Serial.println(getGsmInfo());
      Serial.println("-------------------");
      Serial.println(" ");
    } else {
      Serial.println("Comando: ");
      Serial.print(readString);
      Serial.println(" ");
    }
    readString = "";
  }
}
