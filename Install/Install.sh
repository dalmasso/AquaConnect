#!/bin/bash

echo "AquaConnect Install"

###################
# UPDATE RASPBIAN #
###################
sudo apt-get update

#############
# LIBRARIES #
#############

# USB Hub Control
sudo apt-get install -y uhubctl

# Python3 & Flask Server Library
sudo apt-get install -y python3 \
python3-flask

# JSON Parser
sudo apt-get install -y jq

# Video Converter (RTSP to MPEG)
sudo apt-get install -y ffmpeg \
nodejs \
npm


###########
# GRAFANA #
###########

# Install MySQL Database
sudo apt install -y mariadb-server

# Configure MySQL Grafana Client
sudo service mysql start
sudo mysql -e "CREATE USER 'grafanaReader'@'localhost'"
sudo mysql -e "GRANT ALL PRIVILEGES ON *.* TO 'grafanaReader'@'localhost';"
sudo mysql -e "FLUSH PRIVILEGES;"

# Enable & Restart MySQL
sudo /bin/systemctl restart mysql

# Authenticate Packages
wget -q -O - https://packages.grafana.com/gpg.key | sudo apt-key add -

# Add Repository
echo "deb https://packages.grafana.com/oss/deb stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list

# Install Grafana
sudo apt-get update
sudo apt-get install -y grafana

# Enable Grafana Server
sudo /bin/systemctl enable grafana-server

# Grafana Disable Sanitize HTML
sudo sed -i "s/disable_sanitize_html = .*/disable_sanitize_html = true/g" /usr/share/grafana/conf/defaults.ini

# Grafana Video Plugin
sudo grafana-cli plugins install innius-video-panel

# Start Grafana (Access: http://<ip address>:3000)
sudo /bin/systemctl start grafana-server


###############
# AQUACONNECT #
###############

# Install Streaming Setup
# IP Cam (RTSP) -> Video Converter (MPEG)
# Video Converter -> Internal Server (port 8081)
# Internal Server (port 8082) -> WebSocket -> Web Browser/Dashboard
npm install --prefix ~/AquaConnect/Camera/ ws
wget https://johnvansickle.com/ffmpeg/builds/ffmpeg-git-armhf-static.tar.xz
tar xvf ffmpeg-git-armhf-static.tar.xz
sudo cp ffmpeg-*/ffmpeg /usr/bin/




# Start AquaConnect Server at Boot
# .bashrc python3 Server.py



# Enable AquaConnect script executions
find ~/AquaConnect -type f -iname "*.sh" -exec chmod +x {} \;

# Install AquaConnect Controller scripts for Apache Server used by Grafana
sudo find ~/AquaConnect -type f -iname "*Controller.sh" -exec cp {} /usr/lib/cgi-bin \;


# Enable Access to WebSocket from Apache Server
sudo cp ~/AquaConnect/Camera/jsmpeg.min.js /var/www/html
sudo cp ~/AquaConnect/Camera/ViewStream.html /var/www/html

# Install AquaConnect Dashboard & DataSource
#.yaml /etc/grafana/provisioning/datasources/
#.js /usr/share/grafana/public/dashboards/




# #################
# # Apache Server #
# #################

# # Install Apache Server
# sudo apt-get install -y apache2

# # Enable Apache Server
# sudo /bin/systemctl enable apache2

# # Enable CGI module
# sudo a2enmod cgi
# sudo /bin/systemctl restart apache2