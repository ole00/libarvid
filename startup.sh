#!/bin/bash

#this is the statup script executed from /etc/rc.local

#wait till everything is initialised
sleep 16

#enter directory where pru files exist
cd /home/debian/dev/libarvic

#start the server
./arvic-server
