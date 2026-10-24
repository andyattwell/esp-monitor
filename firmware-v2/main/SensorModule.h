/*
  SensorModule.h - Library for maging ESP I/O sensor.
  Created by Andy Attwell, December 29, 2019..
*/
#ifndef SensorModule_h
#define SensorModule_h

#include <Arduino.h>
#include "ConfigModule.h"

class SensorModule
{
  public:
    SensorModule(int pumpPin = 12, int sensorPin1 = 2, int sensorPin2 = 5);
    void    init();
    int     listen();
    void    togglePump();
    void    turnOnPump();
    void    turnOffPump();
    int     checkBtn();
    int     readLevel();
  private:
    int _pumpPin;                       // Pump relay pin
    int _lvlPin1;                       // Level 1 sensor
    int _lvlPin2;                       // Level 2 sensor
    int _btnPin;                        // Button pin
    bool autoTurnOn;                    // Turn on pump on sensor event
    bool pumpStatus;                    // Pump status
    unsigned long onTime;               // Time when pump was turned on millis
    long timeout;                       // Time interval run pump
    int level;                          // Current level
    int lastLvl;                        // Last known level
    int lastChange;                     // Last change registered
    unsigned long lastchangeTime;       // Time of last level change
    int btnLastState;                   // Button preseed?
    unsigned long presedTime;           // Time when button was pressed
};

#endif