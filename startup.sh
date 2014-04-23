#!/bin/bash

#this is the statup script executed from /etc/rc.local

#wait till everything is initialised
sleep 12

#enter directory where pru files exist
cd /home/debian/dev/libarvid

#start the server
./arvid-server
