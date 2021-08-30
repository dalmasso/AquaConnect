/*
 * Carbon Dioxide (CO2) Estimation
 * Reference: https://www.aquariophilie-aquarium.fr/Eau/Gaz/calcul-taux-CO2.html
 * Converter: https://www.aquachange.fr/forum_aquariophilie.php?espace=1&groupe=4&sujet=7950  
 */

#include "CO2.h"


/***************************************
 * CARBON DIOXIDE CLASS IMPLEMENTATION *
 ***************************************/

/*
 * Constructor
 */
CO2::CO2() :
  co2(0.0) {}


/*
 * Measure Carbon Dioxide (CO2 in mg/L)
 */
void CO2::measurement(float ph, float kh) {

  /* Carbon Dioxide (CO2 in mg/L) Conversion from pH (NaN) and KH (dKH) */
  this->co2 = (CO2_KH_COEFF * kh * pow(10, (CO2_PH_COEFF - ph))) * CO2_OFFSET_CALIBRATION;
}


/*
 * Get Carbon Dioxide (CO2 in mg/L)
 */
float CO2::getCo2() const {
  return this->co2;
}