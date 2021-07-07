# ESP32-LoRa-TTNv3

This sample PlatformIO project sends every minute 'Hello LoRa Fcnt=nnn' via LoRaWAN to TheThingsNetwork.
It is a test code for TTNv3

## Initialize Build Environment

`./INITIALIZE_BUILD_ENVIRONMENT.sh` copies `config` to `private` directory.

make all your private settings in `./private/AppConfig.h`

## LMIC Project Settings

Find the LMIC project settings at 

`.pio/libdeps/<board name>/MCCI LoRaWAN LMIC library/project_config/lmic_project_config.h`

e.g. `#define CFG_eu868 1` for Europe 868MHz Sender/Receiver

## Docker
If you do not have or want to install a PlatformIO Environment but you have a docker engine running. Go into the `docker`-directory 

1. ./BUILD.sh - builds `platformio-esp32` docker image based on Ubuntu 20.04
2. ./RUN_PLATFORMIO.sh - runns `platformio` twice 
3. find firmware `.pio/build/heltec_wifi_lora_32/firmware.bin`
4. upload firmware via `esptool`

## TTNv3 payload formatter

Find a JavaScript payload formatter in the `TTNv3` directory.

## References

- [Heltec WiFi LoRa 32](https://heltec.org/project/wifi-lora-32/)
- [Heltec GitHub ESP32 LoRaWAN Repository](https://github.com/HelTecAutomation/ESP32_LoRaWAN)
- [TheThingsNetwork](https://www.thethingsnetwork.org/)
- [MCCI Arduino LoRaWAN Library](https://github.com/mcci-catena/arduino-lorawan)

## Pinouts

```text
PIO Environment : heltec_wifi_lora_32
LMIC_NSS  = 18
LMIC_RXTX = 255
LMIC_RST  = 14
LMIC_DIO0 = 26
LMIC_DIO1 = 33
LMIC_DIO2 = 32
OLED_SDA = 4
OLED_SCL = 15
OLED_RST = 16
BUILTIN_LED = 25

PIO Environment : heltec_wifi_lora_32_V2
LMIC_NSS  = 18
LMIC_RXTX = 255
LMIC_RST  = 14
LMIC_DIO0 = 26
LMIC_DIO1 = 35
LMIC_DIO2 = 34
OLED_SDA = 4
OLED_SCL = 15
OLED_RST = 16
BUILTIN_LED = 25

PIO Environment : ttgo-lora32-v1
LMIC_NSS  = 18
LMIC_RXTX = 255
LMIC_RST  = 14
LMIC_DIO0 = 26
LMIC_DIO1 = 33
LMIC_DIO2 = 32
OLED_SDA = 4
OLED_SCL = 15
OLED_RST = 16
BUILTIN_LED = 2

PIO Environment : ttgo-lora32-v2
LMIC_NSS  = 18
LMIC_RXTX = 255
LMIC_RST  = 23
LMIC_DIO0 = 26
LMIC_DIO1 = 33
LMIC_DIO2 = 32
OLED_SDA = 21
OLED_SCL = 22
OLED_RST = 16
BUILTIN_LED = 22 *this is useless*, same as OLED_SCL

PIO Environment : ttgo-lora32-v21
LMIC_NSS  = 18
LMIC_RXTX = 255
LMIC_RST  = 23
LMIC_DIO0 = 26
LMIC_DIO1 = 33
LMIC_DIO2 = 32
OLED_SDA = 21
OLED_SCL = 22
OLED_RST = 16
BUILTIN_LED = 25
```
