#include <Arduino.h>
#include <Streaming.h>
#include <stdio.h>

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
uint16_t ID;

#include <TinyGPS++.h>
TinyGPSPlus gps;

#include "AnalogMeter.h"
AnalogMeter analogMeter = AnalogMeter(&tft);

const uint16_t screenWidth = tft.height(); // landscape
const uint16_t screenHeight = tft.width();

/**
 * Core-0 Setup
 */
void setup(void) {

    // UDB Serial - Console debug
    Serial.begin(115200);

    // TFT init
    tft.reset();

    ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);

    tft.begin(ID);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    analogMeter.draw(); // Draw analogue meter
}

/**
 * Core-0 Loop
 */
void loop(void) {

    while (true) {
        delay(1000);
        Serial.println("\n--- main thread----------");

        analogMeter.plotNeedle(random(0, 100), 6); // Update analogue meter, xms delay per needle increment
    }
}

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
    long ledChanged;

    while (true) {
        while (Serial1.available()) {

            if (gps.encode(Serial1.read())) {
                Serial << "GPS data!!" << endl;
                digitalWrite(LED_BUILTIN, LOW);
                ledChanged = millis();
                break;
            }
        }
        if (ledChanged > 0 && millis() - ledChanged >= 1000) {
            digitalWrite(LED_BUILTIN, HIGH);
            ledChanged = 0L;
        }
    }
}