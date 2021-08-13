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

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>

#include "Conductivity.h"


/***********************
 * WIFI CONFIGURATIONS *
 ***********************/

/* WiFi Network */
#define WIFI_NETWORK                  "AquaNetwork"
#define WIFI_PASSWORD                 "xja49ukqz4zyz83umqq8"

/* WiFi Client Config */
#define WIFI_CLIENT_IP                "192.168.1.61"
#define WIFI_CLIENT_PORT              80


/*************************
 * WIFI CLASS DEFINITION *
 *************************/

class WiFi {

  public:

  /* WiFi Constructor */
  WiFi();

  /* WiFi Setup */
  void setup();

  /* WiFi Handle Client */
  void handleClient();


  private:

  /* Server Object, available on port 80 */
  ESP8266WebServer server(80);
};