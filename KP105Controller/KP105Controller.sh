#!/bin/bash

##################################################
# Credits to softScheck GmbH                     #
# https://github.com/softScheck/tplink-smartplug #
##################################################


###########################
# KP105 CONFIG & COMMANDS #
###########################
KP105_IP=$1
KP105_CMD=$2
KP105_ARG=$3

KP105_PORT=9999
KP105_ON="{\"system\":{\"set_relay_state\":{\"state\":1}}}"
KP105_OFF="{\"system\":{\"set_relay_state\":{\"state\":0}}}"
KP105_INFO="{\"system\":{\"get_sysinfo\":{}}}"
KP105_SCHEDULE_RULES="{\"schedule\":{\"get_rules\":{}}}"
KP105_SCHEDULE_RESET="{\"schedule\":{\"delete_all_rules\":{}}}"
KP105_GET_TIMEZONE="{\"time\":{\"get_time\":{}}}"
KP105_SET_TIMEZONE="{\"time\":{\"set_timezone\":{\"year\":$(date +'%Y'),\
\"month\":$(date +'%m'),\"mday\":$(date +'%d'),\"hour\":$(date +'%H'),\
\"min\":$(date +'%M'),\"sec\":$(date +'%S'),\"index\":$(date +'%z')}}}"


#####################################
# DISPLAY LIGHTING CONTROLLER USAGE #
#####################################
usage(){
  echo "KP105 Controller Usage:"
  echo -e "\tBasic Mode:\tKP105Controller.sh <IP_ADDR> <ON/OFF/STATE>"
  echo -e "\tSchedule Mode:\tKP105Controller.sh <IP_ADDR> LIST"
  echo -e "\tSchedule Mode:\tKP105Controller.sh <IP_ADDR> RESET"
  echo -e "\tSchedule Mode:\tKP105Controller.sh <IP_ADDR> DELETE <SCHEDULE_ID>"
  echo -e "\tSchedule Mode:\tKP105Controller.sh <IP_ADDR> ADD_ON <HH:MM>"
  echo -e "\tSchedule Mode:\tKP105Controller.sh <IP_ADDR> ADD_OFF <HH:MM>"
  exit 1
}


#################
# TURN ON KP105 #
#################
on(){
  # Send Request to KP105
  encode "$KP105_ON" | nc -n $KP105_IP $KP105_PORT > /dev/null
}


##################
# TURN OFF KP105 #
##################
off(){
  # Send Request to KP105
  encode "$KP105_OFF" | nc -n $KP105_IP $KP105_PORT > /dev/null
}


###################
# GET KP105 STATE #
###################
getState(){

  # Send Request to KP105
  response=$(encode "$KP105_INFO" | nc -n $KP105_IP $KP105_PORT | tr -d '\0')
  decodedResponse=$(decode "$response" | grep -oa 'relay_state":[0,1]' | grep -o '[0,1]')

  if [[ $decodedResponse -eq 1 ]]; then
    echo "ON"
  elif [[ $decodedResponse -eq 0 ]]; then
    echo "OFF"
  else
    echo "Bad Response ($decodedResponse)"
  fi
}


############################
# GET KP105 SCHEDULE RULES #
############################
getScheduleRules(){
  
  # Send Request to KP105
  response=$(encode "$KP105_SCHEDULE_RULES" | nc -n $KP105_IP $KP105_PORT | tr -d '\0')
  decodedResponse=$(decode "$response")

  # Parse Response: Number of Schedule rules
  nbSchedule=$(echo "$decodedResponse" | jq '.schedule.get_rules.rule_list | length')
  result=""
  for ((i=0; i<$nbSchedule; i++))
  do
    # Parse rule: ID, Time, ON/OFF
    ruleId=$(echo "$decodedResponse" | jq ".schedule.get_rules.rule_list[$i].id" | sed 's/\"//g')
    ruleTime=$(echo "$decodedResponse" | jq ".schedule.get_rules.rule_list[$i].smin")
    ruleAction=$(echo "$decodedResponse" | jq ".schedule.get_rules.rule_list[$i].sact")
    
    # Format Time (HH:MM)
    printf -v formatedTime "%02d:%02d" $(( $ruleTime / 60)) $(( $ruleTime % 60))
    
    # Format result with RuleID, RuleAction and FormatedTim
    if [[ i -eq 0 ]]; then
      result=$result"$ruleId $ruleAction $formatedTime"
    else
      result=$result$'\n'"$ruleId $ruleAction $formatedTime"
    fi
  done

  # Return result
  echo -e "$result"
}


##############################
# RESET KP105 SCHEDULE RULES #
##############################
resetScheduleRules(){
  
  # Send Request to KP105
  encode "$KP105_SCHEDULE_RESET" | nc -n $KP105_IP $KP105_PORT > /dev/null

  # Recover Schedule rules
  getScheduleRules
}


########################################
# DELETE SPECIFIC KP105 SCHEDULE RULES #
########################################
deleteScheduleRule(){

  # Check Argument
  if [ -z "$KP105_ARG" ]; then
    usage
  fi

  # Specific Scheduled Rule ID
  request="{\"schedule\":{\"delete_rule\":{\"id\":\"$KP105_ARG\"}}}"
  encode "$request" | nc -n $KP105_IP $KP105_PORT > /dev/null

  # Recover Schedule rules
  getScheduleRules
}


####################################
# SET TURN ON KP105 SCHEDULE RULES #
####################################
setTurnOnSchedule(){

  # Check Argument
  if [ -z "$KP105_ARG" ]; then
    usage
  fi

  # Set Timezone
  verifyTimeZone=$(encode "$KP105_GET_TIMEZONE" | nc -n $KP105_IP $KP105_PORT | tr -d '\0')
  decodedResponse=$(decode "$verifyTimeZone" | grep -oa "time not sync")

  # Configure Timezone
  if [[ ! -z $decodedResponse ]]; then
    encode "$KP105_SET_TIMEZONE" | nc -n $KP105_IP $KP105_PORT > /dev/null
  fi

  # Scheduled Time: convert HH:MM to Minute only format
  scheduleTime=$(( 10#$(echo "$KP105_ARG" | cut -f1 -d ':') * 60 + $(echo "$KP105_ARG" | cut -f2 -d ':') ))

  #Add New Schedule Rule
  request="{\"schedule\":{\"add_rule\":{\"stime_opt\":0,\"etime_opt\":-1,\"enable\":1,\"repeat\":1,\
  \"sact\":1,\
  \"wday\":[1,1,1,1,1,1,1],\
  \"smin\":$scheduleTime,
  \"name\":\"LightsON\"}}}"

  encode "$request" | nc -n $KP105_IP $KP105_PORT > /dev/null

  # Recover Schedule rules
  getScheduleRules
}


#####################################
# SET TURN OFF KP105 SCHEDULE RULES #
#####################################
setTurnOffSchedule(){

  # Check Argument
  if [ -z "$KP105_ARG" ]; then
    usage
  fi

  # Set Timezone
  verifyTimeZone=$(encode "$KP105_GET_TIMEZONE" | nc -n $KP105_IP $KP105_PORT | tr -d '\0')
  decodedResponse=$(decode "$verifyTimeZone" | grep -oa "time not sync")

  # Configure Timezone
  if [[ ! -z $decodedResponse ]]; then
    encode "$KP105_SET_TIMEZONE" | nc -n $KP105_IP $KP105_PORT > /dev/null
  fi

  # Scheduled Time: convert HH:MM to Minute only format
  scheduleTime=$(( 10#$(echo "$KP105_ARG" | cut -f1 -d ':') * 60 + $(echo "$KP105_ARG" | cut -f2 -d ':') ))

  #Add New Schedule Rule
  request="{\"schedule\":{\"add_rule\":{\"stime_opt\":0,\"etime_opt\":-1,\"enable\":1,\"repeat\":1,\
  \"sact\":0,\
  \"wday\":[1,1,1,1,1,1,1],\
  \"smin\":$scheduleTime,
  \"name\":\"LightsOFF\"}}}"

  encode "$request" | nc -n $KP105_IP $KP105_PORT > /dev/null

  # Recover Schedule rules
  getScheduleRules
}


########################
# ENCODE KP105 MESSAGE #
########################
encode() {
  key=0xAB  
  
  # Convert Input to Hex (with 800 bytes per line)
  hexInput=$(echo -n "$1" | xxd -u -p -c800)

  # Coding
  result=""
  for ((i=0; i<${#hexInput}; i=i+2))
  do
    # XOR
    xor=$(( 0x${hexInput:$i:2} ^ $key ))

    # Convert to Hex format
    xor=$(printf '%X' "$xor")

    # Save new key and store the result
    key="0x"$xor
    result=$result$xor
  done

  # Add Header Length
  length=$(printf '%.8X' $(( ${#hexInput}/2 )))
  result=$length$result

  # Convert Hex to Binary format
  echo -n "$result" | xxd -r -p
}


########################
# DECODE KP105 MESSAGE #
########################
decode() {
  key=0xAB

  # Convert Input to Hex (with 8000 bytes per line)
  hexInput=$(echo -n "$1" | xxd -u -b -p -c8000)

  # Remove header length (start with "{")
  search="D0"
  rest=${hexInput#*$search}
  index=$(( ${#hexInput} - ${#rest} - ${#search} ))

  # Decoding, header length offset 4 first bytes
  result=""
  for ((i=index; i<${#hexInput}; i=i+2))
  do
    # XOR
    xor=$(( 0x${hexInput:$i:2} ^ $key ))

    # Convert to Hex format
    xor=$(printf '%X' "$xor")

    # Save new key and store the result
    key="0x"${hexInput:$i:2}
    result=$result$xor
  done

  # Convert Hex to ASCII
  echo -n "$result" | xxd -r -p
}


################
# MAIN PROGRAM #
################

case "$KP105_CMD" in

  ON)
    on
    ;;

  OFF)
    off
    ;;

  STATE)
    getState
    ;;

  LIST)
    getScheduleRules
    ;;

  RESET)
    resetScheduleRules
    ;;

  DELETE)
    deleteScheduleRule 
    ;;

  ADD_ON)
    setTurnOnSchedule
    ;;

  ADD_OFF)
    setTurnOffSchedule
    ;;

  *)
    usage
    ;;
esac