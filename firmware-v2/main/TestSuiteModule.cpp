#include <Arduino.h>
#include "TestSuiteModule.h"
#include "ConfigModule.h"
#include "ApModule.h"
#include "SensorModule.h"
#include "AlertModule.h"

TestSuiteModule::TestSuiteModule () {}

void TestSuiteModule::init (SensorModule _sensor, ApModule _ap, AlertModule _alert) {
  sensor = _sensor;
  ap = _ap;
  alert = _alert;
  // Serial.begin(115200);
}

void TestSuiteModule::listen () {
  while (Serial.available()) {
    delay(3);
    char c = Serial.read();
    
    readString += c;

  }

  readString.trim();

  if (readString.length() > 0) {
    run(readString);
  }
}

void TestSuiteModule::run (String cmdStr) {

  Serial.println("-------------------");

  if (readString == "help") {
    help();
  } else if (readString == "level") {

    Serial.println("Nivel: " + String( sensor.readLevel()) );

  } else if (readString == "pumpToggle") {

    Serial.println("Cambiando estado de bomba");
    sensor.togglePump();

  } else if (readString == "pumpON") {

    Serial.println("Prendiendo bomba");
    sensor.turnOnPump();

  } else if (readString == "pumpOFF") {

    Serial.println("Apagando bomba");
    sensor.turnOffPump();

  } else if (readString == "apON") {

    Serial.println("Conectando Ap");
    ap.connectAp();

  } else if (readString == "apOFF") {

    Serial.println("Apagando Ap");
    ap.disconnectAp();

  } else if (readString == "alertSettings") {

    Serial.println("Configuracion de alertas");
    alert.printSettings();

  } else if (readString == "sendAlert" ) {

    Serial.println("Enviando notificacion");
    alert.sendNotification(SENSOR_LVL_UP_2);

  } else if (readString == "list") {

    Serial.println("Listando directorio");
    Serial.println(config.listDir("/"));

  } else if (readString == "conf") {

    Serial.println("Leyendo archivo de configuracion");
    config.getConfig();
    Serial.println("Configuracion:");
    Serial.println(config.getConfigAsString());

  } else if (readString == "confreset") {

    Serial.println("Reestableciendo configuracion");

    if (config.resetDefault()) {
      Serial.println("Configuracion reestablecida");
    } else {
      Serial.println("Error al reestablecer configuracion");    
    }

  }

  Serial.println("-------------------");
  Serial.println(" ");

  readString = "";
}

void TestSuiteModule::help () {
  Serial.println("Comandos disponibles: ");
  Serial.println("--------------------------------------------------------");
  Serial.println("help          - Muestra este menu");
  Serial.println("level         - Nivel de sensor 0|1|2");
  Serial.println("pumpToggle    - Cambiar estado de bomba");
  Serial.println("pumpON        - Prender bomba");
  Serial.println("pumpOFF       - Apagar bomba");
  Serial.println("apON          - Conectar Access Point");
  Serial.println("apOFF         - Desconectar Access Point");
  Serial.println("alertSettings - Mostrar configruacion de alertas");
  Serial.println("sendAlert     - Enviar alerta");
  Serial.println("list          - listar contenido de SPIFF");
  Serial.println("conf          - Ver configuracion Actual");
  Serial.println("confreset     - Reestablecer datos de configuracion");
}