#include <Arduino.h>
#include <SoftwareSerial.h>
#include "ConfigModule.h"
#include "ApModule.h"
#include "SensorModule.h"
#include "AlertModule.h"
#include "TestSuiteModule.h"

ConfigModule configModule;
SensorModule sensorModule;
AlertModule alertModule;
ApModule apModule;
TestSuiteModule TestSuite;

void setup() {
  Serial.begin(115200);
  // ConfigStruc conf = configModule.getConfig();
  sensorModule.init();
  // apModule.init(conf.apSsid, conf.apPass);
  // alertModule.init(conf.phoneno, conf.phoneno2);
  alertModule.init("123123123", "654654654");
  TestSuite.init(sensorModule, apModule, alertModule);
}

void loop() {

  // apModule.run();
  
  // // Check for level and send notification
  int lvlStatus = sensorModule.listen();
  if ( lvlStatus != SENSOR_LVL_0 ) {
    alertModule.sendNotification(lvlStatus);
  }

  // // Check for incomign msg/call
  // int incomingEvt = alertModule.listenIncoming();
  // if (incomingEvt != 0) {
  //   if (incomingEvt == INCOMING_CALL || incomingEvt == INCOMING_MSG_ON) {
  //     sensorModule.turnOnPump();
  //   } else if (incomingEvt == INCOMING_MSG_OFF) {
  //     sensorModule.turnOffPump();
  //   }
  // }

  // // Check for button events
  int btnEvt = sensorModule.checkBtn();
  if (btnEvt == BTN_PRESSED_3) {
    sensorModule.togglePump();
  }
  // if (btnEvt == BTN_PRESSED_10) {
  //   apModule.toggleAp();
  // }

  // Serial Test I/O
  TestSuite.listen();
}
