/*
 * Conductivity Sensor: Electrical Conductivity (EC, in µS/cm) & Total Dissolved Solids (TDS, in ppm)
 * Reference: https://wiki.keyestudio.com/KS0429_keyestudio_TDS_Meter_V1.0
 */

#include "Conductivity/Conductivity.h"


/*************************************
 * CONDUCTIVITY CLASS IMPLEMENTATION *
 *************************************/

/*
 * Constructor
 */
Conductivity::Conductivity() :
  ec(0.0),
  tds(0.0) {}


/* 
 * Setup
 */
void Conductivity::setup() {

  /* KS0429 Conductivity Sensor GPIO Input Mode */
  pinMode(KS0429_GPIO, INPUT);
}


/*
 * Measure Conductivity (EC in µS/cm & TDS in ppm)
 */
void Conductivity::measurement(float temperature) {

  /* Read KS0429 Sensor */
  int ks0429Buffer[KS0429_SAMPLE_POINTS];
  ks0429Read(ks0429Buffer);

  /* Apply Median Filter Algorithm */
  int raw;
  raw = ks0429MedianFilter(ks0429Buffer);

  /* Convert Raw value to Voltage value */
  float voltage;
  voltage = KS0429_VOLTAGE_CONVERSION(raw);

  /* Convert Voltage value to EC value */
  float rawEC;
  rawEC = KS0429_EC_CONVERSION(voltage);

  /* Compute EC with Temperature compensation */
  this->ec = KS0429_EC_TEMPERATURE_COMP(rawEC, temperature);

  /* Convert EC value to TDS value */
  this->tds = KS0429_TDS_CONVERSION(this->ec);
}


/*
 * Get Electrical Conductivity (EC, in µS/cm)
 */
float Conductivity::getEc() const {
  return this->ec;
}


/*
 * Get Total Dissolved Solids (TDS, in ppm)
 */
float Conductivity::getTds() const {
  return this->tds;
}


/*
 * Read KS0429 current raw value and store it into buffer
 */
void Conductivity::ks0429Read(int ks0429Buffer[]) {

  /* Read the KS0429 Sensor */
  for(int i=0; i<KS0429_SAMPLE_POINTS; i++) {

    ks0429Buffer[i] = analogRead(KS0429_GPIO);
    delay(40);
  }
}


/*
 * Get KS049 value through Median Filter Algorithm (for value stability)
 */
int Conductivity::ks0429MedianFilter(int ks0429Buffer[]) {

  int ks0429BufferTemp[KS0429_SAMPLE_POINTS];
  int ks0429ValueTemp;

  /* Copy KS0429 Buffer Values */
  for (byte i = 0; i<KS0429_SAMPLE_POINTS; i++)
    ks0429BufferTemp[i] = ks0429Buffer[i];
  
  /* Apply Median Filter Algorithm */
  for (int j = 0; j < KS0429_SAMPLE_POINTS - 1; j++) {

    for (int i = 0; i < KS0429_SAMPLE_POINTS - j - 1; i++) {
      if (ks0429BufferTemp[i] > ks0429BufferTemp[i + 1]) {
        ks0429ValueTemp = ks0429BufferTemp[i];
        ks0429BufferTemp[i] = ks0429BufferTemp[i + 1];
        ks0429BufferTemp[i + 1] = ks0429ValueTemp;
      }
    }
  }
  
  if ((KS0429_SAMPLE_POINTS & 1) > 0)
    ks0429ValueTemp = ks0429BufferTemp[(KS0429_SAMPLE_POINTS - 1) / 2];
  else
    ks0429ValueTemp = (ks0429BufferTemp[KS0429_SAMPLE_POINTS / 2] + ks0429BufferTemp[KS0429_SAMPLE_POINTS / 2 - 1]) / 2;
  return ks0429ValueTemp;
}