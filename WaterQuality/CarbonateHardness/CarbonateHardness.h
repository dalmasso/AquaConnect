/*
 * Carbonate Hardness (KH) Estimation
 */


/*************************************
 * CARBONATE HARDNESS CONFIGURATIONS *
 *************************************/

/* Carbonate Hardness Offset Calibration */
#define KH_OFFSET_CALIBRATION         0.433

/* Carbonate Hardness (KH in dKH) Conversion from Conductivity (µS/cm) */
/* Estimation: 1 dKH => 30 µS/cm (ref: http://aquafish.free.fr/tables/tables.htm) */
#define KH_EC_CONVERSION(_ec_)        (_ec_ / 30.0) * KH_OFFSET_CALIBRATION


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