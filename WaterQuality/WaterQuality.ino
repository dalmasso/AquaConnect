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

#include "CarbonateHardness/CarbonateHardness.h"
#include "CO2/CO2.h"
#include "Conductivity/Conductivity.h"
#include "pH/pH.h"
#include "Temperature/Temperature.h"
// #include "WiFiController/WiFiController.h"


/********************************
 * WATER QUALITY CONFIGURATIONS *
 ********************************/

/* Default Water Quality Span (in minutes) */
#define WATER_QUALITY_SPAN_DEFAULT      10

/* Time Conversion */
#define SECS_PER_MIN                    (60UL)
#define ELAPSED_MINUTES(_time_)         ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)

/* Water Quality Controller - Server API */
#define WATER_QUALITY_API_MANUAL        "/WATER_QUALITY/MANUAL"
#define WATER_QUALITY_API_SPAN_GET      "/WATER_QUALITY/SPAN/GET"
#define WATER_QUALITY_API_SPAN_SET      "/WATER_QUALITY/SPAN/SET"
#define WATER_QUALITY_API_SPAN__ARG     "Span"


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

/* Water Quality - WiFi Controller */
// WiFiController wifiController();

/* Water Quality - Span Measure (in minutes) */
unsigned int waterQualitySpan;

/* Water Quality - Previous Water Quality Measurement */
unsigned int previousMeasureTime;


/****************************
 * WATER QUALITY CONTROLLER *
 ****************************/




/***********************
 * WATER QUALITY SETUP *
 ***********************/

void setup() {

  Serial.begin(115200);
  Serial.println("Hello");

  /* Init Sensors (no required for Temperature) */
  conductivity.setup();
  ph.setup();
}


/******************************
 * WATER QUALITY MAIN PROGRAM *
 ******************************/

void loop() {

}