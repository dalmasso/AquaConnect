/*
 * Temperature Sensor (DS18B20)
 */

#include "Temperature/Temperature.h"


/**************************************************
 * TEMPERATURE CLASS CONSTANTS & GLOBAL VARIABLES *
 **************************************************/

/* Temperature Sensor DS18B20 Access */
OneWire DS18B20(DS18B20_GPIO);

/* DS18B20 OneWire Address */
const byte DS18B20Addr[] = {0x28, 0xD1, 0x19, 0x75, 0xD0, 0x01, 0x3C, 0x8B};


/************************************
 * TEMPERATURE CLASS IMPLEMENTATION *
 ************************************/

/*
 * Constructor
 */
Temperature::Temperature() :
  temperature(0.0) {}


/*
 * Measure Temperature (in °C)
 */
void Temperature::measurement() {

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
  this->temperature = (float) rawTemp / 16.0;
}


/*
 * Get Temperature (in °C)
 */
float Temperature::getTemperature() const {
  return this->temperature;
}