#!/usr/bin/sh

DIR=$(dirname $(readlink -f $0))

cd $DIR

sudo docker build . -t ackos-build
