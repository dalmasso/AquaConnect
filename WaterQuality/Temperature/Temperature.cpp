/*
 * Temperature Sensor (DS18B20)
 */

#include "Temperature.h"


/************************************
 * TEMPERATURE CLASS IMPLEMENTATION *
 ************************************/

/*
 * Temperature Constructor
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
  temperature = (float) rawTemp / 16.0;
}


/*
 * Get Temperature (in °C)
 */
float Temperature::getTemperature() const {
  return temperature;
}