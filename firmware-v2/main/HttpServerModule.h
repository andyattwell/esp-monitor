/*
  ApModule.h - Library for maging ESP AP / Wifi connections.
  Created by Andy Attwell, December 29, 2019..
*/
#ifndef HttpServerModule_h
#define HttpServerModule_h

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include "ConfigModule.h"

class HttpServerModule
{
  public:
    HttpServerModule();
    void run();
    void startHandler();
  private:
    ConfigModule config;
    File fsUploadFile;    // upload file.
    String getContentType(String filename);
    String listDir(String path);
    const char* www_username;
    const char* www_password;

    void sendResponse(String res, int code);
    bool handleFileRead(String path);
    void handlePostConfig();
    void handleGetConfig();
    void handleResetDefault();

};

#endif