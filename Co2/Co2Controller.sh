#!/bin/bash

##################################################
# Credits to softScheck GmbH                     #
# https://github.com/softScheck/tplink-smartplug #
##################################################


#########################
# CO2 CONFIG & COMMANDS #
#########################
CO2_IP=$1
CO2_CMD=$2
CO2_ARG=$3

CO2_DATABASE="AquaCo2"
CO2_PROG_TABLE="Co2gProgram"


################################
# DISPLAY CO2 CONTROLLER USAGE #
################################
usage(){
  
  # Co2 Controller is a KP105 Wrapper Controller
  KP105Usage=$(KP105Plug/KP105PlugController.sh)

  # Replace "KP105" to "CO2", remove STATE operation
  Parsing=$(echo -e "${KP105Usage//KP105/CO2}")
  Parsing=$(echo -e "${Parsing//'/STATE'/}")
  echo -e "$Parsing"
  exit 1
}


###################
# KP105 OPERATION #
###################
KP105Operation(){

  # Execute KP105 Operation
  result=$(KP105Plug/KP105PlugController.sh "$CO2_IP" "$CO2_CMD" "$CO2_ARG")

  # Schedule Mode Operation
  if [ "$CO2_CMD" != "ON" ] && [ "$CO2_CMD" != "OFF" ]; then

    # Clear Database
    databaseClear

    # Update Database
    databaseUpdateProgram $result
  fi
}


##################
# SETUP DATABASE #
##################
databaseSetup() {
  mysql -u grafanaReader -e "CREATE DATABASE IF NOT EXISTS $CO2_DATABASE;"
  mysql -u grafanaReader -D $CO2_DATABASE -e "CREATE TABLE IF NOT EXISTS $CO2_PROG_TABLE (ID VARCHAR(64) UNIQUE, ACTION VARCHAR(32), TIME TIME);"
}


###############################
# DATABASE INSERT CO2 PROGRAM #
###############################
databaseUpdateProgram() {

  # Parse each result line
  while read -r line;
  do
    # Parse variables
    ruleId=$(echo "$line" | cut -f1 -d ' ')
    action=$(echo "$line" | cut -f2 -d ' ')
    time=$(echo "$line" | cut -f3 -d ' ')

    # Format Action
    if [[ "$action" == "1" ]]; then
      action="ON"
    else
      action="OFF"
    fi

    # Store in Database
    mysql -u grafanaReader -D $CO2_DATABASE -e "INSERT INTO $CO2_PROG_TABLE (ID, ACTION, TIME) VALUES ('$ruleId', '$action', '$time');"
  done < <(echo "$1")
}


##################
# CLEAR DATABASE #
##################
databaseClear() {
  mysql -u grafanaReader -D $CO2_DATABASE -e "DELETE FROM $CO2_PROG_TABLE;"
}


###################
# REMOVE DATABASE #
###################
databaseRemove() {
  mysql -u grafanaReader -D $CO2_DATABASE -e "DROP TABLE $CO2_PROG_TABLE;"
  mysql -u grafanaReader -e "DROP DATABASE $CO2_DATABASE;"
}


################
# MAIN PROGRAM #
################

# Setup Database
databaseSetup

case "$CO2_CMD" in

  ON | OFF | LIST | RESET | DELETE | ADD_ON | ADD_OFF)
    KP105Operation
    ;;

  *)
    usage
    ;;
esac