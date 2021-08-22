/*
 * pH Sensor (SEN0161)
 * Reference: https://wiki.dfrobot.com/Gravity__Analog_pH_Sensor_Meter_Kit_V2_SKU_SEN0161-V2
 */

#include "pH.h"


/***************************
 * PH CLASS IMPLEMENTATION *
 ***************************/

/*
 * Constructor
 */
PH::PH() :
  pH(0.0) {}


/* 
 * Setup
 */
void PH::setup() {

  /* SEN0161 pH Sensor GPIO Input Mode */
  pinMode(SEN0161_GPIO, INPUT);
}


/*
 * Measure pH
 */
void PH::measurement() {

  /* Read SEN0161 Sensor */
  int sen0161Buffer[SEN0161_SAMPLE_POINTS];
  sen0161Read(sen0161Buffer);

  /* Apply Median Filter Algorithm */
  int raw;
  raw = sen0161MedianFilter(sen0161Buffer);

  /* Convert Raw value to Voltage value */
  float voltage;
  voltage = SEN0161_VOLTAGE_CONVERSION(raw);

  /* Convert Voltage value to pH value */
  this->pH = SEN0161_PH_CONVERSION(voltage);
}


/*
 * Get pH
 */
float PH::getPh() const {
  return this->pH;
}


/*
 * Read SEN0161 current raw value and store it into buffer
 */
void PH::sen0161Read(int sen0161Buffer[]) {

  /* Read the SEN0161 Sensor */
  for(int i=0; i<SEN0161_SAMPLE_POINTS; i++) {

    sen0161Buffer[i] = analogRead(SEN0161_GPIO);
    delay(40);
  }
}


/*
 * Get SEN0161 value through Median Filter Algorithm (for value stability)
 */
int PH::sen0161MedianFilter(int sen0161Buffer[]) {

  int sen0161BufferTemp[SEN0161_SAMPLE_POINTS];
  int sen0161ValueTemp;

  /* Copy SEN0161 Buffer Values */
  for (byte i = 0; i<SEN0161_SAMPLE_POINTS; i++)
    sen0161BufferTemp[i] = sen0161Buffer[i];
  
  /* Apply Median Filter Algorithm */
  for (int j = 0; j < SEN0161_SAMPLE_POINTS - 1; j++) {

    for (int i = 0; i < SEN0161_SAMPLE_POINTS - j - 1; i++) {
      if (sen0161BufferTemp[i] > sen0161BufferTemp[i + 1]) {
        sen0161ValueTemp = sen0161BufferTemp[i];
        sen0161BufferTemp[i] = sen0161BufferTemp[i + 1];
        sen0161BufferTemp[i + 1] = sen0161ValueTemp;
      }
    }
  }
  
  if ((SEN0161_SAMPLE_POINTS & 1) > 0)
    sen0161ValueTemp = sen0161BufferTemp[(SEN0161_SAMPLE_POINTS - 1) / 2];
  else
    sen0161ValueTemp = (sen0161BufferTemp[SEN0161_SAMPLE_POINTS / 2] + sen0161BufferTemp[SEN0161_SAMPLE_POINTS / 2 - 1]) / 2;
  return sen0161ValueTemp;
}