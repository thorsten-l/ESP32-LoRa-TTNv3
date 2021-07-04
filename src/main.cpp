#include <Arduino.h>
#include <WiFi.h>
#include <App.hpp>
#include <AppConfig.h>
#include <DisplayHandler.hpp>
#include <LoRaWANHandler.hpp>

void printAsDouble(const char *label, uint32_t value, double divisor,
                   const char *unit)
{
    Serial.print(label);
    double dvalue = (double)value;
    dvalue /= divisor;
    Serial.print(dvalue);
    Serial.println(unit);
}

void setup()
{
    Serial.begin(115200);

#ifdef BUILTIN_LED
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
#endif

    WiFi.mode(WIFI_OFF); // turn wifi module off

    display.init();
    display.flipScreenVertically();
    display.clear();
    display.setBrightness(255);
    display.display();

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, APP_NAME );
    display.drawString(0, 12, "Version: " APP_VERSION );
    display.drawString(0, 24, "Build Date: " __DATE__);
    display.drawString(0, 36, "Build Time: " __TIME__);
    display.drawString(0, 48, "Booting...");
    display.display();

    loRaWANHandler.setup();

    delay(3000);

    Serial.println("\n\n\n" APP_NAME " - Version " APP_VERSION " by " APP_AUTHOR);
    Serial.println("Build date: " __DATE__ " " __TIME__ "\n");

    Serial.printf("SDK Version         : %s\n", ESP.getSdkVersion());
    Serial.printf("PIO Environment     : %s\n", PIOENV);
    Serial.printf("PIO Platform        : %s\n", PIOPLATFORM);
    Serial.printf("PIO Framework       : %s\n", PIOFRAMEWORK);
    Serial.printf("Arduino Board       : %s\n", ARDUINO_BOARD);
    Serial.println();
    Serial.printf("CPU Frequency       : %dMHz\n", ESP.getCpuFreqMHz());
    printAsDouble("Flash Chip          : ", ESP.getFlashChipSize(), 1048576, "MB");
    printAsDouble("Flash Chip Speed    : ", ESP.getFlashChipSpeed(), 1000000, "MHz");
    Serial.println();
    printAsDouble("Heap Size           : ", ESP.getHeapSize(), 1024, "KB");
    printAsDouble("Free Heap           : ", ESP.getFreeHeap(), 1024, "KB");
    printAsDouble("Sketch Size         : ", ESP.getSketchSize(), 1024, "KB");
    printAsDouble("Free Sketch Space   : ", ESP.getFreeSketchSpace(), 1024, "KB");

    Serial.println();

#ifdef BUILTIN_LED
    Serial.printf( "BUILTIN_LED=%d\n", BUILTIN_LED );
#endif
    Serial.printf( "OLED_RST=%d\n", OLED_RST );

    display.clear();
    display.display();

    loRaWANHandler.start();
}

void loop()
{
    loRaWANHandler.runOnce();
}
