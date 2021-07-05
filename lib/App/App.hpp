#ifndef __APP_HPP__
#define __APP_HPP__

#include <AppConfig.h>

#define LOG0( format ) Serial.printf( "(%lu) " format, millis())
#define LOG1( format, x) Serial.printf( "(%lu) " format, millis(), x )

#define APP_NAME "ESP32-LoRa-TTNv3-OTAA"
#define APP_AUTHOR "Dr. Thorsten Ludewig <t.ludewig@gmail.com>"
#define APP_CONFIG_FILE "/config.bin"

#if defined(HELTEC) || defined(TTGO_LORA32_V1)
#define LMIC_NSS  18
#define LMIC_RXTX LMIC_UNUSED_PIN
#define LMIC_RST  14
#define LMIC_DIO0 26 
#define LMIC_DIO1 33
#define LMIC_DIO2 32

#define OLED_SDA  4
#define OLED_SCL  15
#define OLED_RST  16
#endif

#ifdef TTGO_LORA32_V2
#undef  BUILTIN_LED

#define LMIC_NSS  18
#define LMIC_RXTX LMIC_UNUSED_PIN
#define LMIC_RST  23
#define LMIC_DIO0 26
#define LMIC_DIO1 33
#define LMIC_DIO2 32

#define OLED_SDA  21
#define OLED_SCL  22
#define OLED_RST  16

#define ADC_PIN   35
#endif

#ifdef TTGO_LORA32_V21

#define LMIC_NSS  18
#define LMIC_RXTX LMIC_UNUSED_PIN
#define LMIC_RST  23
#define LMIC_DIO0 26
#define LMIC_DIO1 33
#define LMIC_DIO2 32

#define OLED_SDA  21
#define OLED_SCL  22
#define OLED_RST  16

#define ADC_PIN   35
#endif


#endif
