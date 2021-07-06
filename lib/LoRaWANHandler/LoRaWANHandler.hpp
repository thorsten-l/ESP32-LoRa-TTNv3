#ifndef __LORAWAN_HANDLER_H__
#define __LORAWAN_HANDLER_H__

#include <SPI.h>
#include <Wire.h>
#include <SSD1306Wire.h>

class LoRaWANHandler
{
public:
  void setup();
  void runOnce();
  void start();
  void printPinout();
};

extern LoRaWANHandler loRaWANHandler;

#endif
