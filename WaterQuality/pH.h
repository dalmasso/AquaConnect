/*
 * pH Sensor (SEN0161)
 * Reference: https://wiki.dfrobot.com/Gravity__Analog_pH_Sensor_Meter_Kit_V2_SKU_SEN0161-V2
 */

#include <Arduino.h>


/*********************
 * PH CONFIGURATIONS *
 *********************/

/* SEN0161 pH Sensor GPIO */
#define SEN0161_GPIO                                39 // TODO

/* SEN0161 Measure Sample Point */
#define SEN0161_SAMPLE_POINTS                       30

/* SEN0161 ADC Voltage Conversion */
#define SEN0161_ADC_VREF                            3.3
#define SEN0161_ADC_RESOLUTION_BITS                 12
#define SEN0161_VOLTAGE_CONVERSION(_adcRaw_)        _adcRaw_ * ((float) SEN0161_ADC_VREF / (1 << SEN0161_ADC_RESOLUTION_BITS))

/* SEN0161 Neutral Voltage Reference */
#define SEN0161_NEUTRAL_VOLTAGE_REF                 1500.0

/* SEN0161 Neutral Voltage Calibration */
#define SEN0161_NEUTRAL_VOLTAGE_CALIBRATION         1500.0

/* SEN0161 Acid Voltate Calibration */
#define SEN0161_ACIDE_VOLTAGE_CALIBRATION           2032.44

/* SEN0161 Slope */
#define SEN0161_SLOPE                               (7.0 - 4.0)/ ( (SEN0161_NEUTRAL_VOLTAGE_CALIBRATION - SEN0161_NEUTRAL_VOLTAGE_REF) / 3.0 - (SEN0161_ACIDE_VOLTAGE_CALIBRATION - SEN0161_NEUTRAL_VOLTAGE_REF) /3.0 )

/* SEN0161 Intercept */
#define SEN0161_INTERCEPT                           7.0 - SEN0161_SLOPE * (SEN0161_NEUTRAL_VOLTAGE_CALIBRATION - SEN0161_NEUTRAL_VOLTAGE_REF) / 3.0

/* SEN0161 Voltage to pH Conversion */
#define SEN0161_PH_CONVERSION(_voltage_)            SEN0161_SLOPE * (_voltage_ - SEN0161_NEUTRAL_VOLTAGE_REF) / 3.0 + SEN0161_INTERCEPT


/***********************
 * PH CLASS DEFINITION *
 ***********************/

class PH {

  public:

  /* Constructor */
  PH();

  /* Setup */
  void setup();

  /* Measure pH */
  void measurement();

  /* Get pH */
  float getPh() const;


  private:

  /* Read SEN0161 current raw value and store it into buffer */
  void sen0161Read(int sen0161Buffer[]);

  /* Get SEN0161 value through Median Filter Algorithm (for value stability) */
  int sen0161MedianFilter(int sen0161Buffer[]);

  /* pH value */
  float pH;

};