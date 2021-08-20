/*
 * Conductivity Sensor: Electrical Conductivity (EC, in µS/cm) & Total Dissolved Solids (TDS, in ppm)
 * Reference: https://wiki.keyestudio.com/KS0429_keyestudio_TDS_Meter_V1.0
 */

#include <Arduino.h>


/*******************************
 * CONDUCTIVITY CONFIGURATIONS *
 *******************************/

/* KS0429 Conductivity Sensor GPIO */
#define KS0429_GPIO                                   4 // TODO

/* KS0429 Measure Sample Point */
#define KS0429_SAMPLE_POINTS                          30

/* KS0429 ADC Voltage Conversion */
#define KS0429_ADC_VREF                               5.0
#define KS0429_ADC_RESOLUTION_BITS                    10
#define KS0429_VOLTAGE_CONVERSION(_voltage_)          _voltage_ * ((float) KS0429_ADC_VREF / (1 << KS0429_ADC_RESOLUTION_BITS))

/* KS0429 Temperature Compensation */
#define KS0429_TEMPERATURE_COMP(_voltage_, _temp_)    _voltage_ / ((float) 1.0 + 0.02 * (_temp_ - 25.0))

/* KS0429 Voltage to EC Conversion (in µS/cm) */
#define KS0429_EC_CONVERSION(_voltage_)               (133.42*_voltage_*_voltage_*_voltage_ - 255.86*_voltage_*_voltage_ + 857.39*_voltage_)

/* KS0429 EC to TDS Conversion (in ppm) */
#define KS0429_TDS_CONVERSION(_ec_)                   _ec_ * 0.5


/*********************************
 * CONDUCTIVITY CLASS DEFINITION *
 *********************************/

class Conductivity {

  public:

  /* Constructor */
  Conductivity();

  /* Setup */
  void setup();

  /* Measure Conductivity (EC in µS/cm & TDS in ppm) */
  void measurement(float temperature);

  /* Get Electrical Conductivity (EC, in µS/cm) */
  float getEc() const;

  /* Get Total Dissolved Solids (TDS, in ppm) */
  float getTds() const;


  private:

  /* Read KS0429 current raw value and store it into buffer */
  void ks0429Read(int ks0429Buffer[]);

  /* Get KS049 value through Median Filter Algorithm (for value stability) */
  int ks0429MedianFilter(int ks0429Buffer[]);

  /* Electrical Conductivity value (EC, in µS/cm) */
  float ec;

  /* Total Dissolved Solids value (TDS, in ppm) */
  float tds;

};
