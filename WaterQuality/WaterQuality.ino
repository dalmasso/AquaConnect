/*
 * ATtiny85 Water Quality Sensor
 * ATtiny85 PINOUT:
 *   -----------------------
    |         ----        * | P5 / ADC
 *  ---      |AT85|       * | P4 / PWM / ADC / USB-
 * |   |      ----        * | P3 / PWM / ADC / USB+
 * |   |                  * | P2 / ADC / SCL / USCK
 *  ---   5V   GND  VIN   * | P1 / DO / PWM
 *  |      *    *    *    * | P0 / AREF / SDA / DI / PWM         
 *   -----------------------
 */

#include "WaterQuality.h"


/********************************
 * WATER QUALITY CONFIGURATIONS *
 ********************************/


/* Server API */
#define TEMPERATURE_GET_TEMP          "/GET_TEMP"
#define TEMPERATURE_GET_SPAN          "/GET_SPAN"
#define TEMPERATURE_SET_SPAN          "/SET_SPAN"
#define TEMPERATURE_SET_SPAN_ARG      "Span"
/* Formatted Temperaure length (format: xx.x\n) */
#define TEMPERATURE_STR_LENGTH        5+1


/* Server API */
#define CONDUCTIVITY_GET_EC                           "/GET_EC"
#define CONDUCTIVITY_GET_TDS                          "/GET_TDS"
#define CONDUCTIVITY_GET_EC_TDS                       "/GET_EC_TDS"


/* Default Conductivity Span (in minutes) */
#define CONDUCTIVITY_SPAN_DEFAULT                     15

/* Time Conversion */
#define SECS_PER_MIN                                  (60UL)
#define ELAPSED_MINUTES(_time_)                       ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)

  /* Water Quality Span (in minutes) */
  unsigned int waterQualitySpan;

  /* Previous Water Quality Measurement */
  unsigned int previousMeasureTime;

/***********************
 * WATER QUALITY SETUP *
 ***********************/

void setup() {

}


/******************************
 * WATER QUALITY MAIN PROGRAM *
 ******************************/

void loop() {

}