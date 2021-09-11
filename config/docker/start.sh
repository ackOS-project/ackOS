#!/usr/bin/sh

ID=$(sudo docker run --rm -dv $(pwd):/mnt/host -w /mnt/host -ti ackos-build)

sudo docker attach $ID
