/*
  TestSuiteModule.h - Library for managing ESP Serial command Input.
  Created by Andy Attwell, Jan 11, 2020..
*/
#ifndef TestSuiteModule_h
#define TestSuiteModule_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "ConfigModule.h"
#include "ApModule.h"
#include "SensorModule.h"
#include "AlertModule.h"

#define TESTSUITE_LOG

class TestSuiteModule
{
  public:
    TestSuiteModule();
    void          init(SensorModule _sensor, ApModule _ap, AlertModule _alert);
    void          listen();
  private:
    ConfigModule  config;
    SensorModule  sensor;
    ApModule      ap;
    AlertModule   alert;
    String        readString;
    char*         readChar;
    String        atComm;
    String        readreadStringAt;
    void          run(String cmdStr);
    void          help();                   // Print help menu to serial
};

#endif