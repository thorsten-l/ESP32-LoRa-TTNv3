# ESP32-LoRa-TTNv3-OTAA

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

