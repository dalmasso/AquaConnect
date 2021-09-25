/*
 * Carbonate Hardness (KH) Estimation
 */

#include "CarbonateHardness/CarbonateHardness.h"


/*******************************************
 * CARBONATE HARDNESS CLASS IMPLEMENTATION *
 *******************************************/

/*
 * Constructor
 */
CarbonateHardness::CarbonateHardness() :
  kh(0.0) {}


/*
 * Measure Carbonate Hardness (KH in dKH)
 */
void CarbonateHardness::measurement(float ec) {

  /* Carbonate Hardness (KH in dKH) Conversion from Conductivity (µS/cm) */
  this->kh = KH_EC_CONVERSION(ec);
}


/*
 * Get Carbonate Hardness (KH in dKH)
 */
float CarbonateHardness::getKh() const {
  return this->kh;
}