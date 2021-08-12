#!/bin/bash

##################################################
# Credits to softScheck GmbH                     #
# https://github.com/softScheck/tplink-smartplug #
##################################################


######################################
# CIRCULATION PUMP CONFIG & COMMANDS #
######################################
PUMP_IP=$1
PUMP_CMD=$2
PUMP_ARG=$3

PUMP_DATABASE="AquaCirculationPump"
PUMP_PROG_TABLE="CirculationPumpProgram"


#############################################
# DISPLAY CIRCULATION PUMP CONTROLLER USAGE #
#############################################
usage(){
  
  # Circulation Pump Controller is a KP105 Wrapper Controller
  KP105Usage=$(../KP105Plug/KP105PlugController.sh)

  # Replace "KP105" to "Circulation Pump", remove STATE operation
  Parsing=$(echo -e "${KP105Usage//KP105/Circulation Pump}")
  Parsing=$(echo -e "${Parsing//'/STATE'/}")
  exit 1
}


###################
# KP105 OPERATION #
###################
KP105Operation(){

  # Execute KP105 Operation
  result=$(../KP105Plug/KP105PlugController.sh "$PUMP_IP" "$PUMP_CMD" "$PUMP_ARG")

  # Schedule Mode Operation
  if [ "$PUMP_CMD" != "ON" ] && [ "$PUMP_CMD" != "OFF" ]; then

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
  mysql -u grafanaReader -e "CREATE DATABASE IF NOT EXISTS $PUMP_DATABASE;"
  mysql -u grafanaReader -D $PUMP_DATABASE -e "CREATE TABLE IF NOT EXISTS $PUMP_PROG_TABLE (ID VARCHAR(64) UNIQUE, ACTION VARCHAR(32), TIME TIME);"
}


############################################
# DATABASE INSERT CIRCULATION PUMP PROGRAM #
############################################
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
    mysql -u grafanaReader -D $PUMP_DATABASE -e "INSERT INTO $PUMP_PROG_TABLE (ID, ACTION, TIME) VALUES ('$ruleId', '$action', '$time');"
  done < <(echo "$1")
}


##################
# CLEAR DATABASE #
##################
databaseClear() {
  mysql -u grafanaReader -D $PUMP_DATABASE -e "DELETE FROM $PUMP_PROG_TABLE;"
}


###################
# REMOVE DATABASE #
###################
databaseRemove() {
  mysql -u grafanaReader -D $PUMP_DATABASE -e "DROP TABLE $PUMP_PROG_TABLE;"
  mysql -u grafanaReader -e "DROP DATABASE $PUMP_DATABASE;"
}


################
# MAIN PROGRAM #
################

# Setup Database
databaseSetup

case "$PUMP_CMD" in

  ON | OFF | LIST | RESET | DELETE | ADD_ON | ADD_OFF)
    KP105Operation
    ;;

  *)
    usage
    ;;
esac