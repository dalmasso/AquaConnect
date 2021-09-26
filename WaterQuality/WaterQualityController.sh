#!/bin/bash

##############################################
# WATER QUALITY CONTROLLER CONFIG & COMMANDS #
##############################################

WATER_QUALITY_IP=$1
WATER_QUALITY_CMD=$2
WATER_QUALITY_ARG=$3

MANUAL="$WATER_QUALITY_IP/MANUAL"
GET_SPAN="$WATER_QUALITY_IP/GET_SPAN"
SET_SPAN="$WATER_QUALITY_IP/SET_SPAN?Span="
FIRMWARE_UPDATE="$WATER_QUALITY_IP/UPDATE"


##########################################
# DISPLAY WATER QUALITY CONTROLLER USAGE #
##########################################
usage(){
  echo "Water Quality Controller Usage:"
  echo -e "\tManual Mode:\t\tWaterQualityController.sh <IP_ADDR> MANUAL"
  echo -e "\tGet Span:\t\tWaterQualityController.sh <IP_ADDR> GET"
  echo -e "\tSet New Span:\t\tWaterQualityController.sh <IP_ADDR> SET <SPAN_MINUTE>"
  echo -e "\tFirmware Update:\tWaterQualityController.sh <IP_ADDR> UPDATE <BINARY_FILE>"
  exit 1
}


########################
# MANUAL WATER QUALITY #
########################
manualWaterQuality(){

  # Send Request to Water Quality Module
  response=$(curl -s -w "%{http_code}" $MANUAL)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Display Water Quality Manual Operation Result
  echo -e "$http_code: $content"
}


##########################
# GET WATER QUALITY SPAN #
##########################
getWaterQualitySpan(){

  # Send Request to Water Quality Module
  response=$(curl -s -w "%{http_code}" $GET_SPAN)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Display Water Quality Get Operation Result
  echo -e "$http_code: $content"
}


##########################
# SET WATER QUALITY SPAN #
##########################
setWaterQualitySpan(){

  # Check Arguments
  if [ -z "$WATER_QUALITY_ARG" ]; then
    usage
  fi

  # Send Request to Water Quality Module
  response=$(curl -s -w "%{http_code}" $SET_SPAN+$WATER_QUALITY_ARG)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Display Water Quality Set Operation Result
  echo -e "$http_code: $content"
}


#################################
# WATER QUALITY FIRMWARE UPDATE #
#################################
firmwareUpdateWaterQuality(){

  # Check Arguments
  if [ -z "$WATER_QUALITY_ARG" ]; then
    usage
  fi

  # Send Request to Water Quality Module
  response=$(curl -X POST -s -w "%{http_code}" -d $WATER_QUALITY_ARG $FIRMWARE_UPDATE)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Display Water Quality Firmware Update Operation Result
  echo -e "$http_code: $content"
}


################
# MAIN PROGRAM #
################

case "$WATER_QUALITY_CMD" in

  MANUAL)
    manualWaterQuality
    ;;

  GET)
    getWaterQualitySpan
    ;;

  SET)
    setWaterQualitySpan
    ;;

  UPDATE)
    firmwareUpdateWaterQuality
    ;;

  *)
    usage
    ;;
esac