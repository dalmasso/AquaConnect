#!/bin/bash

##################################################
# Credits to softScheck GmbH                     #
# https://github.com/softScheck/tplink-smartplug #
##################################################


############################
# FILTER CONFIG & COMMANDS #
############################
FILTER_IP=$1
FILTER_CMD=$2


###################################
# DISPLAY FILTER CONTROLLER USAGE #
###################################
usage(){
  
  # Filter Controller is a KP105 Wrapper Controller
  KP105Usage=$(KP105Plug/KP105PlugController.sh)

  # Replace "KP105" to "Filter"
  Parsing=$(echo -e "${KP105Usage//KP105/Filter}")
  echo -e "$Parsing"
  exit 1
}


###################
# KP105 OPERATION #
###################
KP105Operation(){

  # Execute KP105 Operation
  result=$(KP105Plug/KP105PlugController.sh "$FILTER_IP" "$FILTER_CMD")

  # State Operation
  if [ "$FILTER_CMD" == "STATE" ]; then
    echo -e "$result"
  fi
}


################
# MAIN PROGRAM #
################

case "$FILTER_CMD" in

  ON | OFF | STATE)
    KP105Operation
    ;;

  *)
    usage
    ;;
esac