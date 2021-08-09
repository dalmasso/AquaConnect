/*
 * ESP8266-01 Temperature Sensor
 * ESP8266-01 PINOUT:
 * -------------------------------------------
 * | TX/GPIO1/LED * * GND           |   -----|
 * | CH_PD/EN     * * GPIO2         |   |-----
 * | RST          * * GPIO0/FLASH   |   -----|
 * | VCC(3.3)     * * RX/GPIO3      |        |
 * -------------------------------------------
 * 
 * Note: Set FLASH pin to ground for programming
 * 
 * AI-Thinker Firmware: https://wiki.aprbrother.com/en/Firmware_For_ESP8266.html#v0-9-5-2
 * Flasher: https://github.com/nodemcu/nodemcu-flasher
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <OneWire.h>


/*****************************
 * ESP8266-01 CONFIGURATIONS *
 *****************************/

/* DS18B20 Temperature Sensor GPIO */
#define DS18B20_GPIO                  2

/* DS18B20 Commands */
#define DS18B20_TEMPERATURE           0x44
#define DS18B20_SCRATCHPAD            0xBE
#define DS18B20_SCRATCHPAD_LENGTH     9
#define DS18B20_TEMP_LSB              0
#define DS18B20_TEMP_MSB              1

/* Default Temperature Span (in minutes) */
#define TEMPERATURE_SPAN_DEFAULT      10

/* Formatted Temperaure length (format: xx.x\n) */
#define TEMPERATURE_STR_LENGTH        5+1

/* Time Conversion */
#define SECS_PER_MIN                  (60UL)
#define ELAPSED_MINUTES(_time_)       ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)

/* Server API */
#define TEMPERATURE_GET_TEMP          "/GET_TEMP"
#define TEMPERATURE_GET_SPAN          "/GET_SPAN"
#define TEMPERATURE_SET_SPAN          "/SET_SPAN"
#define TEMPERATURE_SET_SPAN_ARG      "Span"

/* WiFi */
#define WIFI_NETWORK                  "AquaNetwork"
#define WIFI_PASSWORD                 "pVdOKCvU9MYC8"

/* WiFi Client Config */
#define WIFI_CLIENT_IP                "192.168.3.61"
#define WIFI_CLIENT_PORT              80


/*************************
 * ESP8266-01 CONSTANTES *
 *************************/

/* DS18B20 OneWire Address */
const byte DS18B20Addr[] = {0x28, 0xD1, 0x19, 0x75, 0xD0, 0x01, 0x3C, 0x8B};


/*******************************
 * ESP8266-01 GLOBAL VARIABLES *
 *******************************/

/* Temperature Span (in minutes) */
unsigned int temperatureSpan = TEMPERATURE_SPAN_DEFAULT;

/* Previous Temperature read */
unsigned int previousReadTime = 0;

/* Temperature Sensor DS18B20 Access */
OneWire DS18B20(DS18B20_GPIO);

/* Server Object, available on port 80 */
ESP8266WebServer server(80);


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


/*********************
 * DS18B20 CONTROLLER *
 *********************/

/*
 * Temperature Controller
 */
void TemperatureController() {

  /* Get current Time in minutes */
  unsigned int currentTimeMin = ELAPSED_MINUTES(millis() / 1000);

  /* Check Temperature Span */
  if ( (currentTimeMin - previousReadTime) >= temperatureSpan) {

    /* Reset Previous Temperature Read */
    previousReadTime = currentTimeMin;

    /* Read & Send Temperature */
    float temperature;
    if (getTemperature(&temperature)) {
      clientSendTemperature(temperature);
    }
  }
}

/*
 * Get Temperature value from DS18B20
 * Arg: Temperature value pointer
 * Return: True if read operation succeeds, else false
 */
bool getTemperature(float* temperature) {
  byte rawData[DS18B20_SCRATCHPAD_LENGTH];
  int16_t rawTemp;

  /* Reset & Select DS18B20 */
  DS18B20.reset();
  DS18B20.select(DS18B20Addr);
  
  /* Start Temperature measure */
  DS18B20.write(DS18B20_TEMPERATURE, 1);
  delay(800);

  /* Reset the One-Wire bus & Select DS18B20 Chip */
  DS18B20.reset();
  DS18B20.select(DS18B20Addr);

  /* Request the DS18B20 Scratchpad */
  DS18B20.write(DS18B20_SCRATCHPAD);

  /* Read the scratchpad */
  for (byte i=0; i < DS18B20_SCRATCHPAD_LENGTH; i++) {
    rawData[i] = DS18B20.read();
  }

  /* Compute Temperature */
  rawTemp = (int16_t) ((rawData[DS18B20_TEMP_MSB] << 8) | rawData[DS18B20_TEMP_LSB]);

  /* Convert Temperature in Celsius */
  *temperature = (float) rawTemp / 16.0;
  return true;
} 


/********************
 * ESP8266-01 SETUP *
 ********************/

void setup()
{
  /* WiFi Configuration as Station mode */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  /* Connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  /* Server Operation Configurations */
  server.on(TEMPERATURE_GET_TEMP, serverGetTemperature);
  server.on(TEMPERATURE_GET_SPAN, serverGetTemperatureSpan);
  server.on(TEMPERATURE_SET_SPAN, serverSetTemperatureSpan);
  server.onNotFound(serverUnsupportedOperation);

  /* Start Server */
  server.begin();
}


/***************************
 * ESP8266-01 MAIN PROGRAM *
 ***************************/

void loop()
{
  /* Server Controller */
  server.handleClient();

  /* Temperature Controller,*/
  TemperatureController();

  /* Sampling 0.5 second */
  delay(500);
}
