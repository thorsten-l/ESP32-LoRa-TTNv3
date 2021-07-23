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
#include <FS.h>
#include <SPIFFS.h>
#include <Preferences.h>

#define uS_TO_S_FACTOR 1000000

LoRaWANHandler loRaWANHandler;

RTC_DATA_ATTR unsigned long rxFrameCounter = 0;

int bwf[] = {125, 250, 500, 750};

#ifdef ACTIVATION_MODE_OTAA
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
#endif

#ifdef ACTIVATION_MODE_ABP
static Preferences preferences;

static uint8_t NETWORK_SESSION_KEY[16] = TTN_NETWORK_SESSION_KEY;
static uint8_t APP_SESSION_KEY[16] = TTN_APP_SESSION_KEY;
static u4_t DEVADDR = TTN_DEVICE_ADDRESS;

void os_getArtEui(u1_t *buf) {}
void os_getDevEui(u1_t *buf) {}
void os_getDevKey(u1_t *buf) {}
#endif

// static uint8_t mydata[64];
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
        SERIAL_PRINT('0');
    SERIAL_PRINTB(v, HEX);
}

void do_send(osjob_t *j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        SERIAL_PRINTLN(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        lora_send(LMIC.seqnoUp);
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent(ev_t ev)
{
    SERIAL_PRINT(os_getTime());
    SERIAL_PRINT(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        SERIAL_PRINTLN(F("EV_SCAN_TIMEOUT"));
        break;

    case EV_BEACON_FOUND:
        SERIAL_PRINTLN(F("EV_BEACON_FOUND"));
        break;

    case EV_BEACON_MISSED:
        SERIAL_PRINTLN(F("EV_BEACON_MISSED"));
        break;

    case EV_BEACON_TRACKED:
        SERIAL_PRINTLN(F("EV_BEACON_TRACKED"));
        break;

    case EV_JOINING:
        SERIAL_PRINTLN(F("EV_JOINING"));
        DISPLAY_STATUS("JOINING");
        break;

    case EV_JOINED:
    {
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
        LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);

#if defined(BUILTIN_LED) && defined(BUILTIN_LED_ENABLED)
        digitalWrite(BUILTIN_LED, HIGH);
#endif

#ifdef DISPLAY_ENABLED
        display.drawString(0, 12, "JOINED");
        display.display();
#endif

#ifdef SERIAL_ENABLED
        SERIAL_PRINTLN(F("EV_JOINED"));
        SERIAL_PRINT("netid: ");
        SERIAL_PRINTLNB(netid, DEC);
        SERIAL_PRINT("devaddr: ");
        SERIAL_PRINTLNB(devaddr, HEX);
        SERIAL_PRINT("AppSKey: ");
        for (size_t i = 0; i < sizeof(artKey); ++i)
        {
            if (i != 0)
                SERIAL_PRINT("-");
            printHex2(artKey[i]);
        }
        SERIAL_PRINTLN("");
        SERIAL_PRINT("NwkSKey: ");
        for (size_t i = 0; i < sizeof(nwkKey); ++i)
        {
            if (i != 0)
                SERIAL_PRINT("-");
            printHex2(nwkKey[i]);
        }
        SERIAL_PRINTLN();
#endif

        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
    }
    break;

    case EV_JOIN_FAILED:
        SERIAL_PRINTLN(F("EV_JOIN_FAILED"));
        DISPLAY_ERROR("JOIN_FAILED");
        break;

    case EV_REJOIN_FAILED:
        SERIAL_PRINTLN(F("EV_REJOIN_FAILED"));
        DISPLAY_ERROR("REJOIN_FAILED");
        break;

    case EV_TXCOMPLETE:
        SERIAL_PRINTLN(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
#ifdef DISPLAY_ENABLED
        display.clear();
#endif
        if (LMIC.txrxFlags & TXRX_ACK)
            SERIAL_PRINTLN(F("Received ack"));

        SERIAL_PRINTF("RSSI: %d\n", (int)LMIC.rssi);
        SERIAL_PRINTF("SNR: %d\n", (int)LMIC.snr);
        SERIAL_PRINTF("SF: %d\n", (LMIC.rps & 0x07) + 6);
        SERIAL_PRINTF("BW: %d\n", bwf[(LMIC.rps >> 3) & 0x03]);

        if (LMIC.dataLen)
        {
            rxFrameCounter++;
            lora_receive(rxFrameCounter);
        }

#ifdef DISPLAY_ENABLED
        display.drawString(0, 0, "TXCOMPLETE");
        char buf[32];
        sprintf(buf, "TXC: %d", (LMIC.seqnoUp-1));
        display.drawString(0, 12, buf);
        sprintf(buf, "RXC: %ld (%d)", rxFrameCounter, LMIC.dataLen);
        display.drawString(52, 12, buf);
        sprintf(buf, "RSSI: %d", (int)LMIC.rssi);
        display.drawString(0, 24, buf);
#ifdef ADC_PIN
        sprintf(buf, "BAT: %.02fV\n", analogRead(ADC_PIN) * 6.6 / 4095.0);
        display.drawString(52, 24, buf);
#endif
        sprintf(buf, "SNR: %u", LMIC.snr);
        display.drawString(0, 36, buf);
        sprintf(buf, "SF: %u", (LMIC.rps & 0x07) + 6);
        display.drawString(52, 36, buf);
        sprintf(buf, "BW: %u", bwf[(LMIC.rps >> 3) & 0x03]);
        display.drawString(88, 36, buf);

        sprintf(buf, "FREQ: %u", LMIC.freq);
        display.drawString(0, 48, buf);
        display.display();
        delay(1000);
#endif

#ifdef ACTIVATION_MODE_ABP
        preferences.putUInt(SEQUENCE_KEY, LMIC.seqnoUp);
        SERIAL_PRINTF( "TXC " SEQUENCE_KEY "=%u\n", LMIC.seqnoUp);
#endif

#ifdef DEEP_SLEEP_ENABLED
        ESP.deepSleep(TRANSMIT_INTERVAL * uS_TO_S_FACTOR);
        yield();
#else
        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TRANSMIT_INTERVAL), do_send);
#endif
        break;

    case EV_LOST_TSYNC:
        SERIAL_PRINTLN(F("EV_LOST_TSYNC"));
        DISPLAY_ERROR("LOST_TSYNC");
        break;

    case EV_RESET:
        SERIAL_PRINTLN(F("EV_RESET"));
        DISPLAY_ERROR("RESET");
        break;

    case EV_RXCOMPLETE:
        // data received in ping slot
        SERIAL_PRINTLN(F("EV_RXCOMPLETE"));
        break;

    case EV_LINK_DEAD:
        SERIAL_PRINTLN(F("EV_LINK_DEAD"));
        DISPLAY_ERROR("LINK_DEAD");
        break;

    case EV_LINK_ALIVE:
        SERIAL_PRINTLN(F("EV_LINK_ALIVE"));
        DISPLAY_STATUS("LINK_ALIVE");
        break;

    case EV_TXSTART:
        SERIAL_PRINTLN(F("EV_TXSTART"));
        DISPLAY_STATUS("TXSTART");
        SERIAL_PRINTF("RSSI: %d\n", (int)LMIC.rssi);
        SERIAL_PRINTF("SNR: %d\n", (int)LMIC.snr);
        SERIAL_PRINTF("SF: %d\n", (LMIC.rps & 0x07) + 6);
        break;

    case EV_TXCANCELED:
        SERIAL_PRINTLN(F("EV_TXCANCELED"));
        DISPLAY_ERROR("TXCANCELED");
        break;

    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;

    case EV_JOIN_TXCOMPLETE:
        SERIAL_PRINTLN(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        DISPLAY_STATUS("NO JOIN ACCEPTED");
        break;

    default:
        SERIAL_PRINT(F("Unknown event: "));
        SERIAL_PRINTLN((unsigned)ev);
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
    LMIC.rssi = 0;
    LMIC_reset();

#ifdef ACTIVATION_MODE_ABP
    LMIC_setSession(0x13, DEVADDR, NETWORK_SESSION_KEY, APP_SESSION_KEY);

    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI); // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);  // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK, DR_FSK), BAND_MILLI);   // g2-band  // LMIC_setClockError(7);

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;

    // Set data rate and transmit power for uplink
    LMIC_setDrTxpow(DR_SF7, 14);
#endif
}

void LoRaWANHandler::runOnce()
{
    os_runloop_once();
}

void LoRaWANHandler::start()
{
#ifdef ACTIVATION_MODE_ABP
    preferences.begin(PREFERENCE_NAME);
    LMIC.seqnoUp = preferences.getUInt(SEQUENCE_KEY);
    preferences.putUInt(SEQUENCE_KEY, LMIC.seqnoUp);
    SERIAL_PRINTF(SEQUENCE_KEY "=%u\n", LMIC.seqnoUp);
#endif

    do_send(&sendjob);
}

void LoRaWANHandler::printPinout()
{
    SERIAL_PRINTF("\nPIO Environment : %s\n", PIOENV);
    SERIAL_PRINTF("LMIC_NSS  = %d\n", LMIC_NSS);
    SERIAL_PRINTF("LMIC_RXTX = %d\n", LMIC_RXTX);
    SERIAL_PRINTF("LMIC_RST  = %d\n", LMIC_RST);
    SERIAL_PRINTF("LMIC_DIO0 = %d\n", LMIC_DIO0);
    SERIAL_PRINTF("LMIC_DIO1 = %d\n", LMIC_DIO1);
    SERIAL_PRINTF("LMIC_DIO2 = %d\n", LMIC_DIO2);
    SERIAL_PRINTF("OLED_SDA = %d\n", OLED_SDA);
    SERIAL_PRINTF("OLED_SCL = %d\n", OLED_SCL);
    SERIAL_PRINTF("OLED_RST = %d\n", OLED_RST);

#ifdef BUILTIN_LED
    SERIAL_PRINTF("BUILTIN_LED = %d\n", BUILTIN_LED);
#endif
    SERIAL_PRINTLN();
}
