#include <Arduino.h>
#include <Streaming.h>
#include <stdio.h>

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
uint16_t ID;
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>

#include <TinyGPS++.h>
TinyGPSPlus gps;

#include "DayLightSaving.h"
DaylightSaving dls;

#include "commons.h"

/**
 *
 */
bool timeHasPassed(long fromWhen, int howLong) {
    return fromWhen > 0 && millis() - fromWhen >= howLong;
}

/**
 * LED villogtatása, ha van GPS bejövő mondat
 */
void gpsDataReceivedLED() {
#ifdef __DEBUG

    static bool ledState = false;
    static long stateChanged = millis();

    // Ha eltelt már 10msec és a LED aktív, akkor kikapcsoljuk a LED-et
    if (timeHasPassed(stateChanged, 10) && ledState) {
        ledState = false;
        digitalWrite(LED_BUILTIN, ledState);
        return;
    }

    if (timeHasPassed(stateChanged, 1000)) {
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
        stateChanged = millis();
    }
#endif
}

/**
 * A megadott string területét törli
 */
void clearStrRect(const char *str, int16_t strX, int16_t strY) {
    int16_t x1, y1;
    uint16_t w, h;

    // Lekérjük, hogy a String mekkora területet foglal el
    tft.getTextBounds(str, strX, strY, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w + 8, h, TFT_BLACK);

    // Az elejére visszük a kurzort
    tft.setCursor(strX, strY);
}

/**
 * Sebesség kijelzése
 */
#define LOC_SPEED_X 175
#define LOC_SPEED_Y 300
void displaySpeed(int speed) {
    tft.setFont(&FreeSerifBold24pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    clearStrRect("888", LOC_SPEED_X, LOC_SPEED_Y);
    tft.printf("%*s", 3, String(speed).c_str());
}

/**
 * Keretek és feliratok kirajzolása
 */
void drawDisplay() {
#define BLOCK_HEIGHT 70
#define BLOCK_RADIUS 10

    // Felső sor
    for (short i = 0; i <= 3; i++) {
        tft.drawRoundRect(i * 120, 0, 114, BLOCK_HEIGHT, BLOCK_RADIUS, TFT_CYAN);
    }

#define HEADER_TEXT_Y 6
    tft.setFont();
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);

    tft.setCursor(10, HEADER_TEXT_Y);
    tft.print("Batt [V]");

    tft.setCursor(150, HEADER_TEXT_Y);
    tft.print("Sats");

    tft.setCursor(268, HEADER_TEXT_Y);
    tft.print("HDop");

    tft.setCursor(370, HEADER_TEXT_Y);
    tft.print("Temp [C]");

    // Alsó sor
#define LOWER_BLOCK_WIDTH 150
    tft.drawRoundRect(0, 250, LOWER_BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_RADIUS, TFT_CYAN);
    tft.setCursor(10, 252);
    tft.print("Date");

    tft.drawRoundRect(tft.width() - LOWER_BLOCK_WIDTH, 250, LOWER_BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_RADIUS, TFT_CYAN);
    tft.setCursor(345, 252);
    tft.print("Time");
}

/**
 *
 */
void drawValues() {
#define VALUES_Y 58

    tft.setFont(&FreeSerifBold12pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);

    // Batterry
    clearStrRect("88.8", 20, VALUES_Y);
    tft.printf("%*.1f", 1, (analogRead(A3) * 3.3f / (1 << 12)) * 3);

    // Satellites
    clearStrRect("88", 155, VALUES_Y);
    tft.printf("%*d", 2, gps.satellites.isValid() && gps.satellites.age() < 3000 ? gps.satellites.value() : 0);

    // HDop
    tft.setTextSize(1);
    clearStrRect("88.88", 265, VALUES_Y);
    tft.printf("%*.2f", 2, gps.satellites.isValid() && gps.hdop.age() < 3000 ? gps.hdop.hdop() : 0);

    // Temp
    tft.setTextSize(2);
    clearStrRect("88.8", 375, VALUES_Y);
    tft.printf("%*.1f", 1, analogReadTemp());

    tft.setTextSize(1);
    // Date
    if (gps.date.isValid() && gps.date.age() < 3000) {
        clearStrRect("8888-88-88", 15, 300);
        tft.printf("%04d-%02d-%02d", gps.date.year(), gps.date.month(), gps.date.day());
    }
    // Time
    if (gps.time.isValid() && gps.time.age() < 3000) {

        int hours = gps.time.hour();
        int mins = gps.time.minute();

        dls.correctTime(mins, hours, gps.date.day(), gps.date.month(), gps.date.year());

        tft.setTextSize(2);
        clearStrRect("88:88", 350, 310);
        tft.printf("%02d:%02d", hours, mins);
    }
}

//--- Core-0 --------------------------------------------------------------------------------------------
/**
 * Core-0 Setup
 */
void setup(void) {

    // UDB Serial - Console debug
#ifdef __DEBUG
    Serial.begin(115200);
#endif

    // TFT init
    tft.reset();

    ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);

    tft.begin(ID);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    analogReadResolution(12);

    drawDisplay();
}

/**
 * Core-0 Loop
 */
void loop(void) {

    while (true) {
        drawValues();
        displaySpeed(random(0, 299));
        delay(1000);
        // Serial.printf("screenWidth: %d", screenWidth);
        // Serial.printf("screenHeight: %d", screenHeight);
    }
}

//--- Core-1 --------------------------------------------------------------------------------------------
/**
 * Core-1 Setup
 */
void setup1(void) {
    // GPS Serial
    Serial1.begin(9600);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}
/**
 * Core-1 - GPS process
 */
void loop1(void) {

    while (true) {
        while (Serial1.available()) {

            if (gps.encode(Serial1.read())) {
                gpsDataReceivedLED();
                break;
            }
        }
    }
}