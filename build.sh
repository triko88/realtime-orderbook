#!/bin/sh

mkdir build
cd build

cmake ..
make
mv ./realtime-orderbook ..
