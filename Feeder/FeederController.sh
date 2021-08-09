#!/bin/bash

################################
# FEEDER CONFIG & COMMANDS #
################################
FEEDER_CMD=$1
FEEDER_PARAM=$2

FEEDER_IP="192.168.1.35"
GET_FEEDERTIMES_LIST="$FEEDER_IP/GET"
NEW_FEEDERTIME="$FEEDER_IP/SET?Time="
DELETE_FEEDERTIME="$FEEDER_IP/DELETE?Time="
MANUAL="$FEEDER_IP/MANUAL"


###################################
# DISPLAY FEEDER CONTROLLER USAGE #
###################################
usage(){
  echo "Feeder Controller Usage:"
  echo -e "\tGet FeederTimes List:\tFeederController.sh LIST"
  echo -e "\tAdd New FeederTime:\tFeederController.sh ADD <HH:MM>"
  echo -e "\tDelete FeederTime:\tFeederController.sh DELETE <HH:MM>"
  echo -e "\tManual Mode:\t\tFeederController.sh MANUAL"
  exit 1
}


#########################
# GET FEEDER TIMES LIST #
#########################
getFeederTimesList(){

  # Send Request
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
newFeederTime(){

  # Check Argument
  if [ -z "$1" ]; then
    exit 1
  fi

  # Convert HH:MM to Minute only format
  feederTime=$(( 10#$(echo "$1" | cut -f1 -d ':') * 60 + $(echo "$1" | cut -f2 -d ':') ))

  # Send Request
  response=$(curl -s -w "%{http_code}" $NEW_FEEDERTIME+$feederTime)

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
  if [ -z "$1" ]; then
    exit 1
  fi

  # Convert HH:MM to Minute only format
  feederTime=$(( 10#$(echo "$1" | cut -f1 -d ':') * 60 + $(echo "$1" | cut -f2 -d ':') ))

  # Send Request
  response=$(curl -s -w "%{http_code}" $DELETE_FEEDERTIME+$feederTime)

  # Parse HTTP Code & Content
  http_code=$(tail -n1 <<< "$response")
  content=$(sed '$ d' <<< "$response")

  # Parse received FeederTimesList
  if [[ http_code -eq 200 ]]; then
    parseFeederTimeList "$content"
  fi
}


#################
# MANUAL FEEDER #
#################
manualFeeder(){
  curl $MANUAL
}


##########################
# PARSE FEEDER TIME LIST #
##########################
parseFeederTimeList(){

  # Reset Database
  resetDatabase

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

    # Store in Database
    updateDatabase $formatedFeederTime $status
  done
}


##################
# SETUP DATABASE #
##################
setupDatabase() {
  mysql -u grafanaReader -e "CREATE DATABASE IF NOT EXISTS Feeder;"
  mysql -u grafanaReader -D Feeder -e "CREATE TABLE IF NOT EXISTS FeederProgram (TIME VARCHAR(32), STATUS VARCHAR(32));"
}


##################
# RESET DATABASE #
##################
resetDatabase() {
  mysql -u grafanaReader -D Feeder -e "DELETE FROM FeederProgram;"
}


###################
# UPDATE DATABASE #
###################
updateDatabase() {
  feederTime=$1
  status=$2

  # Store in Database
  mysql -u grafanaReader -D Feeder -e "INSERT INTO FeederProgram (TIME, STATUS) VALUES ('$feederTime', '$status');"
}


################
# MAIN PROGRAM #
################

# Setup Database
setupDatabase

case "$FEEDER_CMD" in

  LIST)
    getFeederTimesList
    ;;

  ADD)
    newFeederTime "$FEEDER_PARAM"
    ;;

  DELETE)
    deleteFeederTime "$FEEDER_PARAM"
    ;;

  MANUAL)
    manual
    ;;

  *)
    usage
    ;;
esac