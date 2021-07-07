#ifndef __APP_HPP__
#define __APP_HPP__

#include <AppConfig.h>

#define LOG0( format ) Serial.printf( "(%lu) " format, millis())
#define LOG1( format, x) Serial.printf( "(%lu) " format, millis(), x )


#define APP_NAME "ESP32-LoRa-TTNv3"

#ifdef ACTIVATION_MODE_OTAA
#undef APP_NAME
#define APP_NAME "ESP32-LoRa-TTNv3 OTAA"
#endif

#ifdef ACTIVATION_MODE_ABP
#undef APP_NAME
#define APP_NAME "ESP32-LoRa-TTNv3 ABP"
#endif

#define APP_AUTHOR "Dr. Thorsten Ludewig <t.ludewig@gmail.com>"
#define APP_CONFIG_FILE "/config.bin"

#define NO_BAT_SAMPLES 64

#ifdef SERIAL_ON
#define SERIAL_PRINT( value ) Serial.print( value )
#define SERIAL_PRINTF( format, ... ) Serial.printf( format, __VA_ARGS__ )
#define SERIAL_PRINTB( value, base ) Serial.print( value, base )
#define SERIAL_PRINTLN( value ) Serial.println( value )
#define SERIAL_PRINTLNB( value, base ) Serial.println( value, base )
#else
#define SERIAL_PRINT( value )
#define SERIAL_PRINTF( format, ... )
#define SERIAL_PRINTB( value, base )
#define SERIAL_PRINTLN( value )
#define SERIAL_PRINTLNB( value, base )
#endif

#ifdef DISPLAY_ON
#define DISPLAY_STATUS( value ) displayHandler.printStatus( value ) 
#define DISPLAY_ERROR( value ) displayHandler.printError( value ) 
#define DISPLAY_STRING( x, y, value ) display.drawString( x, y, value ) 
#else
#define DISPLAY_STATUS( value )
#define DISPLAY_ERROR( value )
#define DISPLAY_STRING( x, y, value )
#endif

// Pinout definitions

#ifdef HELTEC
#define LMIC_NSS  SS
#define LMIC_RXTX LMIC_UNUSED_PIN
#define LMIC_RST  RST_LoRa
#define LMIC_DIO0 DIO0 
#define LMIC_DIO1 DIO1
#define LMIC_DIO2 DIO2

#define OLED_SDA  SDA_OLED
#define OLED_SCL  SCL_OLED
#define OLED_RST  RST_OLED
#endif

#ifdef TTGO_LORA32_V1
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
