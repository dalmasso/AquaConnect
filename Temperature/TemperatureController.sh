#!/bin/bash

#################################
# TEMPERATURE CONFIG & COMMANDS #
#################################
TEMPERATURE_IP=$1
TEMPERATURE_CMD=$2
TEMPERATURE_ARG=$3

TEMPERATURE_DATABASE="AquaTemperature"
TEMPERATURE_TEMP_TABLE="Temperature"
TEMPERATURE_SPAN_TABLE="Span"

GET_TEMPERATURE="$TEMPERATURE_IP/GET_TEMP"
GET_TEMPERATURE_SPAN="$TEMPERATURE_IP/GET_SPAN"
SET_TEMPERATURE_SPAN="$TEMPERATURE_IP/SET_SPAN?Span="


####################################
# DISPLAY TEMPERATURE SCRIPT USAGE #
####################################
usage() {
  echo "Temperature Controller Usage:"
  echo -e "\tGet Temperature:\tTemperatureController.sh <IP_ADDR> TEMP"
  echo -e "\tGet Temperature Span:\tTemperatureController.sh <IP_ADDR> GET_SPAN"
  echo -e "\tUpdate Temperature Span:\tTemperatureController.sh <IP_ADDR> SET_SPAN <Minutes>"
  echo -e "\tClear Temperature Database:\tTemperatureController.sh CLEAR"
  exit 1
}


###################
# GET TEMPERATURE #
###################
getTemperature(){

  # Send Request to Temperature Sensor
  response=$(curl -s -w "%{http_code}" $GET_TEMPERATURE)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Store received Temperature value
  if [[ http_code -eq 200 ]]; then
    databaseInsertTemperature "$content"
  fi
}


########################
# GET TEMPERATURE SPAN #
########################
getTemperatureSpan(){

  # Send Request to Temperature Sensor
  response=$(curl -s -w "%{http_code}" $GET_TEMPERATURE_SPAN)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Store received Temperature value
  if [[ http_code -eq 200 ]]; then
    databaseUpdateTemperatureSpan "$content"
  fi
}


########################
# SET TEMPERATURE SPAN #
########################
setTemperatureSpan(){

  # New Temperature Span
  newTempSpanMinutes=$1

  # Send Request to Temperature Sensor
  response=$(curl -s -w "%{http_code}" $SET_TEMPERATURE_SPAN+$newTempSpanMinutes)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Store received Temperature value
  if [[ http_code -eq 200 ]]; then
    databaseUpdateTemperatureSpan "$content"
  fi
}


##################
# SETUP DATABASE #
##################
databaseSetup() {
  mysql -u grafanaReader -e "CREATE DATABASE IF NOT EXISTS $TEMPERATURE_DATABASE;"
  mysql -u grafanaReader -D $TEMPERATURE_DATABASE -e "CREATE TABLE IF NOT EXISTS $TEMPERATURE_TEMP_TABLE (TIME VARCHAR(32), TEMPERATURE DECIMAL(3,1));"
  mysql -u grafanaReader -D $TEMPERATURE_DATABASE -e "CREATE TABLE IF NOT EXISTS $TEMPERATURE_SPAN_TABLE (SPAN INTEGER);"
}


#####################################
# DATABASE INSERT TEMPERATURE VALUE #
#####################################
databaseInsertTemperature(){
  date=$(date "+%x %X")
  mysql -u grafanaReader -D $TEMPERATURE_DATABASE -e "INSERT INTO $TEMPERATURE_TEMP_TABLE (TIME, TEMPERATURE) VALUES ('$date', '$1');"
}


####################################
# DATABASE UPDATE TEMPERATURE SPAN #
####################################
databaseUpdateTemperatureSpan(){
  mysql -u grafanaReader -D $TEMPERATURE_DATABASE -e "UPDATE $TEMPERATURE_SPAN_TABLE SET SPAN='$1';"
}


##################
# CLEAR DATABASE #
##################
databaseClear() {
  mysql -u grafanaReader -D TEMPERATURE_DATABASE -e "DELETE FROM $TEMPERATURE_TEMP_TABLE;"
  mysql -u grafanaReader -D TEMPERATURE_DATABASE -e "DELETE FROM $TEMPERATURE_SPAN_TABLE;"
}


################
# MAIN PROGRAM #
################

# Setup Database
databaseSetup

case "$TEMPERATURE_CMD" in

  TEMP)
    getTemperature
    ;;

  GET_SPAN)
    getTemperatureSpan
    ;;

  SET_SPAN)
    setTemperatureSpan "$TEMPERATURE_ARG"
    ;;

  CLEAR)
    databaseClear
    ;;

  *)
    usage
    ;;
esac