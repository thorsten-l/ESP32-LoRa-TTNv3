/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/

#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <DisplayHandler.hpp>
#include "LoRaWANHandler.hpp"
#include <App.hpp>

#define NO_BAT_SAMPLES 64

LoRaWANHandler loRaWANHandler;

unsigned long txFrameCounter = 0;
unsigned long rxFrameCounter = 0;

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = TTN_APPLICATION_EUI;
void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = TTN_DEVICE_EUI;
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = TTN_APP_KEY;
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

static uint8_t mydata[64];
static osjob_t sendjob;

const lmic_pinmap lmic_pins = {
    .nss = LMIC_NSS,
    .rxtx = LMIC_RXTX,
    .rst = LMIC_RST,
    .dio = {LMIC_DIO0, LMIC_DIO1, LMIC_DIO2}};

void printHex2(unsigned v)
{
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void do_send(osjob_t *j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        txFrameCounter++;

#ifdef ADC_PIN
        float bat_sum = 0;
        for (int i = 0; i < NO_BAT_SAMPLES; i++)
        {
            bat_sum += analogRead(ADC_PIN);
        }
        bat_sum /= NO_BAT_SAMPLES;

        // 3.3 / 4095 * 2 =
        float bat = bat_sum * 6.6 / 4095.0;
        Serial.printf("bat=%.02fV\n", bat);
        sprintf((char *)mydata, "Fcnt=%ld, bat=%.02fV", txFrameCounter, bat);
#else
        sprintf((char *)mydata, "Fcnt=%ld", txFrameCounter);
#endif

        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, strlen((char *)mydata), 0);
        Serial.printf("%ld Packet queued\n", txFrameCounter);
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent(ev_t ev)
{
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;

    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;

    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;

    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;

    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        displayHandler.printStatus("JOINING");
        break;

    case EV_JOINED:

#ifdef BUILTIN_LED
        digitalWrite(BUILTIN_LED, HIGH);
#endif

        display.drawString(0, 12, "JOINED");
        display.display();
        Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(artKey[i]);
            }
            Serial.println("");
            Serial.print("NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(nwkKey[i]);
            }
            Serial.println();
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
        break;

    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        displayHandler.printError("JOIN_FAILED");
        break;

    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        displayHandler.printError("REJOIN_FAILED");
        break;

    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        display.clear();

        if (LMIC.txrxFlags & TXRX_ACK)
            Serial.println(F("Received ack"));
        if (LMIC.dataLen)
        {
            rxFrameCounter++;
            Serial.print(F("Received "));
            Serial.print(LMIC.dataLen);
            Serial.println(F(" bytes of payload"));
            for (int i = 0; i < LMIC.dataLen; i++)
            {
                Serial.printf("%02X ", LMIC.frame[LMIC.dataBeg + i]);
            }
            Serial.println();

            for (int i = 0; i < 8 && i < LMIC.dataLen; i++)
            {
                char buf[3];
                sprintf(buf, "%02X", LMIC.frame[LMIC.dataBeg + i]);
                display.drawString(i * 16, 36, buf);
            }
        }

        display.drawString(0, 0, "TXCOMPLETE");
        char buf[32];
        sprintf(buf, "TX cnt: %ld", txFrameCounter);
        display.drawString(0, 12, buf);
        sprintf(buf, "RX cnt: %ld (%d)", rxFrameCounter, LMIC.dataLen);
        display.drawString(0, 24, buf);
        display.display();

        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TTN_TX_INTERVAL), do_send);
        break;

    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        displayHandler.printError("LOST_TSYNC");
        break;

    case EV_RESET:
        Serial.println(F("EV_RESET"));
        displayHandler.printError("RESET");
        break;

    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;

    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        displayHandler.printError("LINK_DEAD");
        break;

    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        displayHandler.printStatus("LINK_ALIVE");
        break;

    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        displayHandler.printStatus("TXSTART");
        break;

    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        displayHandler.printError("TXCANCELED");
        break;

    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;

    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        displayHandler.printStatus("NO JOIN ACCEPTED");
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void LoRaWANHandler::setup()
{
#ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
#endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
}

void LoRaWANHandler::runOnce()
{
    os_runloop_once();
}

void LoRaWANHandler::start()
{
    do_send(&sendjob);
}

void LoRaWANHandler::printPinout()
{
    Serial.printf("\nPIO Environment : %s\n", PIOENV);
    Serial.printf("LMIC_NSS  = %d\n", LMIC_NSS);
    Serial.printf("LMIC_RXTX = %d\n", LMIC_RXTX);
    Serial.printf("LMIC_RST  = %d\n", LMIC_RST);
    Serial.printf("LMIC_DIO0 = %d\n", LMIC_DIO0);
    Serial.printf("LMIC_DIO1 = %d\n", LMIC_DIO1);
    Serial.printf("LMIC_DIO2 = %d\n", LMIC_DIO2);
    Serial.printf("OLED_SDA = %d\n", OLED_SDA);
    Serial.printf("OLED_SCL = %d\n", OLED_SCL);
    Serial.printf("OLED_RST = %d\n", OLED_RST);

#ifdef BUILTIN_LED
    Serial.printf( "BUILTIN_LED = %d\n", BUILTIN_LED );
#endif
    Serial.println();
}
