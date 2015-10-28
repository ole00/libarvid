#!/bin/bash

#this is the statup script executed from /etc/rc.local

infinity=1

#enter directory where pru files exist
cd /home/debian/dev/libarvid

#show splash
./arvid-splash mame.png > splash.txt

#wait till everything is initialised & settled
sleep 14

#remove 'stop' file if it exists
rm -f stop

echo "Starting" > log.txt

while [ $infinity -gt 0 ]; do
	#start the server
	./arvid-server >> log.txt

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

