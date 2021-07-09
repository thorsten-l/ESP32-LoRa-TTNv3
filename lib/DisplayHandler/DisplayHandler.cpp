#include <Arduino.h>
#include <App.hpp>
#include "DisplayHandler.hpp"

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, OLED_RST, GEOMETRY_128_64);
DisplayHandler displayHandler;

void DisplayHandler::setup()
{
    display.init();
#ifdef DISPLAY_ENABLED
    display.flipScreenVertically();
    display.clear();
    display.setBrightness(255);
    display.display();

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, APP_NAME);
    display.drawString(0, 12, "Board: " PIOENV);
    display.drawString(0, 24, "Version: " APP_VERSION);
    display.drawString(0, 36, "Build Date: " __DATE__);
    display.drawString(0, 48, "Build Time: " __TIME__);
    display.display();
#else
    display.displayOff();
#endif
}

void DisplayHandler::printStatus(const char *status)
{
    display.setColor(BLACK);
    display.fillRect(0, 0, 128, 12);
    display.setColor(WHITE);
    display.drawString(0, 0, status);
    display.display();
}

void DisplayHandler::printError(const char *error)
{
    display.setColor(BLACK);
    display.fillRect(0, 48, 128, 16);
    display.setColor(WHITE);
    display.drawString(0, 48, error);
    display.display();
}