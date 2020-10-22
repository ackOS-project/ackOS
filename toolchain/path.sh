#!/bin/bash

DIR=$(dirname $(readlink -f $0))

echo $DIR/local/bin/:$PATH
