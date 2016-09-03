#!/bin/bash

#this is the startup script executed from /etc/rc.local

infinity=1

#enter directory where pru files exist
cd /home/debian/libarvid

# start arvid-splash here if you want fancy splash/logo
# during boot

#wait till everything is initialised & settled.
#don't skip this step otherwise the network might not be ready
sleep 14

#stop unwanted services
service apache2 stop
systemctl stop wpa_supplicant.service
systemctl stop bonescript-autorun.service


#remove the 'stop' file if it exists
rm -f stop

echo "Starting" > log.txt

while [ $infinity -gt 0 ]; do
	#start the server 
	#use -noServiceScreen parameter to hide the service screen and keep the logo
	./arvid-server  >> log.txt

	#if the server unexpectedly stopped, then start it again
	#unless 'stop' file exists

	#check whether to stop the loop if file 'stop' exists
	if [ -e stop ]; then
		echo "Stop found. Exiting..." >> log.txt
		exit 0
	else
		sleep 2
	fi
done

