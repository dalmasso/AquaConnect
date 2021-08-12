#!/bin/bash

##################################################
# Credits to softScheck GmbH                     #
# https://github.com/softScheck/tplink-smartplug #
##################################################


##############################
# LIGHTING CONFIG & COMMANDS #
##############################
LIGHTING_IP=$1
LIGHTING_CMD=$2
LIGHTING_ARG=$3

LIGHTING_DATABASE="AquaLight"
LIGHTING_PROG_TABLE="LightingProgram"


#####################################
# DISPLAY LIGHTING CONTROLLER USAGE #
#####################################
usage(){
  
  # Lighting Controller is a KP105 Wrapper Controller
  KP105Usage=$(../KP105Plug/KP105PlugController.sh)

  # Replace "KP105" to "Lighting", remove STATE operation
  Parsing=$(echo -e "${KP105Usage//KP105/Lighting}")
  Parsing=$(echo -e "${Parsing//'/STATE'/}")
  exit 1
}


###################
# KP105 OPERATION #
###################
KP105Operation(){

  # Execute KP105 Operation
  result=$(../KP105Plug/KP105PlugController.sh "$LIGHTING_IP" "$LIGHTING_CMD" "$LIGHTING_ARG")

  # Schedule Mode Operation
  if [ "$LIGHTING_CMD" != "ON" ] && [ "$LIGHTING_CMD" != "OFF" ]; then

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
  mysql -u grafanaReader -e "CREATE DATABASE IF NOT EXISTS $LIGHTING_DATABASE;"
  mysql -u grafanaReader -D $LIGHTING_DATABASE -e "CREATE TABLE IF NOT EXISTS $LIGHTING_PROG_TABLE (ID VARCHAR(64) UNIQUE, ACTION VARCHAR(32), TIME TIME);"
}


####################################
# DATABASE INSERT LIGHTING PROGRAM #
####################################
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
    mysql -u grafanaReader -D $LIGHTING_DATABASE -e "INSERT INTO $LIGHTING_PROG_TABLE (ID, ACTION, TIME) VALUES ('$ruleId', '$action', '$time');"
  done < <(echo "$1")
}


##################
# CLEAR DATABASE #
##################
databaseClear() {
  mysql -u grafanaReader -D $LIGHTING_DATABASE -e "DELETE FROM $LIGHTING_PROG_TABLE;"
}


###################
# REMOVE DATABASE #
###################
databaseRemove() {
  mysql -u grafanaReader -D $LIGHTING_DATABASE -e "DROP TABLE $LIGHTING_PROG_TABLE;"
  mysql -u grafanaReader -e "DROP DATABASE $LIGHTING_DATABASE;"
}


################
# MAIN PROGRAM #
################

# Setup Database
databaseSetup

case "$LIGHTING_CMD" in

  ON | OFF | LIST | RESET | DELETE | ADD_ON | ADD_OFF)
    KP105Operation
    ;;

  *)
    usage
    ;;
esac