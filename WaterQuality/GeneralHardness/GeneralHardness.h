/*
 * General Hardness (GH) Estimation
 */


/***********************************
 * GENERAL HARDNESS CONFIGURATIONS *
 ***********************************/

/* General Hardness Offset Calibration */
#define GH_OFFSET_CALIBRATION         1.05 //TODO

/* General Hardness (GH in dGH) Conversion from Conductivity (µS/cm) */
/* Estimation: 1 dGH => 38 µS/cm (ref: http://aquafish.free.fr/tables/tables.htm) */
#define GH_EC_CONVERSION(_ec_)        (_ec_ / 38.0) * GH_OFFSET_CALIBRATION


/*************************************
 * GENERAL HARDNESS CLASS DEFINITION *
 *************************************/

class GeneralHardness {

  public:

  /* Constructor */
  GeneralHardness();

  /* Measure General Hardness (GH in dGH) */
  void measurement(float ec);

  /* Get General Hardness (GH in dGH) */
  float getGh() const;


  private:

  /* General Hardness (GH in dGH) */
  float gh;

};