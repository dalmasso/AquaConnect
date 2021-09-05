/*
 * Firmware Update
 */

#include "FirmwareUpdate.h"


/****************************************
 * FIRMWARE UPDATE CLASS IMPLEMENTATION *
 ****************************************/

/*
 * Constructor
 */
FirmwareUpdate::FirmwareUpdate() : {}


/* 
 * Execute the Firmware Update 
 */
void FirmwareUpdate::update(const char* url) {

    HttpsOTA.begin(url, SERVER_CERTIFICATE); 
}


/*
 * Get the Firmware Update status
 */
float FirmwareUpdate::getStatus() const {
  return this->status;
}