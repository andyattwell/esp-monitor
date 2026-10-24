#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "ConfigModule.h"

ConfigModule::ConfigModule()
{
  ConfigStruc settings = {
    "ESPDEF",
    "fedpse",
    300000,
    "348769753721",
    "348769753721"
  };
}

/*
   Load config values from JSON file
*/
ConfigStruc ConfigModule::getConfig(String filePath) {
  int spiffsErr = 0;
  if (!SPIFFS.begin()) {
    Serial.println("Error al inicar SPIFF");
    spiffsErr = 1;
  }

  File configFile = SPIFFS.open(filePath, "r");
  if (!configFile) {
    Serial.println("Error al abrir archivo: " + filePath);
    spiffsErr = 2;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Archivo demasiado grande");
    spiffsErr = 3;
  }

  if (spiffsErr) {
    return settings;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> json;
  DeserializationError error = deserializeJson(json, buf.get());
  
  if (error) {
    return settings;
  }

  char apssid[30];
  strcpy(apssid, json["apssid"]);
  settings.apSsid = apssid;

  char appass[30];
  strcpy(appass, json["appass"]);
  settings.apPass = appass;

  char aptimeout[30];
  strcpy(aptimeout, json["aptimeout"]);
  settings.apTimeout = atoi(aptimeout);

  char phoneno[30];
  strcpy(phoneno, json["phoneno"]);
  settings.phoneno = phoneno;

  char phoneno2[30];
  strcpy(phoneno2, json["phoneno2"]);
  settings.phoneno2 = phoneno2;

  char autoTurnOn[30];
  strcpy(autoTurnOn, json["autoTurnOn"]);
  settings.autoTurnOn = atol(autoTurnOn);

  return settings;
}

/*
  Set config values and save to JSON file
*/
bool ConfigModule::setConfig(String apSsid, String apPass, long apTimeout, String phoneno, String phoneno2, int autoTurnOn) {

  ConfigStruc set = {
    apSsid,
    apPass,
    apTimeout,
    phoneno,
    phoneno2,
    autoTurnOn
  };

  return saveConfigFile(set);
}

/*
  Save config values to JSON file
*/
bool ConfigModule::saveConfigFile(ConfigStruc newSettings) {
  bool res;
  String json = "{";
    json += "\"appass\" : \"" + newSettings.apPass + "\"";
    json += ", \"apssid\" : \"" + newSettings.apSsid + "\"";
    json += ", \"aptimeout\" : \"" + String(newSettings.apTimeout) + "\"";
    json += ", \"phoneno\" : \"" + newSettings.phoneno + "\"";
    json += ", \"phoneno2\" : \"" + newSettings.phoneno + "\"";
    json += ", \"autoTurnOn\" : \"" + String(newSettings.autoTurnOn) + "\"";
  json += "}";

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    res = false;
  } else {
    configFile.print(json);
    configFile.close();
    res = true;
  }
  return res;
}

/*
    Get config values as JSON String
*/
String ConfigModule::getConfigAsString() {
  ConfigStruc settings = getConfig();
  String json = "{";
  json += "\"appass\" : \"" + String(settings.apPass) + "\"";
  json += ", \"apssid\" : \"" + String(settings.apSsid) + "\"";
  json += ", \"aptimeout\" : \"" + String(settings.apTimeout) + "\"";
  json += ", \"phoneno\" : \"" + String(settings.phoneno) + "\"";
  json += ", \"phoneno2\" : \"" + String(settings.phoneno2) + "\"";
  json += ", \"autoTurnOn\" : \"" + String(settings.autoTurnOn) + "\"";
  json += "}";

  return json;

}

/*
  Reset config values to factory defauls
*/
bool ConfigModule::resetDefault(){
  return saveConfigFile(getConfig("/config.default.json"));
}

String ConfigModule::listDir(String path) {

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