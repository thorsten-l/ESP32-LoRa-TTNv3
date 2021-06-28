#!/bin/sh

if [ -x "private" ]; then
  echo private directory exists
else
  cp -rv config private
  echo "edit your private settings in ./private/AppConfig.h"
fi
