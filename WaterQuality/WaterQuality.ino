/*
 * Water Quality Module (ESP32 WROOM 32)
 * Sensors:
 *  - Conductivity Sensor
 *  - ph Sensor
 *  - Temperature Sensor
 *
 * Estimation:
 *  - Carbonate Hardness Estimation
 *  - CO2 Estimation
 */

#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

#include "CarbonateHardness/CarbonateHardness.h"
#include "CO2/CO2.h"
#include "Conductivity/Conductivity.h"
#include "GeneralHardness/GeneralHardness.h"
#include "pH/pH.h"
#include "Temperature/Temperature.h"


/********************************
 * WATER QUALITY CONFIGURATIONS *
 ********************************/

/* Default Water Quality Span (in minutes) */
#define WATER_QUALITY_SPAN_DEFAULT              0 // TODO

/* Formatted Water Quality Span Length (format: x...x\n) */
#define WATER_QUALITY_SPAN_STR_LENGTH           10+2

/* JSON Water Quality Length */
#define WATER_QUALITY_JSON_LENGTH               200

/* JSON Water Quality Elements */
#define WATER_QUALITY_JSON_CARBONATE_HARDNESS   "CarbonateHardness"
#define WATER_QUALITY_JSON_CO2                  "CO2"
#define WATER_QUALITY_JSON_CONDUCTIVITY_EC      "EC"
#define WATER_QUALITY_JSON_CONDUCTIVITY_TDS     "TDS"
#define WATER_QUALITY_JSON_GENERAL_HARDNESS     "GeneralHardness"
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
#define WIFI_DATABASE_SERVER                    "http://192.168.1.61:5000/WaterQuality"
#define WIFI_DATABASE_TIMEOUT_MS                3000


/**********************************
 * WATER QUALITY GLOABL VARIABLES *
 **********************************/

/* Water Quality - Carbonate Hardness Estimation */
CarbonateHardness carbonateHardness;

/* Water Quality - CO2 Estimation  */
CO2 co2;

/* Water Quality - Conductivity */
Conductivity conductivity;

/* Water Quality - General Hardness Estimation */
GeneralHardness generalHardness;

/* Water Quality - ph */
PH ph;

/* Water Quality - Temperature */
Temperature temperature;

/* WiFi Server Object */
WebServer server(WIFI_WATER_QUALITY_SERVER_PORT);

/* Water Quality - Span Measure (in minutes) */
unsigned int waterQualitySpan = WATER_QUALITY_SPAN_DEFAULT;

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

  /* Send Water Quality Analysis & Server Response */
  if (clientSendWaterQuality()) {
    server.send(200, "text/plain", "OK\n");
  
  } else {
    server.send(400, "text/plain", "Connection to Database failed\n");
  }
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
  waterQualityJSON[WATER_QUALITY_JSON_CARBONATE_HARDNESS] = carbonateHardness.getKh();

  /* CO2 */
  waterQualityJSON[WATER_QUALITY_JSON_CO2] = co2.getCo2();

  /* Conductivity (EC & TDS) */
  waterQualityJSON[WATER_QUALITY_JSON_CONDUCTIVITY_EC] = conductivity.getEc();
  waterQualityJSON[WATER_QUALITY_JSON_CONDUCTIVITY_TDS] = conductivity.getTds();

  /* General Hardness */
  waterQualityJSON[WATER_QUALITY_JSON_GENERAL_HARDNESS] = generalHardness.getGh();

  /* pH */
  waterQualityJSON[WATER_QUALITY_JSON_PH] = ph.getPh();

  /* Temperature */
  waterQualityJSON[WATER_QUALITY_JSON_TEMPERATURE] = temperature.getTemperature();  

  /* Serialize JSON */
  serializeJson(waterQualityJSON, serializedJSON, WATER_QUALITY_JSON_LENGTH);
}


/*
 * Client Operation - Send Water Quality analysis
 * Return True if Send successed, else False
 */
bool clientSendWaterQuality() {
  Serial.println("Send");
  /* Send Result */
  bool result = false;

  /* Check WiFi Status */
  if (WiFi.status() == WL_CONNECTED) {

    /* HTTP Client Objects */
    HTTPClient http;

    /* JSON String containing Water Quality Results */
    char serializedJSON[WATER_QUALITY_JSON_LENGTH];

    /* Client Connection */
    http.setConnectTimeout(WIFI_DATABASE_TIMEOUT_MS);
    http.begin(WIFI_DATABASE_SERVER);

    /* Specify content-type header */
    http.addHeader("Content-Type", "application/json");

    /* Create JSON containing Water Quality Results */
    clientCreateWaterQualityJSON(serializedJSON);

    /* Send Water Quality Results */
    int httpResult;
    httpResult = http.POST(serializedJSON);
    if (httpResult == HTTP_CODE_OK) {
      result = true;
      Serial.println(serializedJSON);
    }

    /* Disconnect Client */
    http.end();

    Serial.println(httpResult);
    
  }
  Serial.println(result);
  return result;
}


/**************************************************
 * WATER QUALITY SENSORS & Estimation MANAGER *
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

  /* General Hardness */
  generalHardness.measurement(conductivity.getEc());

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

  /* WiFi Server Configuration as Station mode */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  /* Connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
  }

  /* Init Sensors (no required for Temperature & Estimation) */
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