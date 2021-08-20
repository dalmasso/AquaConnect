/*
 * Carbon Dioxide (CO2) Measurement Approximation
 */


/*********************************
 * CARBON DIOXIDE CONFIGURATIONS *
 *********************************/

/* Approximatives KH and pH coefficients */
#define CO2_KH_COEFF              15.6896
#define CO2_PH_COEFF              6.31

/* Soil Compensation */
#define CO2_SOIL_COMPENSATION     0 //TODO


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