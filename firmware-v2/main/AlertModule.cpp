#include <Arduino.h>
#include <SoftwareSerial.h>
#define TINY_GSM_MODEM_A6
#include <TinyGsmClient.h>
#include "AlertModule.h"

SoftwareSerial SerialAT(13, 15);
TinyGsm modem(SerialAT);

AlertModule::AlertModule () {
  lastAlertTime = 0;
  lastAlertType = 0;
  alertInterval = 300000;
  success1 = false;
  success2 = false;
}

void AlertModule::init (String _phoneno, String _phoneno2) {
  phoneno = _phoneno;
  phoneno2 = _phoneno2;
} 

void AlertModule::printSettings () {
  Serial.println("lastAlertTime: " + String(lastAlertTime));
  Serial.println("lastAlertType: " + String(lastAlertType));
  Serial.println("alertInterval: " + String(alertInterval));
  Serial.println("success1: " + String(success1));
  Serial.println("success2: " + String(success2));
  Serial.println("phoneno2: " + String(phoneno2));
}

/*
   Loads the modem and returns info
*/
String AlertModule::loadGSM () {

  String modemInfo = "No hay modem";
  if (modem.init()) {
    modem.sendAT(GF("AT+CPMS=\"SM\",\"SM\",\"SM\""));
  }
  delay(300);
  return String(modemInfo);
}

/*
  Connect to Gprs
*/
String AlertModule::loadGPRS() {
  const char * apn = "internet.gprs.unifon.com.ar";
  const char * user = "wap";
  const char * pass = "wap";
  
  if (modem.gprsConnect(apn, user, pass)) {
    return String("Conectado a GPRS:");
  } else {
    return String("No hay conexion GPRS");
  }
}

/*
  Get GPRS status
*/
String AlertModule::getGPRSInfo(){
  if(modem.isGprsConnected()){
    return "Conectado";
  }else{
    return "Desconectado";
  }
}

/*
   Get GSM Info as JSON String
*/

String AlertModule::getGsmInfo() {

  String _status = "Desconectado";

  if (modem.isNetworkConnected()) {
    _status = "Conectado";
  }

  String json = "{";
  json += ", \"csq\" : \"" + String(modem.getSignalQuality()) + "\"";
  json += ", \"battLevel\" : \"" + String(modem.getBattPercent()) + "\"";
  json += ", \"status\" : \"" + String(_status) + "\"";
  json += "}";
  return json;

}

/*
  Send notification SMS
*/
void AlertModule::sendNotification(int type) {

  Serial.println("Notification request (" + String(type) + ")");

  int next = chektime(lastAlertTime, alertInterval);

  if ((type != lastAlertType) && next == 0) {
    String msg = getAlertMessage(type);

    if (!success1 & !phoneno.equals("")) {
      Serial.println("Enviando Notificacion 1: (" + String(msg) + ")");
      success1 = modem.sendSMS(phoneno, msg);
    }

    if (!success2 & !phoneno2.equals("")) {
      Serial.println("Enviando Notificacion 2: (" + String(msg) + ")");
      success2 = modem.sendSMS(phoneno2, msg);
    }

    // if (success1 & success2) {
      lastAlertTime = millis();
      lastAlertType = type;
    // }

  } else {
    Serial.println("NOT NOW - Next in: " + millisToTime(next) );
    success1 = false;
    success2 = false;
  }
}

String AlertModule::millisToTime(int time) {

  int secs = time / 1000;
  int mins = secs > 60 ? secs / 60 : 0;
  int leftSecs = secs - (mins * 60);
  
  String secStr = leftSecs <= 9 ? "0" + String(leftSecs) : String(leftSecs);
  String minStr = mins <= 9 ? "0" + String(mins) : String(mins);

  return minStr  + ":" + secStr;
}

String AlertModule::getAlertMessage (int type) {
  String msg = "";
  switch (type) {
    case SENSOR_LVL_UP_1:
      msg += "El nivel subió a 1";
      break;
    case SENSOR_LVL_UP_2:
      msg += "El nivel subió a 2";
      break;
    case SENSOR_LVL_DOWN_1:
      msg += "El nivel bajó a 1";
      break;
    case SENSOR_LVL_DOWN_0:
      msg += "El nivel bajó a 0";
      break;
  }

  return msg;
}

int AlertModule::chektime(unsigned long startTime, long timeout) {
  unsigned long currentMil = millis();

  if ( (currentMil - startTime >= timeout) || startTime == 0 ) {
    return 0;
  }

  return timeout - (currentMil - startTime);
}

int AlertModule::listenIncoming () {
  while (SerialAT.available()) {

    String c = SerialAT.readStringUntil(':');
    delay(3);
    c.trim();
    
    if(c == "RING"){
      // Incoming call
      // TODO - Check if incoming number matches config phones

      return INCOMING_CALL;
    } else if(c == "+CIEV"){
      SerialAT.readStringUntil('"');
      String d = SerialAT.readStringUntil('"');
      d.trim();
      
      if(d == "MESSAGE"){
        // Incoming message
        // Send at command to retrieve message
        modem.sendAT(GF("+CMGR=1"));
      }

    } else if(c == "+CMGR") {
      // Reading incoming message
      SerialAT.readStringUntil('"');
      String sto = SerialAT.readStringUntil('"');
      sto.trim();
      SerialAT.readStringUntil('"');
      String num = SerialAT.readStringUntil('"');
      num.trim();
      SerialAT.readStringUntil('"');
      String fecha = SerialAT.readStringUntil('"');
      fecha.trim();

      SerialAT.readStringUntil('\n');
      String orden = SerialAT.readStringUntil(' ');
      int val = SerialAT.readStringUntil('\n').toInt();

      if (orden.equals("ON")) {
        // Turn on
        return INCOMING_MSG_ON;

      } else if (orden.equals("OFF")) {
        // Turn off
        return INCOMING_MSG_OFF;
      }
      
      // Not shure what this does
      modem.sendAT(GF("+CMGD=1"));

      delay(300);
      
    }
  }
}