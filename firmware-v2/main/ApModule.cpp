#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ApModule.h"
#include "HttpServerModule.h"

ApModule::ApModule()
{
  HttpServerModule httpServer;
  IPAddress apIP(192, 168, 4, 1); // Ip de ap
  IPAddress netMsk(255, 255, 255, 0); // Submascara de ap
  apTimeout = 300000; // 5 min
  apConnected = false;
  apStarted = 0;
}

void ApModule::init (String apSsid, String apPass)
{
  apSsid = apSsid;
  apPass = apPass; // empty = open
  connectAp();
}

void ApModule::connectAp ()
{
  if (!apSsid.equals("")) {
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(apSsid.c_str(), apPass.c_str());
    httpServer.run();
    apConnected = true;
    apStarted = millis();
  }
}

/*
 * Desconectar AP
 */
void ApModule::disconnectAp () {
  WiFi.softAPdisconnect(true);
  apConnected = false;
  apStarted = 0;
}
/*
 * Desconectar AP
 */
void ApModule::toggleAp () {
  if (apConnected) {
    disconnectAp();
  }
}

/*
 * Chekear estado de AP y desconectar si pasaron N minutos
 */
void ApModule::run () {
  if(apConnected){
    unsigned long currentMills = millis();
    if(apStarted > 0 && (unsigned long)(currentMills - apStarted) >= apTimeout){
      disconnectAp();
    } else {
      httpServer.startHandler();
    }
  }
}