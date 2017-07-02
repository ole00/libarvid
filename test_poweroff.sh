#!/bin/bash

infinity=1

#remove the 'stop' file if it exists
rm -f stop

echo "Starting"

while [ $infinity -gt 0 ]; do
	#start the server 
	#use -noServiceScreen parameter to hide the service screen and keep the logo
	./arvid-server

	#check retun code 50 which is 'poweroff' request
	if [ $? -eq 50 ]; then
		echo "Arvid power-off detected..."
		exit 50
	fi

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

