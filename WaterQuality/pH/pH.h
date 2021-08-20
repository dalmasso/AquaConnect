/*
 * pH Sensor (SEN0161)
 * Reference: https://wiki.dfrobot.com/PH_meter_SKU__SEN0161_
 */

#include <Arduino.h>


/*********************
 * PH CONFIGURATIONS *
 *********************/

/* SEN0161 pH Sensor GPIO */
#define SEN0161_GPIO                                2 // TODO

/* SEN0161 Measure Sample Point */
#define SEN0161_SAMPLE_POINTS                       40

/* SEN0161 ADC Voltage Conversion */
#define SEN0161_ADC_VREF                            5.0
#define SEN0161_ADC_RESOLUTION_BITS                 10
#define SEN0161_VOLTAGE_CONVERSION(_voltage_)       _voltage_ * ((float) SEN0161_ADC_VREF / (1 << SEN0161_ADC_RESOLUTION_BITS))

/* SEN0161 Offset Compensation */
#define SEN0161_OFFSET_COMP                         0 //TODO

/* SEN0161 Voltage to pH Conversion */
#define SEN0161_PH_CONVERSION(_voltage_)            3.5 * _voltage_ + SEN0161_OFFSET_COMP


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