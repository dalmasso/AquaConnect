/*
 * ESP8266-01 Feeder Program
 * ESP8266-01 PINOUT:
 * -------------------------------------------
 * | TX/GPIO1/LED * * GND           |   -----|
 * | CH_PD/EN     * * GPIO2         |   |-----
 * | RST          * * GPIO0/FLASH   |   -----|
 * | VCC(3.3)     * * RX/GPIO3      |        |
 * -------------------------------------------
 * 
 * Note: Set FLASH pin to ground for programming
 * 
 * AI-Thinker Firmware: https://wiki.aprbrother.com/en/Firmware_For_ESP8266.html#v0-9-5-2
 * Flasher: https://github.com/nodemcu/nodemcu-flasher
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


/*****************************
 * ESP8266-01 CONFIGURATIONS *
 *****************************/

/* Feeder Times (3x configuration times, format: daily timestamp from 0 to 1439 min) */
#define FEEDER_TIME_LIST          3
#define FEEDER_STATUS_WAIT        0
#define FEEDER_STATUS_DONE        1
#define FEEDER_STATUS_UNSET       2

/* Feeder Execution Time: 9.5 seconds (in ticks: 9 500 000 us / 3.2) */
#define FEEDER_EXEC_TIME          2968750

/* Feeder Relay State */
#define FEEDER_OFF                 0
#define FEEDER_STOP                1
#define FEEDER_ON                  2

/* Feeder Times operation reset each new day (between 00:00:00 and 00:00:05)*/
#define NEW_DAY_MIN_TRIGGER       0
#define NEW_DAY_SEC_TRIGGER       5

/* Feeder Times format: "FeederTime:status\n\0" */
#define FEEDER_TIME_STR_LENGTH    15
#define FEEDER_STATUS_STR_WAIT    "Waiting"
#define FEEDER_STATUS_STR_DONE    "Done"
#define FEEDER_STATUS_STR_UNSET   "Unset:Unset\n"

/* Server API */
#define FEEDER_API_MANUAL         "/MANUAL"
#define FEEDER_API_GET            "/GET"
#define FEEDER_API_SET            "/SET"
#define FEEDER_API_DELETE         "/DELETE"

/* WiFi */
#define WIFI_NETWORK              "SFR_6B70"
#define WIFI_PASSWORD             "xja49ukqz4zyz83umqq8"


/*******************************
 * ESP8266-01 GLOBAL VARIABLES *
 *******************************/

/* Feeder Times List: Feeder Time - Status */
unsigned int feederTimesList[FEEDER_TIME_LIST][2];

/* Feeder Relay State Controller */
unsigned int feederRelayState = FEEDER_OFF;
const byte relayON[] = {0xA0, 0x01, 0x01, 0xA2};
const byte relayOFF[] = {0xA0, 0x01, 0x00, 0xA1};

/* NTP Client Time (UTC+02:00) */
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", 7200);

/* Server Object, available on port 80 */
ESP8266WebServer server(80);


/********************************
 * ESP8266-01 SERVER CONTROLLER *
 ********************************/

/*
 * Start the Feeder manually
 * API Return: 200
 */
void serverManualFeeder() {
  
  /* Start Feeder */
  feederStart();
  
  /* Server Response */
  server.send(200, "text/plain", "Feeder Started\n");
}

/*
 * Get current Feeder Times List
 * API Return: 200, with list of Feeder Times
 */
void serverGetFeederTime() {

  /* Format Feeder Times List: "FeederTime:status\n" */
  char formatedfeederTimesList[FEEDER_TIME_LIST * FEEDER_TIME_STR_LENGTH + FEEDER_TIME_LIST] = {0};
  for (int i = 0; i < FEEDER_TIME_LIST; i++) {

    /* Format FeederTime Status */
    if (feederTimesList[i][1] == FEEDER_STATUS_WAIT) {
      snprintf(formatedfeederTimesList + strlen(formatedfeederTimesList), FEEDER_TIME_STR_LENGTH, "%d:%s\n", feederTimesList[i][0], FEEDER_STATUS_STR_WAIT);

    } else if (feederTimesList[i][1] == FEEDER_STATUS_DONE) {
      snprintf(formatedfeederTimesList + strlen(formatedfeederTimesList), FEEDER_TIME_STR_LENGTH, "%d:%s\n", feederTimesList[i][0], FEEDER_STATUS_STR_DONE);
    
    } else {
      strncpy(formatedfeederTimesList + strlen(formatedfeederTimesList), FEEDER_STATUS_STR_UNSET, FEEDER_TIME_STR_LENGTH);
    }
  }

  /* Send formated Feeder Times List */
  server.send(200, "text/plain", formatedfeederTimesList);
}

/*
 * Set new Feeder Time
 * API Return: 200, with the updated Feeder Times List
 * API Return: 400 if no new Feeder Time is provided
 * API Return: 403 if the Feeder Times is full
 */
void serverSetFeederTime() {

  /* Check new Feeder Time argument */
  if (!server.hasArg("Time")) {
    server.send(400, "text/plain", "No new Feeder Time to set\n");

  } else {
    /* Convert String Time to unsigned int */
    unsigned int newFeederTime = server.arg("Time").toInt();

    /* Verify Duplicate */
    bool duplicate = false;
    for (int i = 0; i < FEEDER_TIME_LIST; i++) {
      if (feederTimesList[i][0] == newFeederTime) {
        duplicate = true;
        break;
      }
    }
    
   /* Check available Feeder Time slot AND not already present */
   int i = 0;
   for (i = 0; i < FEEDER_TIME_LIST && !duplicate; i++) {
      if (feederTimesList[i][1] == FEEDER_STATUS_UNSET) {
        feederTimesList[i][0] = newFeederTime;
        feederTimesList[i][1] = FEEDER_STATUS_WAIT;
        break;
      }
    }

    /* Server Response */
    if (i < FEEDER_TIME_LIST) {
      /* Server sends updated Feeder Times List */
      serverGetFeederTime();
    } else {
      server.send(403, "text/plain", "Feeder Times List is Full\n");
    }
  }
}

/*
 * Delete specific Feeder Time
 * API Return: 200, with the updated Feeder Times List
 * API Return: 400 if no Feeder Time is provided
 */
void serverDeleteFeederTime() {

  /* Check Feeder Time argument */
  if (!server.hasArg("Time")) {
    server.send(400, "text/plain", "No specified Feeder Time to delete\n");

  } else {
    /* Convert String Time to unsigned int */
    unsigned int timeToDelete = server.arg("Time").toInt();

    bool deleted = false;
    for (int i = 0; i < FEEDER_TIME_LIST; i++) {
      /* Reset Feeder Time & Status */
      if (feederTimesList[i][0] == timeToDelete) {
        feederTimesList[i][0] = 0;
        feederTimesList[i][1] = FEEDER_STATUS_UNSET;
        deleted = true;
      }
    }

    /* Server Response */
    if (deleted) {
      /* Server Response, send updated Feeder Times List */
      serverGetFeederTime();
    } else {
      server.send(403, "text/plain", "Unknown specified Feeder Times\n");
    }
  }
}

/*
 * Unsupported server operation
 * API Return: 501
 */
void serverUnsupportedOperation() {
  server.send(501, "text/plain", "Unsupported Operation\n");
}


/*********************
 * FEEDER CONTROLLER *
 *********************/

/*
 * Feeder Controller
 */
void feederController() {

  /* Actual Time */
  unsigned int currentTimeMin = ntpClient.getHours()*60 + ntpClient.getMinutes();
  unsigned int currentTimeSec = ntpClient.getSeconds();

  /* Parse the Feeder Times List */
  for (int i = 0; i < FEEDER_TIME_LIST; i++) {

    /* Reset Feeder Time status for the new day (00:00:00-06) */
    if ((currentTimeMin == NEW_DAY_MIN_TRIGGER) && (currentTimeSec <= NEW_DAY_SEC_TRIGGER)) {

      /* Only for set Feeder Times */
      if (feederTimesList[i][1] != FEEDER_STATUS_UNSET) {
        feederTimesList[i][1] = FEEDER_STATUS_WAIT;
      }

      /* Feeder Time */
    } else if ((currentTimeMin == feederTimesList[i][0]) && (feederTimesList[i][1] == FEEDER_STATUS_WAIT)) {

      /* Start Feeder */
      feederStart();
      
      /* Update Feeder Time Status */
      feederTimesList[i][1] = FEEDER_STATUS_DONE;
    }
  }

  /* Stop Feeder */
  feederStop();
}

/*
 * Start the Feeder
 */
 void feederStart() {

  /* Check Feeder Relay State */
  if (feederRelayState == FEEDER_OFF) {

    /* Load Feeder Timer */
    timer1_write(FEEDER_EXEC_TIME);

    /* Start Feeder */
    Serial.write(relayON, sizeof(relayON));
    feederRelayState = FEEDER_ON;
  }

}

/*
 * Detect the Feeder Ending process (ISR)
 */
void ICACHE_RAM_ATTR feederEndingTrigger() {
  if (feederRelayState == FEEDER_ON) {
    feederRelayState = FEEDER_STOP;
  }
}

/*
 * Stop the Feeder
 */
void feederStop() {
  
  /* Check Feeder Relay State */
  if (feederRelayState == FEEDER_STOP) {
    Serial.write(relayOFF, sizeof(relayOFF));
    feederRelayState = FEEDER_OFF;
  }
}

/********************
 * ESP8266-01 SETUP *
 ********************/

void setup()
{
  /* Init Serial Communication with Relay */
  Serial.begin(9600);

  /* Relay OFF */
  Serial.write(relayOFF, sizeof(relayOFF));

  /* Setup Feeder Timer Interrupt ISR */
  /* 312.5Khz (1 tick = 3.2us - 26843542.4 us max), on Edge, No loop */
  timer1_attachInterrupt(feederEndingTrigger);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);

  /* Clear feederTimesList (FeederTime & Status) */
  for (int i = 0; i < FEEDER_TIME_LIST; i++) {
    feederTimesList[i][0] = 0;
    feederTimesList[i][1] = FEEDER_STATUS_UNSET;
  }
 
  /* WiFi Configuration as Station mode */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

  /* Connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  /* Server Operation Configurations */
  server.on(FEEDER_API_MANUAL, serverManualFeeder);
  server.on(FEEDER_API_GET, serverGetFeederTime);
  server.on(FEEDER_API_SET, serverSetFeederTime);
  server.on(FEEDER_API_DELETE, serverDeleteFeederTime);
  server.onNotFound(serverUnsupportedOperation);

  /* Start Server */
  server.begin();

  /* Start NTP Client */
  ntpClient.begin();
}


/***************************
 * ESP8266-01 MAIN PROGRAM *
 ***************************/

void loop()
{
  /* Update Time */
  ntpClient.update();

  /* Server Controller */
  server.handleClient();

  /* Feeder Controller,*/
  feederController();

  /* Sampling 0.5 second */
  delay(500);
}