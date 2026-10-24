/*
  AlertModule.h - Library for maging ESP AP / Wifi connections.
  Created by Andy Attwell, December 29, 2019..
*/
#ifndef AlertModule_h
#define AlertModule_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#define TINY_GSM_MODEM_A6
#include <TinyGsmClient.h>
#include "ConfigModule.h"

class AlertModule
{
  public:
    AlertModule();
    void          init(String _phoneno, String _phoneno2);
    String        loadGSM();
    String        loadGPRS();
    String        getGPRSInfo();
    String        getGsmInfo();
    void          sendNotification(int type);
    int           listenIncoming();
    void          printSettings();
  private:
    bool          success1;
    bool          success2;
    String        phoneno;
    String        phoneno2;
    int           chektime(unsigned long time, long timeout);
    String        getAlertMessage(int type);
    int           alertInterval;              // Time between notifications millis
    int           lastAlertType;              
    unsigned long lastAlertTime;    // Last notification millis
    String millisToTime(int time);
};

#endif