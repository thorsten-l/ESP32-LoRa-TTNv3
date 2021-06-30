#ifndef __APP_HPP__
#define __APP_HPP__

#include <AppConfig.h>

#define LOG0( format ) Serial.printf( "(%lu) " format, millis())
#define LOG1( format, x) Serial.printf( "(%lu) " format, millis(), x )

#define APP_NAME "ESP32-LoRa-TTNv3-OTAA"
#define APP_VERSION "1.0.0"
#define APP_AUTHOR "Dr. Thorsten Ludewig <t.ludewig@gmail.com>"
#define APP_CONFIG_FILE "/config.bin"

#ifdef TTGO_LORA32_V2

#define UNUSE_PIN                   (0)

#define I2C_SDA                     21
#define I2C_SCL                     22

#define RADIO_SCLK_PIN              5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define SDCARD_MOSI                 15
#define SDCARD_MISO                 2
#define SDCARD_SCLK                 14
#define SDCARD_CS                   13

#define BOARD_LED                   25
#define LED_ON                      HIGH

#define ADC_PIN                     35

#define SS RADIO_CS_PIN
#define RST_LoRa RADIO_RST_PIN
#define DIO0 RADIO_DI0_PIN
#define DIO1 RADIO_DIO1_PIN
#define DIO2 RADIO_BUSY_PIN

#define SDA_OLED I2C_SDA
#define SCL_OLED I2C_SCL
#define RST_OLED OLED_RST
#endif

#endif
