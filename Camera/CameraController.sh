#!/bin/bash

#################################
# EUFY CAMERA CONFIG & COMMANDS #
#################################
CAMERA_IP=$1
CAMERA_CMD=$2

CAMERA_RTPS_ADDR="rtsp://$CAMERA_IP/live0"
CAMERA_RESOLUTION="1280x720"
VIDEO_CONVERTER="http://localhost:8081/"


###################################
# DISPLAY CAMERA CONTROLLER USAGE #
###################################
usage(){
  echo "Camera Controller Usage:"
  echo -e "\tCameraController.sh <IP_ADDR> <START/STOP>"
  exit 1
}


##########################
# START CAMREA STREAMING #
##########################
start(){

	# Start WebSocket Relay:
	# Incomming Streaming: port 8081
	# Outgoing Video: port 8082
  node /home/aquapi/AquaConnect/Camera/WebsocketStreamRelay.js &> /dev/null &
	sleep 6

	# Start Video Converter (IP Cam -RTSP-> Video Converter --> Internal Server port 8081)
	ffmpeg -nostdin -i $CAMERA_RTPS_ADDR -f mpegts -c:v mpeg1video -s $CAMERA_RESOLUTION -r 30 -b:v 1000k -bf 0 $VIDEO_CONVERTER &> /dev/null &
}


#########################
# STOP CAMREA STREAMING #
#########################
stop(){
  kill -SIGKILL $(pidof node) &>/dev/null
  kill -SIGKILL $(pidof ffmpeg) &>/dev/null
}


################
# MAIN PROGRAM #
################

case "$CAMERA_CMD" in

  START)
    start
    ;;

  STOP)
    stop
    ;;

  *)
    usage
    ;;
esac