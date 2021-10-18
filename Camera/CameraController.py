# CameraController

import subprocess
import sys
import time


#################################
# EUFY CAMERA CONFIG & COMMANDS #
#################################
CAMERA_IP="192.168.1.69"
CAMERA_RTPS_ADDR="rtsp://"+CAMERA_IP+"/live0"
CAMERA_RESOLUTION="1280x720"
VIDEO_CONVERTER="http://localhost:8081/"


###################################
# DISPLAY CAMERA CONTROLLER USAGE #
###################################
def usage():
	print("Camera Controller Usage:")
	print("\tCameraController.py <START/STOP>")
	exit()


##########################
# START CAMREA STREAMING #
##########################
def start():

	# Start WebSocket Relay:
	# Incomming Streaming: port 8081
	# Outgoing Video: port 8082
	subprocess.run("node /home/aquapi/AquaConnect/Camera/WebsocketStreamRelay.js &", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
	time.sleep(6)

	# Start Video Converter (IP Cam -RTSP-> Video Converter --> Internal Server port 8081)
	subprocess.run("ffmpeg -nostdin -i " + CAMERA_RTPS_ADDR + " -f mpegts -c:v mpeg1video -s " + CAMERA_RESOLUTION + " -r 30 -b:v 1000k -bf 0 " + VIDEO_CONVERTER + " &", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


#########################
# STOP CAMREA STREAMING #
#########################
def stop():
	subprocess.run("kill $(pidof node)", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
	subprocess.run("kill $(pidof ffmpeg)", shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


################
# MAIN PROGRAM #
################

if __name__ == '__main__':

	# No Argument (Script Name - Argument)
	if len(sys.argv) != 2:
		usage()

	# Argument: Start Camera
	if sys.argv[1] == "START":
		start()

	# Argument: Stop Camera
	elif sys.argv[1] == "STOP":
		stop()

	# Unsupported Argument
	else:
		usage()