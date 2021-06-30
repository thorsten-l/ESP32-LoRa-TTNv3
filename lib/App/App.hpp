#ifndef __APP_HPP__
#define __APP_HPP__

#include <AppConfig.h>

#define LOG0( format ) Serial.printf( "(%lu) " format, millis())
#define LOG1( format, x) Serial.printf( "(%lu) " format, millis(), x )

#define APP_NAME "ESP32-LoRa-TTNv3-OTAA"
#define APP_VERSION "1.0.0"
#define APP_AUTHOR "Dr. Thorsten Ludewig <t.ludewig@gmail.com>"
#define APP_CONFIG_FILE "/config.bin"

#endif
