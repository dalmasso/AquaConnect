/*
 * Temperature Sensor (DS18B20)
 */

#include <OneWire.h>


/******************************
 * TEMPERATURE CONFIGURATIONS *
 ******************************/

/* DS18B20 Temperature Sensor GPIO */
#define DS18B20_GPIO                  25

/* DS18B20 Commands */
#define DS18B20_TEMPERATURE           0x44
#define DS18B20_SCRATCHPAD            0xBE
#define DS18B20_SCRATCHPAD_LENGTH     9
#define DS18B20_TEMP_LSB              0
#define DS18B20_TEMP_MSB              1


/********************************
 * TEMPERATURE CLASS DEFINITION *
 ********************************/

class Temperature {

  public:

  /* Constructor */
  Temperature();

  /* Measure Temperature (in °C) */
  void measurement();

  /* Get Temperature (in °C) */
  float getTemperature() const;


  private:

  /* Temperature value (in °C) */
  float temperature;
};