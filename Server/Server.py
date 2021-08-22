# Python Web Server

# Web Server Librabry
from flask import Flask



####################################
# PYTHON WEB SERVER CONFIGURATIONS #
####################################

# Web Server Configurations
WEB_SERVER_IP = "192.168.1.61"
WEB_SERVER_PORT = 5000

# Web Server Object
webServer = Flask(__name__) 


#######################################
# PYTHON WEB SERVER CAMERA CONTROLLER #
#######################################

@webServer.route('/Camera')
def Camera():
  return 'Camera'


#################################################
# PYTHON WEB SERVER CIRCULATION PUMP CONTROLLER #
#################################################

@webServer.route('/CirculationPump')
def CirculationPump():
  return 'CirculationPump'


####################################
# PYTHON WEB SERVER CO2 CONTROLLER #
####################################

@webServer.route('/CO2')
def CO2():
  return 'CO2'


#######################################
# PYTHON WEB SERVER FEEDER CONTROLLER #
#######################################

@webServer.route('/Feeder')
def Feeder():
  return 'Feeder'


#########################################
# PYTHON WEB SERVER LIGHTING CONTROLLER #
#########################################

@webServer.route('/Lighting')
def Lighting():
  return 'Lighting'


##############################################
# PYTHON WEB SERVER WATER QUALITY CONTROLLER #
##############################################

@webServer.route('/WaterQuality')
def WaterQuality():
  return 'WaterQuality'


##################################
# PYTHON WEB SERVER MAIN PROGRAM #
##################################

if __name__ == '__main__':
	webServer.run(debug=True, host=WEB_SERVER_IP, port=WEB_SERVER_PORT)