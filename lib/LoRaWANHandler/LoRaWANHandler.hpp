#ifndef __LORAWAN_HANDLER_H__
#define __LORAWAN_HANDLER_H__

#include <SPI.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <lmic.h>

#define PREFERENCE_NAME "lmic"
#define SEQUENCE_KEY "sequence"

class LoRaWANHandler
{
public:
  void setup();
  void runOnce();
  void start();
  void printPinout();
};

extern LoRaWANHandler loRaWANHandler;

extern void lora_send(unsigned long txFrameCounter);
extern void lora_receive(unsigned long rxFrameCounter);

#endif
