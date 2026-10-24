/*
  GsmModule.h - Library for maging ESP AP / Wifi connections.
  Created by Andy Attwell, December 29, 2019..
*/
#ifndef ConfigModule_h
#define ConfigModule_h

#include <Arduino.h>

const int SENSOR_LVL_0 = 0;            //  Level is 0 and has not changed
const int SENSOR_LVL_UP_1 = 1;         //  Level has risen and its on lvl 1
const int SENSOR_LVL_UP_2 = 2;         //  Level has risen and its on lvl 2
const int SENSOR_LVL_DOWN_1 = 3;       //  Level has fallen and its on lvl 1
const int SENSOR_LVL_DOWN_0 = 4;       //  Level has fallen and its on lvl 0
const int INCOMING_CALL = 1;
const int INCOMING_MSG_ON = 2;
const int INCOMING_MSG_OFF = 2;
const int BTN_PRESSED_3 = 3;
const int BTN_PRESSED_6 = 6;
const int BTN_PRESSED_10 = 10;

struct ConfigStruc{
  String  apSsid;
  String  apPass;
  long    apTimeout;
  String  phoneno;
  String  phoneno2;
  int    autoTurnOn;
};

class ConfigModule
{
  public:
    ConfigModule();
    ConfigStruc     getConfig(String filePath = "/config.json");
    bool            saveConfigFile(ConfigStruc newSettings);
    bool            setConfig(String apSsid, String apPass, long apTimeout, String phoneno, String phoneno2, int autoTurnOn);
    String          getConfigAsString();
    bool            resetDefault();
    String          getPhoneNo();
    String          listDir(String path);
    ConfigStruc     settings;
  private:
};

#endif