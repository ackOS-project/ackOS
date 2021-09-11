#!/usr/bin/sh

CONTAINER=ackos-build
ID=$(sudo docker run --rm -dv $(pwd):/mnt/host -w /mnt/host -ti $CONTAINER)

sudo docker attach $ID 
