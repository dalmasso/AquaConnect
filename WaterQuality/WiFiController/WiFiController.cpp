/*
 * WiFi Server (ESP32 WROOM 32)
 */

#include "WiFiController.h"


/******************************************
 * WIFI CONTROLLER CLASS GLOBAL VARIABLES *
 ******************************************/

/* WiFi Controller - Server Object */
WebServer server(WIFI_SERVER_PORT);


/****************************************
 * WIFI CONTROLLER CLASS IMPLEMENTATION *
 ****************************************/

/*
 * Constructor
 */
WiFiController::WiFiController() {}


/* 
 * Setup
 */
void WiFiController::setup() {

  /* WiFi Server Configuration as Station mode */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  /* Connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}


/* 
 * Add available URI
 */
 void WiFiController::addUri(char uri[], THandlerFunction handler) {
  server.on(uri, handler);
}


/* 
 * Start Server
 */
 void WiFiController::begin() {

  /* Add Server Configuration - Unsupported Operation */
  server.onNotFound(serverUnsupportedOperation);

  /* Start Server */
  server.begin();
}


/*
 * Client Handler
 */
void WiFiController::handleClient() {
  server.handleClient();
}


/*
 * Received Argument Verification
 */
 bool WiFiController::argVerification(char argName[]) {
  return server.hasArg(argName);
}


/*
 * Server Response - Success (200, OK) 
 */
void WiFiController::serverSuccess(char response[]) {
  server.send(200, "text/plain", response);
}


/*
 * Server Response - Invalid Argument (400, Bad Request)
 */
void WiFiController::serverInvalidArg() {
  server.send(400, "text/plain", "Invalid Request Argument\n");
}


/*
 * Server Response - Unsupported Operation (404, Not Found) 
 */
void WiFiController::serverUnsupportedOperation() {
  server.send(501, "text/plain", "Unsupported Operation\n");
}