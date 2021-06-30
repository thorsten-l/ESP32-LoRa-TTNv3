#ifndef __DISPLAY_HANDLER_H__
#define __DISPLAY_HANDLER_H__

#include <SPI.h>
#include <Wire.h>
#include <SSD1306Wire.h>

class DisplayHandler
{

public:
  void setup();
  void printStatus(const char *status);
  void printError(const char *error);
};

extern DisplayHandler displayHandler;
extern SSD1306Wire display;

#endif
