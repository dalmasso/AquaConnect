/*
 * Carbonate Hardness (KH) Measurement Approximation
 */


/*************************************
 * CARBONATE HARDNESS CONFIGURATIONS *
 *************************************/

/* Carbonate Hardness (KH in dKH) Conversion from Conductivity (µS/cm) */
/* Apporximation: 1 KH => 34 µS/cm (ref: https://www.aquariophilie-aquarium.fr/Eau/Alcalinite-KH/unites-KH.html) */
#define KH_EC_CONVERSION(_ec_)        _ec_ / 34.0


/***************************************
 * CARBONATE HARDNESS CLASS DEFINITION *
 ***************************************/

class CarbonateHardness {

  public:

  /* Constructor */
  CarbonateHardness();

  /* Measure Carbonate Hardness (KH in dKH) */
  void measurement(float ec);

  /* Get Carbonate Hardness (KH in dKH) */
  float getKh() const;


  private:

  /* Carbonate Hardness (KH in dKH) */
  float kh;

};