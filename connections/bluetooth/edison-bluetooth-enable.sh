#!/bin/sh
rfkill unblock bluetooth
sleep 1
hciconfig hci0 up
sleep 1
hciconfig hci0 piscan
