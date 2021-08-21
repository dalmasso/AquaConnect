/*
 * Water Quality Module (ESP32 WROOM 32)
 * Sensors:
 *  - Conductivity Sensor
 *  - ph Sensor
 *  - Temperature Sensor
 *
 * Approximations:
 *  - Carbonate Hardness Approximation
 *  - CO2 Approximation
 */

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>

#include "CarbonateHardness/CarbonateHardness.h"
#include "CO2/CO2.h"
#include "Conductivity/Conductivity.h"
#include "pH/pH.h"
#include "Temperature/Temperature.h"


/********************************
 * WATER QUALITY CONFIGURATIONS *
 ********************************/

/* Default Water Quality Span (in minutes) */
#define WATER_QUALITY_SPAN_DEFAULT              10

/* Formatted Water Quality Span Length (format: x...x\n) */
#define WATER_QUALITY_SPAN_STR_LENGTH           10+2

/* JSON Water Quality Length */
#define WATER_QUALITY_JSON_LENGTH               200

/* JSON Water Quality Elements */
#define WATER_QUALITY_JSON_CARBONATEHARDNESS    "CarbonateHardness"
#define WATER_QUALITY_JSON_CO2                  "CO2"
#define WATER_QUALITY_JSON_CONDUCTIVITY_EC      "EC"
#define WATER_QUALITY_JSON_CONDUCTIVITY_TDS     "TDS"
#define WATER_QUALITY_JSON_PH                   "pH"
#define WATER_QUALITY_JSON_TEMPERATURE          "Temperature"

/* Time Conversion */
#define SECS_PER_MIN                            (60UL)
#define ELAPSED_MINUTES(_time_)                 ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)

/* Water Quality Controller - Server API */
#define WATER_QUALITY_API_MANUAL                "/WATER_QUALITY/MANUAL"
#define WATER_QUALITY_API_SPAN_GET              "/WATER_QUALITY/SPAN/GET"
#define WATER_QUALITY_API_SPAN_SET              "/WATER_QUALITY/SPAN/SET"
#define WATER_QUALITY_API_SPAN_ARG              "Span"

/* WiFi Network */
#define WIFI_NETWORK                            "AquaNetwork"
#define WIFI_PASSWORD                           "xja49ukqz4zyz83umqq8"

/* WiFi Server Config */
#define WIFI_WATER_QUALITY_SERVER_PORT          80

/* WiFi Database Client Config */
#define WIFI_DATABASE_IP                        "192.168.1.61"
#define WIFI_DATABASE_PORT                      80
#define WIFI_DATABASE_URI                       "/WATER_QUALITY "
#define WIFI_DATABASE_SEND_METHOD               "POST "
#define WIFI_DATABASE_HTTP_VERSION              "HTTP/1.1"
#define WIFI_DATABASE_CONTENT_TYPE              "Content-Type: application/json"
#define WIFI_DATABASE_CLOSE_CONNECTION          "Connection: close"
#define WIFI_DATABASE_CONNECTION_RETRIES        5


/**********************************
 * WATER QUALITY GLOABL VARIABLES *
 **********************************/

/* Water Quality - Conductivity */
Conductivity conductivity;

/* Water Quality - ph */
PH ph;

/* Water Quality - Temperature */
Temperature temperature;

/* Water Quality - Carbonate Hardness Approximation */
CarbonateHardness carbonateHardness;

/* Water Quality - CO2 Approximation  */
CO2 co2;

/* WiFi Server Object */
WebServer server(WIFI_WATER_QUALITY_SERVER_PORT);

/* Water Quality - Span Measure (in minutes) */
unsigned int waterQualitySpan;

/* Water Quality - Previous Water Quality Measurement */
unsigned int previousAnalysisTime;


/********************************
 * WATER QUALITY SERVER MANAGER *
 ********************************/

/*
 * Server Operation - Manual Water Quality analysis 
 */
void serverWaterQualityManual() {

  /* Start Water Quality Analysis */
  waterQualityAnalysis();

  /* Send Water Quality Analysis */
  clientSendWaterQuality();

  /* Server Operation Result */
  server.send(200, "text/plain", "OK\n");
}


/*
 * Server Operation - Get Water Quality Span 
 */
void serverWaterQualityGetSpan() {

  /* Format Water Quality Span value */
  char formattedSpan[WATER_QUALITY_SPAN_STR_LENGTH];
  snprintf(formattedSpan, WATER_QUALITY_SPAN_STR_LENGTH, "%d\n", waterQualitySpan);
    
  /* Send formatted Water Quality Span */
  server.send(200, "text/plain", formattedSpan);
}


/*
 * Server Operation - Set Water Quality Span 
 */
void serverWaterQualitySetSpan() {

  /* Check new Water Quality Span value */
  if (!server.hasArg(WATER_QUALITY_API_SPAN_ARG)) {
    server.send(400, "text/plain", "No new Water Quality Span to set\n");

  } else {

    /* Convert String Span to unsigned int */
    unsigned int newWaterQualitySpan = server.arg(WATER_QUALITY_API_SPAN_ARG).toInt();

    /* Verify new Water Quality Span */
    if (newWaterQualitySpan == 0) {
      server.send(400, "text/plain", "Invalid new Water Quality Span (0)\n");

    } else {

      /* Set the new Water Quality Span & Reset current Time */
      waterQualitySpan = newWaterQualitySpan;
      previousAnalysisTime = ELAPSED_MINUTES(millis() / 1000);

      /* Server Operation Result */
      server.send(200, "text/plain", "OK\n");
    }
  }
}


/*
 * Server Response - Unsupported Operation (501, Not Implemented) 
 */
void serverUnsupportedOperation() {
  server.send(501, "text/plain", "Unsupported Operation\n");
}


/********************************
 * WATER QUALITY CLIENT MANAGER *
 ********************************/

/*
 * Client Create JSON containing Water Quality Results
 */
void clientCreateWaterQualityJSON(char serializedJSON[WATER_QUALITY_JSON_LENGTH]) {

  /* Format Water Quality Result (JSON) */
  StaticJsonDocument<WATER_QUALITY_JSON_LENGTH> waterQualityJSON;

  /* Carbonate Hardness */
  waterQualityJSON[WATER_QUALITY_JSON_CARBONATEHARDNESS] = carbonateHardness.getKh();

  /* CO2 */
  waterQualityJSON[WATER_QUALITY_JSON_CO2] = co2.getCo2();

  /* Conductivity (EC & TDS) */
  waterQualityJSON[WATER_QUALITY_JSON_CONDUCTIVITY_EC] = conductivity.getEc();
  waterQualityJSON[WATER_QUALITY_JSON_CONDUCTIVITY_TDS] = conductivity.getTds();

  /* pH */
  waterQualityJSON[WATER_QUALITY_JSON_PH] = ph.getPh();

  /* Temperature */
  waterQualityJSON[WATER_QUALITY_JSON_TEMPERATURE] = temperature.getTemperature();  

  /* Serialize JSON */
  serializeJson(waterQualityJSON, serializedJSON, WATER_QUALITY_JSON_LENGTH);
}


/*
 * Client Operation - Send Water Quality analysis 
 */
void clientSendWaterQuality() {

  /* WiFi Client Object */
  WiFiClient client;

  /* JSON String containing Water Quality Results */
  char serializedJSON[WATER_QUALITY_JSON_LENGTH];

  /* Client Connection */
  int retry = WIFI_DATABASE_CONNECTION_RETRIES;
  while( !client.connect(WIFI_DATABASE_IP, WIFI_DATABASE_PORT) ) {
    retry--;

    /* Connection failed */
    if (retry == 0) {
      return;
    }
    
    delay(200);
  }

  /* Create JSON containing Water Quality Results */
  clientCreateWaterQualityJSON(serializedJSON);

  /* Send Water Quality Results */
  client.print(WIFI_DATABASE_SEND_METHOD);
  client.print(WIFI_DATABASE_URI);
  client.println(WIFI_DATABASE_HTTP_VERSION);
  client.print("Host: ");
  client.println(WIFI_DATABASE_IP);
  client.println(serializedJSON);
  client.println(WIFI_DATABASE_CONTENT_TYPE);
  client.println(WIFI_DATABASE_CLOSE_CONNECTION);
  client.println("");

  /* Disconnect Client */
  client.stop();
}


/**************************************************
 * WATER QUALITY SENSORS & APPROXIMATIONS MANAGER *
 **************************************************/

/*
 * Start Water Quality Analysis
 */
void waterQualityAnalysis() {

  /* Temperature */
  temperature.measurement();

  /* Conductivity */
  conductivity.measurement(temperature.getTemperature());

  /* pH */
  ph.measurement();

  /* Carbonate Hardness */
  carbonateHardness.measurement(conductivity.getEc());

  /* CO2 */
  co2.measurement(ph.getPh(), carbonateHardness.getKh());
}


/*
 * Water Quality Controller
 */
void waterQualityController() {

  /* Get current Time in minutes */
  unsigned int currentTimeMinute = ELAPSED_MINUTES(millis() / 1000);

  /* Check Water Quality Span */
  if ( (currentTimeMinute - previousAnalysisTime) >= waterQualitySpan ) {

    /* Reset previous Water Quality Analysis time */
    previousAnalysisTime = currentTimeMinute;

    /* Perform Water Quality Analysis */
    waterQualityAnalysis();

    /* Send Results */
    clientSendWaterQuality();
  }
}


/***********************
 * WATER QUALITY SETUP *
 ***********************/

void setup() {

  Serial.begin(115200);
  Serial.println("Hello");

  /* WiFi Server Configuration as Station mode */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  /* Connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  /* Init Sensors (no required for Temperature & Approximations) */
  conductivity.setup();
  ph.setup();

  /* Configure Server */
  server.on(WATER_QUALITY_API_MANUAL, serverWaterQualityManual);
  server.on(WATER_QUALITY_API_SPAN_GET, serverWaterQualityGetSpan);
  server.on(WATER_QUALITY_API_SPAN_SET, serverWaterQualitySetSpan);

  /* Add Server Configuration - Unsupported Operation */
  server.onNotFound(serverUnsupportedOperation);

  /* Start Server */
  server.begin();
}


/******************************
 * WATER QUALITY MAIN PROGRAM *
 ******************************/

void loop() {

  /* Server Handle Client */
  server.handleClient();

  /* Water Quality Controller */
  waterQualityController();

  /* Sampling 0.5 second */
  delay(500);
}