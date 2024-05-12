#include <Arduino.h>
#include <Streaming.h>
#include <stdio.h>

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
uint16_t ID;
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
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
#define LOC_SPEED_X 170
#define LOC_SPEED_Y 280
void displaySpeed(int speed) {
    tft.setFont(&FreeSerifBold24pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    clearStrRect("888", LOC_SPEED_X, LOC_SPEED_Y);
    tft.printf("%*s", 3, String(speed).c_str());
}

// #########################################################################
// Draw a circular or elliptical arc with a defined thickness
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same then an arc of a circle is drawn
#define DEG2RAD 0.0174532925
void fillArc2(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour) {

    byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
    byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring

    // Calculate first pair of coordinates for segment start
    float sx = cos((start_angle - 90) * DEG2RAD);
    float sy = sin((start_angle - 90) * DEG2RAD);
    uint16_t x0 = sx * (rx - w) + x;
    uint16_t y0 = sy * (ry - w) + y;
    uint16_t x1 = sx * rx + x;
    uint16_t y1 = sy * ry + y;

    // Draw colour blocks every inc degrees
    for (int i = start_angle; i < start_angle + seg * seg_count; i += inc) {

        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * DEG2RAD);
        float sy2 = sin((i + seg - 90) * DEG2RAD);
        int x2 = sx2 * (rx - w) + x;
        int y2 = sy2 * (ry - w) + y;
        int x3 = sx2 * rx + x;
        int y3 = sy2 * ry + y;

        tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
        tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);

        // Copy segment end to sgement start for next segment
        x0 = x2;
        y0 = y2;
        x1 = x3;
        y1 = y3;
    }
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

    // Körgyűrű
    int w = 50;
    int rx = 160;
    int ry = 100;
    int startAngle = 300;

    // for (int n = 0; n < 5; n++) {
    //     // fillArc2(tft.width() / 2, 200, 300, 40, rx - n * w, ry - n * w, w, 31 - n * 6);
    //      fillArc2(tft.width() / 2, 200, 300, 40, rx - n * w, ry - n * w, w, TFT_YELLOW);
    // }
    fillArc2(tft.width() / 2, 200, startAngle, 10, rx - 0 * w, ry - 0 * w, w, TFT_YELLOW);
    fillArc2(tft.width() / 2, 200, startAngle + 11, 20, rx - 0 * w, ry - 0 * w, w, TFT_ORANGE);
    fillArc2(tft.width() / 2, 200, startAngle + 12 + 20, 30, rx - 0 * w, ry - 0 * w, w, TFT_RED);

    // Alsó sor
#define LOWER_BLOCK_WIDTH 150
    tft.drawRoundRect(0, 250, LOWER_BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_RADIUS, TFT_CYAN);
    tft.setCursor(10, 254);
    tft.print("Date");

    tft.drawRoundRect(tft.width() - LOWER_BLOCK_WIDTH, 250, LOWER_BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_RADIUS, TFT_CYAN);
    tft.setCursor(345, 254);
    tft.print("Time");
}

/**
 *
 */
void drawValues() {
#define VALUES_Y 58

    tft.setTextColor(TFT_WHITE);
    tft.setFont(&FreeSansBold18pt7b);
    tft.setTextSize(1);

    // Batterry
    clearStrRect("88.8", 20, VALUES_Y);
    // tft.printf("%*.1f", 1, (analogRead(A3) * 3.3f / (1 << 12)) * 3);
    tft.printf("%*.1f", 1, 14.44f);

    // Satellites
    clearStrRect("88", 155, VALUES_Y);
    tft.printf("%*d", 2, gps.satellites.isValid() && gps.satellites.age() < 3000 ? gps.satellites.value() : 0);

    // HDop
    clearStrRect("88.88", 260, VALUES_Y);
    tft.printf("%*.2f", 2, gps.satellites.isValid() && gps.hdop.age() < 3000 ? gps.hdop.hdop() : 0);

    // Temp
    clearStrRect("88.8", 390, VALUES_Y);
    tft.printf("%*.1f", 1, analogReadTemp());

    // Date
    if (gps.date.isValid() && gps.date.age() < 3000) {
        tft.setFont(&FreeSansBold12pt7b);
        tft.setTextSize(1);
        clearStrRect("8888-88-88", 15, 300);
        tft.printf("%04d-%02d-%02d", gps.date.year(), gps.date.month(), gps.date.day());
    }
    // Time
    if (gps.time.isValid() && gps.time.age() < 3000) {

        int hours = gps.time.hour();
        int mins = gps.time.minute();

        dls.correctTime(mins, hours, gps.date.day(), gps.date.month(), gps.date.year());

        tft.setFont(&FreeSerifBold24pt7b);
        tft.setTextSize(1);
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