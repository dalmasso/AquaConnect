/*
 * Carbon Dioxide (CO2) Estimation
 * Reference: https://www.aquariophilie-aquarium.fr/Eau/Gaz/calcul-taux-CO2.html
 */

#include <Arduino.h>

/*********************************
 * CARBON DIOXIDE CONFIGURATIONS *
 *********************************/

/* Estimation KH and pH coefficients */
#define CO2_KH_COEFF              15.6896
#define CO2_PH_COEFF              6.31

/* CO2 Offset Calibration (for Soil & Injection compensations) */
#define CO2_OFFSET_CALIBRATION    2.20 //TODO


/***********************************
 * CARBON DIOXIDE CLASS DEFINITION *
 ***********************************/

class CO2 {

  public:

  /* Constructor */
  CO2();

  /* Measure Carbon Dioxide (CO2 in mg/L) */
  void measurement(float ph, float kh);

  /* Get Carbon Dioxide (CO2 in mg/L) */
  float getCo2() const;


  private:

  /* Carbon Dioxide value (CO2 in mg/L) */
  float co2;

};