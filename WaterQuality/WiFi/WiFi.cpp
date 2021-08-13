/*
 * ESP8266-01 WiFi Module
 * ESP8266-01 PINOUT:
 * -------------------------------------------
 * | TX/GPIO1/LED * * GND           |   -----|
 * | CH_PD/EN     * * GPIO2         |   |-----
 * | RST          * * GPIO0/FLASH   |   -----|
 * | VCC (5v)     * * RX/GPIO3      |        |
 * -------------------------------------------
 * 
 * Note: Set FLASH pin to ground for programming
 * 
 * AI-Thinker Firmware: https://wiki.aprbrother.com/en/Firmware_For_ESP8266.html#v0-9-5-2
 * Flasher: https://github.com/nodemcu/nodemcu-flasher
 */

#include "WiFi.h"


/*****************************
 * WIFI CLASS IMPLEMENTATION *
 *****************************/

/*
 * WiFi Constructor
 */
WiFi::WiFi() {}


/* 
 * WiFi Setup
 */
void WiFi::setup() {

  /* WiFi Configuration as Station mode */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  /* Connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  /* Server Configurations - Conductivity Sensor */
  server.on(CONDUCTIVITY_GET_EC, serverGetEC);
  server.on(CONDUCTIVITY_GET_TDS, serverGetTDS);
  server.on(CONDUCTIVITY_GET_EC_TDS, serverGetECTDS);
  server.on(CONDUCTIVITY_GET_SPAN, serverGetConductivitySpan);
  server.on(CONDUCTIVITY_SET_SPAN, serverSetConductivitySpan);

  /* Server Configurations - Temperature Sensor */
  server.on(TEMPERATURE_GET_TEMP, serverGetTemperature);
  server.on(TEMPERATURE_GET_SPAN, serverGetTemperatureSpan);
  server.on(TEMPERATURE_SET_SPAN, serverSetTemperatureSpan);

  /* Server Configurations - PH Sensor */


  /* Server Configurations - Unsupported Operation */
  server.onNotFound(serverUnsupportedOperation);

  /* Start Server */
  server.begin();
}


/*
 * WiFi Handle Client
 */
void WiFi::handleClient() {
    server.handleClient();
}











/********************************
 * ESP8266-01 SERVER CONTROLLER *
 ********************************/

/*
 * Get current Temperature
 * API Return: 200, with current Temperature
 */
void serverGetTemperature() {

  /* Get Temperature */
  float temperature;
  if( getTemperature(&temperature) ) {

    /* Format Temperature value */
    char formatedTemperature[TEMPERATURE_STR_LENGTH];
    snprintf(formatedTemperature, TEMPERATURE_STR_LENGTH, "%.1f\n", temperature);
    
    /* Send formated Temperature */
    server.send(200, "text/plain", formatedTemperature);
  
  } else {
    server.send(400, "text/plain", "Unavailable Temperature\n");
  }
}

/*
 * Get current Temperature Span
 * API Return: 200, with current Temperature span
 */
void serverGetTemperatureSpan() {

  /* Format Temperature Span */
  char formatedTemperature[TEMPERATURE_STR_LENGTH];
  snprintf(formatedTemperature, TEMPERATURE_STR_LENGTH, "%d\n", temperatureSpan);

  /* Send formated Temperature Span */
  server.send(200, "text/plain", formatedTemperature);
}

/*
 * Set new Temperature Span
 * API Return: 200, with the updated Temperature Span
 * API Return: 400 if no/invalid new Temperature Span
 */
void serverSetTemperatureSpan() {

  /* Check new Feeder Time argument */
  if (!server.hasArg(TEMPERATURE_SET_SPAN_ARG)) {
    server.send(400, "text/plain", "No new Temperature Span to set\n");

  } else {

    /* Convert String Time to unsigned int */
    unsigned int newTemperatureSpan = server.arg(TEMPERATURE_SET_SPAN_ARG).toInt();

    /* Verify Temperature Span */
    if (newTemperatureSpan == 0) {
      server.send(400, "text/plain", "Invalid new Temperature Span (0)\n");
    
    } else {

      /* Set the new Temperature Span & Reset current Time */
      temperatureSpan = newTemperatureSpan;
      previousReadTime = ELAPSED_MINUTES(millis() / 1000);

      /* Update Temperature value */
      serverGetTemperatureSpan();
    }
  }
}

/*
 * Unsupported server operation
 * API Return: 501
 */
void serverUnsupportedOperation() {
  server.send(501, "text/plain", "Unsupported Operation\n");
}


/********************************
 * ESP8266-01 CLIENT CONTROLLER *
 ********************************/

/*
 * Send temperature value to server
 */
void clientSendTemperature(float temperature) {

  /* Connect to server */
  WiFiClient client;
  if (!client.connect(WIFI_CLIENT_IP, WIFI_CLIENT_PORT)) {
    return;
  }

  /* Connected Client */
  if (client.connected()) {

    /* Format Temperature value */
    char formatedTemperature[TEMPERATURE_STR_LENGTH];
    snprintf(formatedTemperature, TEMPERATURE_STR_LENGTH, "%.1f\n", temperature);

    /* Send Temperature value */
    client.println(formatedTemperature);
    client.stop();
  }
}
