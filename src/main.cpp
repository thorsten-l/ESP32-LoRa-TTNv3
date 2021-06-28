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
#include <AppConfig.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#include <Wire.h>
#include <SSD1306Wire.h>

// HelTec Automation Board WiFi LoRa
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64);
unsigned long lastTxFrameCounter = 0;
unsigned long lastRxFrameCounter = 0;

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

static uint8_t mydata[32];
static osjob_t sendjob;

const lmic_pinmap lmic_pins = {
    .nss = SS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = RST_LoRa,
    .dio = {DIO0, DIO1, DIO2},
};

void printHex2(unsigned v)
{
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void displayStatus(const char *status)
{
    display.setColor(BLACK);
    display.fillRect(0, 0, 128, 12);
    display.setColor(WHITE);
    display.drawString(0, 0, status);
    display.display();
}

void displayError(const char *error)
{
    display.setColor(BLACK);
    display.fillRect(0, 48, 128, 16);
    display.setColor(WHITE);
    display.drawString(0, 48, error);
    display.display();
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
        lastTxFrameCounter++;
        sprintf((char *)mydata, "Hello LoRa Fcnt=%ld", lastTxFrameCounter);
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, strlen((char *)mydata), 0);
        Serial.printf("%ld Packet queued\n", lastTxFrameCounter);
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
        displayStatus("JOINING");
        break;

    case EV_JOINED:
        digitalWrite(BUILTIN_LED, HIGH);
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
        displayError("JOIN_FAILED");
        break;

    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        displayError("REJOIN_FAILED");
        break;

    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));

        if (LMIC.txrxFlags & TXRX_ACK)
            Serial.println(F("Received ack"));
        if (LMIC.dataLen)
        {
            lastRxFrameCounter++;
            Serial.print(F("Received "));
            Serial.print(LMIC.dataLen);
            Serial.println(F(" bytes of payload"));
        }

        delay(2000);
        display.clear();
        display.drawString(0, 0, "TXCOMPLETE");
        char buf[32];
        sprintf(buf, "TX cnt: %ld", lastTxFrameCounter);
        display.drawString(0, 12, buf);
        sprintf(buf, "RX cnt: %ld", lastRxFrameCounter);
        display.drawString(0, 24, buf);
        display.display();

        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TTN_TX_INTERVAL), do_send);
        break;

    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        displayError("LOST_TSYNC");
        break;

    case EV_RESET:
        Serial.println(F("EV_RESET"));
        displayError("RESET");
        break;

    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;

    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        displayError("LINK_DEAD");
        break;

    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        displayStatus("LINK_ALIVE");
        break;

    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        displayStatus("TXSTART");
        break;

    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        displayError("TXCANCELED");
        break;

    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;

    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        displayStatus("NO JOIN ACCEPTED");
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);

    display.init();
    display.flipScreenVertically();
    display.clear();
    display.setBrightness(255);
    display.display();

    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "WiFi LoRa TTN");
    display.drawString(0, 12, "Version: 2021.06");
    display.drawString(0, 24, "Build Date: " __DATE__);
    display.drawString(0, 36, "Build Time: " __TIME__);
    display.drawString(0, 48, "Booting...");
    display.display();

    Serial.println(F("\n\nStarting..."));

#ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
#endif

    delay(3000);
    display.clear();
    display.display();

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop()
{
    os_runloop_once();
}
