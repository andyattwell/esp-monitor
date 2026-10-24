#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include "HttpServerModule.h"
#include "ConfigModule.h"

ESP8266WebServer server(80);

HttpServerModule::HttpServerModule () {
  www_username = "admin";
  www_password = "esp8266";
}

/*
   HTTP routes
*/
void HttpServerModule::run () {
  /* Rutas del servidor web */
  server.on("/config", HTTP_GET, [&]() {
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    handleGetConfig();
  });
  server.on("/config", HTTP_POST, [&]() {
    handlePostConfig();
  });
  server.on("/config/reset", HTTP_POST, [&]() {
    handleResetDefault();
  });
  server.on("/", HTTP_GET, [&]() {
    if (!handleFileRead("/index.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  server.onNotFound([&]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  server.begin(); // Iniciar servidor web
}

/*
   Client handler to run in the loop
*/
void HttpServerModule::startHandler() {
  server.handleClient();
}

/*
   Returns requested file content type.
*/
String HttpServerModule::getContentType(String filename) {
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
/*
   Open file
*/
bool HttpServerModule::handleFileRead(String path) {

  // Return home page
  if (path.endsWith("/")) path += "index.html";

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";

  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    // If compress file exists return that
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";

    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);

    file.close();
    return true;
  }

  return false;
}

/*
   POST /config.
   Save config parameters to file
*/
void HttpServerModule::handlePostConfig() {

  bool save = config.setConfig(
    server.arg("appass"),
    server.arg("apssid"),
    server.arg("aptimeout").toInt(),
    server.arg("phoneno"),
    server.arg("alertmsg"),
    server.arg("autoTurnOn").toInt()
  );

  if (save) {
    sendResponse("{ \"success\" : true , \"message\" : \"ConfiguraaciÃ³n guardada.\" }", 200);
  } else {
    sendResponse("{ \"error\" : true , \"message\" : \"Error al abrir archivo de configuracion.\" }", 400);
  }
  
}

/*
  Get /config
  Returns config as JSON string
*/
void HttpServerModule::handleGetConfig() {
  String res = config.getConfigAsString();
  sendResponse(res, 200);
}

/*
  POST /config/reset
  Reset config to default values
*/
void HttpServerModule::handleResetDefault(){
  String res = "";
  if (config.resetDefault()) {
    res = "OK";
  } else {
    res = "Error";
  }

  sendResponse(res, 200);
}

/*
  Send HTTP response
*/
void HttpServerModule::sendResponse(String res, int code) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send ( code, "Content-Type: application/json;charset=utf-8", res);
}