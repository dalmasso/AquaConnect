/*
 * ... Conductivity Program: Electrical Conductivity (EC, in µS/cm) & Total Dissolved Solids (TDS, in ppm)
 * ... PINOUT:
 * 
 * Reference: https://wiki.keyestudio.com/KS0429_keyestudio_TDS_Meter_V1.0
 */


/*****************************
 * ... CONFIGURATIONS *
 *****************************/

/* KS0429 Conductivity Sensor GPIO */
#define KS0429_GPIO                                   A1

/* KS0429 Measure Sample Point */
#define KS0429_SAMPLE_POINTS                           30

/* KS0429 ADC Voltage Conversion */
#define KS0429_ADC_VREF                               5.0
#define KS0429_ADC_RESOLUTION_BITS                    10
#define KS0429_VOLTAGE_CONVERSION(_voltage_)          _voltage_ * ((float) KS0429_ADC_VREF / (1 << KS0429_ADC_RESOLUTION_BITS))

/* KS0429 Temperature Compensation */
#define KS0429_TEMPERATURE_COMP(_voltage_, _temp_)    _voltage_ / ((float) 1.0 + 0.02 * (_temp_ - 25.0))

/* KS0429 TDS to EC Conversion (in µS/cm) */
#define KS0429_EC_CONVERSION(_voltage_)               (133.42*_voltage_*_voltage_*_voltage_ - 255.86*_voltage_*_voltage_ + 857.39*_voltage_)

/* KS0429 Voltage to TDS Conversion (in ppm) */
#define KS0429_TDS_CONVERSION(_ec_)                   _ec_ * 0.5

/* Default Conductivity Span (in minutes) */
#define CONDUCTIVITY_SPAN_DEFAULT                     15

/* Time Conversion */
#define SECS_PER_MIN                                  (60UL)
#define ELAPSED_MINUTES(_time_)                       ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)


/*******************************
 * ... GLOBAL VARIABLES *
 *******************************/

/* Conductivity Span (in minutes) */
unsigned int tdsSpan = CONDUCTIVITY_SPAN_DEFAULT;

/* Previous Conductivity read */
unsigned int previousReadTime = 0;


/*********************
 * KS0429 CONTROLLER *
 *********************/

/*
 * Read KS0429 current raw value and store it into buffer
 */
void ks0429Read(int ks0429BufferValues[]) {

  /* Read the KS0429 Sensor */
  for(int i=0; i<KS0429_SAMPLE_POINTS; i++) {

      ks0429BufferValues[i] = analogRead(KS0429_GPIO);
      delay(40);
  }
}

/*
 * Get KS049 value through Median Filter Algorithm (for value stability)
 */
int ks0429MedianFilter(int ks0429BufferValues[]) {

  int ks0429BufferTemp[KS0429_SAMPLE_POINTS];
  int ks0429ValueTemp;

  /* Copy KS0429 Buffer Values */
  for (byte i = 0; i<KS0429_SAMPLE_POINTS; i++)
      ks0429BufferTemp[i] = ks0429BufferValues[i];
  
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

/*
 * Get Conductivity (EC in µS/cm & TDS in ppm)
 */
void getConductivity(float* ec, float* tds) {

  /* Get Temperature (from other Sensor node) */
  float temperature;
  temperature = 25.0;//getTemperature();

  /* Read KS0429 Sensor */
  int ks0429Buffer[KS0429_SAMPLE_POINTS];
  ks0429Read(ks0429Buffer);

  /* Apply Median Filter Algorithm */
  int raw;
  raw = ks0429MedianFilter(ks0429Buffer);

  /* Convert Raw value to Voltage value */
  float voltage;
  voltage = KS0429_VOLTAGE_CONVERSION(raw);

  /* Compute Voltage value with Temperature compensation */
  float voltageTempCompens;
  voltageTempCompens = KS0429_TEMPERATURE_COMP(voltage, temperature);

  /* Convert Voltage value to EC value */
  *ec = KS0429_EC_CONVERSION(voltageTempCompens);

  /* Convert EC value to TDS value */
  *tds = KS0429_TDS_CONVERSION(*ec);
}

/* 
 * Conductivity Controller
 */
void conductivityController() {

  /* Get current Time in minutes */
  unsigned int currentTimeMin = ELAPSED_MINUTES(millis() / 1000);

  /* Check Conductivity Span */
  if ( (currentTimeMin - previousReadTime) >= tdsSpan) {

    /* Reset Previous Conductivity Read */
    previousReadTime = currentTimeMin;

    /* Read EC & TDS values */
    float ec;
    float tds;
    getConductivity(&ec, &tds);

    /* Send TDS and EC values */
    //clientSendConductivity(tds, ec);
  }
}


/********************
 * ... SETUP *
 ********************/

void setup() {

  /* KS0429 Conductivity Sensor GPIO Input Mode */
	pinMode(KS0429_GPIO, INPUT);

	Serial.begin(115200);
}


/***************************
 * ... MAIN PROGRAM *
 ***************************/

void loop() {
  
  /* Conductivity Controller */
  //conductivityController();
  
  float ec;
  float tds;
  getConductivity(&ec, &tds);
  Serial.println(ec);
  Serial.println(tds);
  Serial.println("---");
  /* Sampling 0.5 second */
  delay(500);
}