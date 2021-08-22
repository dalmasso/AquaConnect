/*
 * General Hardness (GH) Estimation
 */

#include "GeneralHardness.h"


/*****************************************
 * GENERAL HARDNESS CLASS IMPLEMENTATION *
 *****************************************/

/*
 * Constructor
 */
GeneralHardness::GeneralHardness() :
  gh(0.0) {}


/*
 * Measure General Hardness (GH in dGH)
 */
void GeneralHardness::measurement(float ec) {

  /* General Hardness (GH in dGH) Conversion from Conductivity (µS/cm) */
  this->gh = GH_EC_CONVERSION(ec);
}


/*
 * Get General Hardness (GH in dGH)
 */
float GeneralHardness::getGh() const {
  return this->gh;
}