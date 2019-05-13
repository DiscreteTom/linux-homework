#!/bin/bash

# define utility function
getCurrentTCP() {
  # record previous variables
  previousReceivedRecord=$currentReceivedRecord
  previousSentRecord=$currentSentRecord
  previousTotal=$currentTotal

  # get new value of current record
  read currentReceivedRecord currentSentRecord \
		<<< `netstat --statistics --tcp \
		| egrep '(segments received|segments sent out)' \
		| awk 'NR==1||NR==2{printf("%s ", $1)}'`

  # calculate current virables
  currentReceived=`expr $currentReceivedRecord - $previousReceivedRecord`
  currentSent=`expr $currentSentRecord - $previousSentRecord`
  currentTotal=`expr $currentSent + $currentReceived`

  # judge state
  if [ $currentTotal -gt $previousTotal ]
  then
    state="+"
  elif [ $currentTotal -lt $previousTotal ]
  then
    state="-"
  else
    state=""
  fi
}

# set initial values
currentReceivedRecord=0
currentSentRecord=0
currentReceived=0
currentSent=0
currentTotal=0

# get initial value
getCurrentTCP

# show tip
echo start

while true
do
  sleep 1m
  getCurrentTCP
  echo `date "+%F %H:%M"` $currentSent $currentReceived $currentTotal $state
done