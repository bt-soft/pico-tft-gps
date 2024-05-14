#include <Arduino.h>
#include <Streaming.h>
#include <stdio.h>

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
uint16_t ID;
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>

#include <TinyGPS++.h>
TinyGPSPlus gps;

// Hőmérés
#define PIN_TEMP_SENSOR 2         /* ATmega328P PIN:4, D10 a DS18B20 bemenete */
#define DS18B20_TEMP_SENSOR_NDX 0 /* Dallas DS18B20 hõmérõ szenzor indexe */
#include <OneWire.h>
#define REQUIRESALARMS false /* nem kell a DallasTemperature ALARM supportja */
#include <DallasTemperature.h>
DallasTemperature ds18B20(new OneWire(PIN_TEMP_SENSOR));

#include "DayLightSaving.h"
DaylightSaving dls;

#include "commons.h"
#include "graphUtils.h"

#define AD_RESOLUTION 10

/**
 * DS18B20 digitális hőmérő szenzor olvasása
 */
float read_DS18B20_Temp(void) {
    ds18B20.requestTemperaturesByIndex(DS18B20_TEMP_SENSOR_NDX);
    return ds18B20.getTempCByIndex(DS18B20_TEMP_SENSOR_NDX);
}

float battVoltage() {
#define V_REF 3.3f
#define ATTENNUATOR ((33.16f + 9.957f) / 9.957f)
#define AD_RES (1 << AD_RESOLUTION)

    // ADC érték átalakítása feszültséggé
    float voltageOut = (analogRead(A3) * V_REF) / AD_RES;
    voltageOut -= 0.11; // csalunk egyet, ennyire nem lenne pontos az AD??
    Serial << "Vout: " << voltageOut << endl;

    // Eredeti feszültség számítása a feszültségosztó alapján
    return voltageOut * ATTENNUATOR;
}

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

    tft.setCursor(260, HEADER_TEXT_Y);
    tft.print("Alt[m]");

    tft.setCursor(370, HEADER_TEXT_Y);
    tft.print("Temp [C]");

    // // Körgyűrű
    // int w = 50;
    // int rx = 160;
    // int ry = 100;
    // int startAngle = 300;
    //
    // for (int n = 0; n < 5; n++) {
    //     // fillArc2(tft.width() / 2, 200, 300, 40, rx - n * w, ry - n * w, w, 31 - n * 6);
    //      fillArc2(tft.width() / 2, 200, 300, 40, rx - n * w, ry - n * w, w, TFT_YELLOW);
    // }
    // fillArc2(tft.width() / 2, 200, startAngle, 10, rx - 0 * w, ry - 0 * w, w, TFT_YELLOW);
    // fillArc2(tft.width() / 2, 200, startAngle + 11, 20, rx - 0 * w, ry - 0 * w, w, TFT_ORANGE);
    // fillArc2(tft.width() / 2, 200, startAngle + 12 + 20, 30, rx - 0 * w, ry - 0 * w, w, TFT_RED);

    // Alsó sor
#define LOWER_BLOCK_WIDTH 150
    tft.drawRoundRect(0, 250, LOWER_BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_RADIUS, TFT_CYAN);
    tft.setCursor(10, 254);
    tft.print("Date");

    tft.drawRoundRect(tft.width() - LOWER_BLOCK_WIDTH, 250, LOWER_BLOCK_WIDTH, BLOCK_HEIGHT, BLOCK_RADIUS, TFT_CYAN);
    tft.setCursor(345, 254);
    tft.print("Time");

    tft.setFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(220, 240);
    tft.print("km/h");
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
    tft.printf("%*.1f", 1, battVoltage());
    // tft.printf("%*.1f", 1, 14.44f);

    // Satellites
    clearStrRect("88", 155, VALUES_Y);
    tft.printf("%*d", 2, gps.satellites.isValid() && gps.satellites.age() < 3000 ? gps.satellites.value() : 0);

    // Alt
    clearStrRect("8888", 260, VALUES_Y);
    tft.printf("%*.0f", 0, gps.satellites.isValid() && gps.altitude.age() < 3000 ? gps.altitude.meters() : 0);

    // Temp
    float temp = read_DS18B20_Temp();
    clearStrRect("88.8", 390, VALUES_Y);
    tft.printf("%*.1f", 1, temp);

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

        tft.setFont(&FreeSansBold12pt7b);
        tft.setTextSize(2);
        clearStrRect("88:88", 350, 310);
        tft.printf("%02d:%02d", hours, mins);
    }

#define LOC_SPEED_X 140
#define LOC_SPEED_Y 200
    tft.setFont(&FreeSerifBold24pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3);
    clearStrRect("888", LOC_SPEED_X, LOC_SPEED_Y);
    tft.printf("%*.0f", 3, gps.speed.isValid() && gps.speed.age() < 3000 && gps.speed.kmph() > 1 ? gps.speed.kmph() : 0);
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

    analogReadResolution(AD_RESOLUTION);

    // Hőmérés felhúzása
    ds18B20.begin();
    ds18B20.setResolution(12);
    ds18B20.setWaitForConversion(false);

    drawDisplay();
}

/**
 * Core-0 Loop
 */
void loop(void) {

    while (true) {
        drawValues();
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