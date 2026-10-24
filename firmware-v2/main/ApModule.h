/*
  ApModule.h - Library for maging ESP AP / Wifi connections.
  Created by Andy Attwell, December 29, 2019..
*/
#ifndef ApModule_h
#define ApModule_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "HttpServerModule.h"

class ApModule
{
  public:
    ApModule();
    void                init(String apSsid, String apPass);
    void                connectAp();
    void                disconnectAp();
    void                run();
    void                toggleAp();

  private:
    HttpServerModule    httpServer;
    ConfigModule        _config;
    IPAddress           apIP;                   // AP IP address
    IPAddress           netMsk;                 // AP Netmask
    String              apSsid;
    String              apPass;
    bool                apConnected;            // Status
    unsigned long       apStarted;              // Time that was started at
    unsigned long       apTimeout;
};

#endif