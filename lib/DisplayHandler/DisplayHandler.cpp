#include <Arduino.h>
#include <App.hpp>
#include "DisplayHandler.hpp"

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, OLED_RST, GEOMETRY_128_64);
DisplayHandler displayHandler;

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