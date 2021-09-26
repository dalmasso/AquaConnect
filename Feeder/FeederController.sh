#!/bin/bash

#######################################
# FEEDER CONTROLLER CONFIG & COMMANDS #
#######################################

FEEDER_IP=$1
FEEDER_CMD=$2
FEEDER_ARG_FIRMWARE=$3
FEEDER_ARG_TIME=$3
FEEDER_ARG_DAY=$4

MANUAL="$FEEDER_IP/MANUAL"
GET_FEEDERTIMES_LIST="$FEEDER_IP/GET"
NEW_FEEDERTIME="$FEEDER_IP/SET"
DELETE_FEEDERTIME="$FEEDER_IP/DELETE?Time="
FIRMWARE_UPDATE="$FEEDER_IP/UPDATE"
NEW_FEEDERTIME_ARG_TIME="?Time="
NEW_FEEDERTIME_ARG_DAYS="&Days="

SUNDAY="Sun"
MONDAY="Mon"
TUESDAY="Tue"
WEDNESDAY="Wed"
THURSDAY="Thu"
FRIDAY="Fri"
SATURDAY="Sat"

FEEDER_DATABASE="AquaFeeder"
FEEDER_PROGRAM_TABLE="FeederProgram"


###################################
# DISPLAY FEEDER CONTROLLER USAGE #
###################################
usage(){
  echo "Feeder Controller Usage:"
  echo -e "\tManual Mode:\t\tFeederController.sh <IP_ADDR> MANUAL"
  echo -e "\tGet FeederTimes List:\tFeederController.sh <IP_ADDR> LIST"
  echo -e "\tAdd New FeederTime:\tFeederController.sh <IP_ADDR> ADD <HH:MM> '<Sun, Mon, Tue, Wed, Thu, Fri, Sat>'"
  echo -e "\t\t\t\tDay=0: Disable"
  echo -e "\t\t\t\tDay=1: Enable"
  echo -e "\tDelete FeederTime:\tFeederController.sh <IP_ADDR> DELETE <HH:MM>" 
  echo -e "\tFirmware Update:\tFeederController.sh <IP_ADDR> UPDATE <BINARY_FILE>"
  exit 1
}


#################
# MANUAL FEEDER #
#################
manualFeeder(){

  # Send Request to Feeder Module
  response=$(curl -s -w "%{http_code}" $MANUAL)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Display Feeder Manual Operation Result
  echo -e "$http_code: $content"
}


#########################
# GET FEEDER TIMES LIST #
#########################
getFeederTimesList(){

  # Send Request to Feeder Module
  response=$(curl -s -w "%{http_code}" $GET_FEEDERTIMES_LIST)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Parse received FeederTimesList
  if [[ http_code -eq 200 ]]; then
    parseFeederTimeList "$content"
  fi
}


###################
# NEW FEEDER TIME #
###################
addFeederTime(){

  # Check Arguments
  if [ -z "$FEEDER_ARG_TIME" ] || [ -z "$FEEDER_ARG_DAY" ]; then
    usage
  fi

  # Convert HH:MM to Minute only format
  feederTime=$(( 10#$(echo "$FEEDER_ARG_TIME" | cut -f1 -d ':') * 60 + $(echo "$FEEDER_ARG_TIME" | cut -f2 -d ':') ))

  # Parse Days list
  feederDays="("

  # Sunday
  if [[ $FEEDER_ARG_DAY == *"$SUNDAY"* ]]; then
    feederDays=$feederDays"1,"
  else
    feederDays=$feederDays"0,"
  fi

  # Monday
  if [[ $FEEDER_ARG_DAY == *"$MONDAY"* ]]; then
    feederDays=$feederDays"1,"
  else
    feederDays=$feederDays"0,"
  fi

  # Tuesday
  if [[ $FEEDER_ARG_DAY == *"$TUESDAY"* ]]; then
    feederDays=$feederDays"1,"
  else
    feederDays=$feederDays"0,"
  fi

  # Wednesday
  if [[ $FEEDER_ARG_DAY == *"$WEDNESDAY"* ]]; then
    feederDays=$feederDays"1,"
  else
    feederDays=$feederDays"0,"
  fi

  # Thursday
  if [[ $FEEDER_ARG_DAY == *"$THURSDAY"* ]]; then
    feederDays=$feederDays"1,"
  else
    feederDays=$feederDays"0,"
  fi

  # Friday
  if [[ $FEEDER_ARG_DAY == *"$FRIDAY"* ]]; then
    feederDays=$feederDays"1,"
  else
    feederDays=$feederDays"0,"
  fi

  # Saturday
  if [[ $FEEDER_ARG_DAY == *"$SATURDAY"* ]]; then
    feederDays=$feederDays"1)"
  else
    feederDays=$feederDays"0)"
  fi

  # Send Request to Feeder Module
  response=$(curl -s -w "%{http_code}" $NEW_FEEDERTIME+$NEW_FEEDERTIME_ARG_TIME+$feederTime+$NEW_FEEDERTIME_ARG_DAYS+$feederDays)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Parse received FeederTimesList
  if [[ http_code -eq 200 ]]; then
    parseFeederTimeList "$content"
  fi
}


######################
# DELETE FEEDER TIME #
######################
deleteFeederTime(){

  # Check Argument
  if [ -z "$FEEDER_ARG_TIME" ]; then
    exit 1
  fi

  # Convert HH:MM to Minute only format
  feederTime=$(( 10#$(echo "$FEEDER_ARG_TIME" | cut -f1 -d ':') * 60 + $(echo "$FEEDER_ARG_TIME" | cut -f2 -d ':') ))

  # Send Request to Feeder Module
  response=$(curl -s -w "%{http_code}" $DELETE_FEEDERTIME+$feederTime)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Parse received FeederTimesList
  if [[ http_code -eq 200 ]]; then
    parseFeederTimeList "$content"
  fi
}


##########################
# FEEDER FIRMWARE UPDATE #
##########################
firmwareUpdateFeeder(){

  # Check Arguments
  if [ -z "$FEEDER_ARG_FIRMWARE" ]; then
    usage
  fi

  # Send Request to Water Quality Module
  response=$(curl -X -s -w "%{http_code}" -d $FEEDER_ARG_FIRMWARE $FIRMWARE_UPDATE)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Display Feeder Firmware Update Operation Result
  echo -e "$http_code: $content"
}


##########################
# PARSE FEEDER TIME LIST #
##########################
parseFeederTimeList(){

  # Clear Database
  databaseClear

  # Parse received FeederTimesList
  feederTimesList=($1)
  for i in "${feederTimesList[@]}"
  do
    # Parse FeederTime
    feederTime=$(echo "$i" | cut -f1 -d ':')

    # FeederTime is set
    if [[ "$feederTime" =~ ^[0-9]+$ ]]; then
      printf -v formatedFeederTime "%02d:%02d" $(( $feederTime / 60)) $(( $feederTime % 60))
    else
      formatedFeederTime=$feederTime
    fi

    # Parse Status
    status=$(echo "$i" | cut -f2 -d ':')

    # Parse Days & Remove "()"
    days=$(echo "$i" | cut -f3 -d ':')
    days=$(echo -e "${days//'('/}")
    days=$(echo -e "${days//')'/}")

    # Store in Database
    databaseUpdateFeederProgram $formatedFeederTime $status $days
  done
}


##################
# SETUP DATABASE #
##################
databaseSetup() {
  mysql -u grafanaReader -e "CREATE DATABASE IF NOT EXISTS $FEEDER_DATABASE;"
  mysql -u grafanaReader -D $FEEDER_DATABASE -e "CREATE TABLE IF NOT EXISTS $FEEDER_PROGRAM_TABLE (TIME TIME, STATUS VARCHAR(32), DAYS VARCHAR(33));"
}


###################
# UPDATE DATABASE #
###################
databaseUpdateFeederProgram() {
  mysql -u grafanaReader -D $FEEDER_DATABASE -e "INSERT INTO $FEEDER_PROGRAM_TABLE (TIME, STATUS, DAYS) VALUES ('$1', '$2', '$3');"
}


##################
# CLEAR DATABASE #
##################
databaseClear() {
  mysql -u grafanaReader -D $FEEDER_DATABASE -e "DELETE FROM $FEEDER_PROGRAM_TABLE;"
}


###################
# REMOVE DATABASE #
###################
databaseRemove() {
  mysql -u grafanaReader -D $FEEDER_DATABASE -e "DROP TABLE $FEEDER_PROGRAM_TABLE;"
  mysql -u grafanaReader -e "DROP DATABASE $FEEDER_DATABASE;"
}


################
# MAIN PROGRAM #
################

# Setup Database
databaseSetup

case "$FEEDER_CMD" in

  MANUAL)
    manualFeeder
    ;;

  LIST)
    getFeederTimesList
    ;;

  ADD)
    addFeederTime
    ;;

  DELETE)
    deleteFeederTime
    ;;

  UPDATE)
    firmwareUpdateFeeder
    ;;

  *)
    usage
    ;;
esac