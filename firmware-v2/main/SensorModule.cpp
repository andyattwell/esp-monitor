#include <Arduino.h>
#include "SensorModule.h"
#include "ConfigModule.h"

SensorModule::SensorModule (int pumpPin, int sensorPin1, int sensorPin2)
{
  _pumpPin = pumpPin;
  _lvlPin1 = sensorPin1;
  _lvlPin2 = sensorPin2;
  _btnPin = 4;

  level = 0;
  lastLvl = 0;
  pumpStatus = false;
  onTime = 0;
  timeout = 300000;
  autoTurnOn = false;
  btnLastState = HIGH;
}

void SensorModule::init() {

  pinMode(_lvlPin1, INPUT_PULLUP);
  pinMode(_lvlPin2, INPUT_PULLUP);
  pinMode(_btnPin, INPUT_PULLUP);
  pinMode(_pumpPin, OUTPUT);

  digitalWrite(_pumpPin, LOW);
}

void SensorModule::togglePump() {
  Serial.println("PIN:  " + String(_pumpPin));
  Serial.println("READ: " + String(digitalRead(_pumpPin)));
  Serial.println("STA:  " + String(pumpStatus));
  if (digitalRead(_pumpPin) == HIGH) {
    turnOffPump();
  } else {
    turnOnPump();
  }
}

void SensorModule::turnOnPump() {
  Serial.println("Prendiendo bomba (" + String(_pumpPin) + ") HIGH");
  digitalWrite(_pumpPin, HIGH);
  pumpStatus = true;
}

void SensorModule::turnOffPump() {
  Serial.println("Apagando bomba (" + String(_pumpPin) + ") LOW");
  digitalWrite(_pumpPin, LOW);
  pumpStatus = false;
}

int SensorModule::readLevel () {
  if (digitalRead(_lvlPin2) == LOW) {
    level = 2;
  } else if (digitalRead(_lvlPin1) == LOW) {
    level = 1;
  } else {
    level = 0;
  }
  return level;
}

int SensorModule::listen() {
  
  readLevel();

  int lvlChange = SENSOR_LVL_0;

  if (level != lastLvl) {
    if (level > lastLvl) {
      // Send alert
      if (level == 1) {
        lvlChange = SENSOR_LVL_UP_1;
      } else {
        lvlChange = SENSOR_LVL_UP_2;
        if (autoTurnOn) {
          turnOnPump();
        }
      }

    } else {
      if (level == 1) {
        lvlChange = SENSOR_LVL_DOWN_1;
      } else {
        lvlChange = SENSOR_LVL_DOWN_0;
      }
    }
    lastLvl = level;
    lastchangeTime = millis();
  }

  lastChange = lvlChange;

  return lastChange;
}

int SensorModule::checkBtn(){
  int action = 0;
  int currentState = digitalRead(_btnPin);
  unsigned long currentMillis = millis();
  
  if (currentState == LOW & btnLastState == HIGH) // Boton apretado.
  {

    presedTime = millis();
    // Serial.println("Start " + String(presedTime));
    btnLastState = LOW;


  } else if (currentState == HIGH & btnLastState == LOW) {

    // This is running while is pushed
    btnLastState = HIGH;
    unsigned long timediff = currentMillis - presedTime;
    // Serial.println("Time pressed: " + String(timediff));
    
    if (timediff >= 200 && timediff <= 3000) {
      Serial.println("Boton 3");
      action = BTN_PRESSED_3;
    } else if (timediff > 3000 && timediff <= 6000) {
      Serial.println("Boton 6");
      action = BTN_PRESSED_6;
    } else if (timediff > 6000 &&timediff <= 10000) {
      Serial.println("Boton 10");
      action = BTN_PRESSED_10;
    }
    
    presedTime = currentMillis;
  }

  
  return action;
}