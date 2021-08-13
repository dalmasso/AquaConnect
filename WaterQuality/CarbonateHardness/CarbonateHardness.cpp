/*
 * Carbonate Hardness (KH) Measurement Approximation
 */

#include "CarbonateHardness.h"


/*******************************************
 * CARBONATE HARDNESS CLASS IMPLEMENTATION *
 *******************************************/

/*
 * Carbonate Hardness Constructor
 */
CarbonateHardness::CarbonateHardness() :
  kh(0.0) {}


/*
 * Measure Carbonate Hardness (KH in dKH)
 */
void CarbonateHardness::measurement(float ec) {

  /* Carbonate Hardness (KH in dKH) Conversion from Conductivity (µS/cm) */
  kh = KH_EC_CONVERSION(ec);
}


/*
 * Get Carbonate Hardness (KH in dKH)
 */
float CarbonateHardness::getKH() const {
  return kh;
}