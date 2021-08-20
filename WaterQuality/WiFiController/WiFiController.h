/*
 * WiFi Controller (ESP32 WROOM 32)
 */

#include <WiFi.h>
#include <WebServer.h>


/**********************************
 * WIFI CONTROLLER CONFIGURATIONS *
 **********************************/

/* WiFi Network */
#define WIFI_NETWORK                    "AquaNetwork"
#define WIFI_PASSWORD                   "xja49ukqz4zyz83umqq8"

/* WiFi Controller - Server Port */
#define WIFI_SERVER_PORT                80

/* WiFi Controller - Client Config */
#define WIFI_CLIENT_IP                  "192.168.1.61"
#define WIFI_CLIENT_PORT                80


/************************************
 * WIFI CONTROLLER CLASS DEFINITION *
 ************************************/

class WiFiController {

  public:

  /* Constructor */
  WiFiController();

  /* Setup */
  void setup();

  /* Add available URI */
  void addUri(char uri[], THandlerFunction handler);

  /* Start Server */
  void begin();

  /* Client Handler */
  void handleClient();

  /* Received Argument Verification */
  bool argVerification(char[] argName);

  /* Server Response - Success (200, OK) */
  void serverSuccess(char response[]);

  /* Server Response - Invalid Argument (400, Bad Request) */
  void serverInvalidArg();


  private:

  /* Server Response - Unsupported Operation (404, Not Found) */
  void serverUnsupportedOperation();

};