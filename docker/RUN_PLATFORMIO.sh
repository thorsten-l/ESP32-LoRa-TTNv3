#!/bin/sh
cd ..

docker run -it --rm \
  -v `pwd`:/workdir \
  --name platformio-esp32 platformio-esp32 \
  /usr/local/bin/platformio run
