#include <Arduino.h>
#include <Streaming.h>
#include <stdio.h>

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
uint16_t ID;

#include <TinyGPSPlus.h>
TinyGPSPlus gps;

/**
 * GPIO29-es lábon lógó 3-as csatornára kötött AD olvasása
 */
// x-bit conversion, assume max value == ADC_VREF == 3.3 V
#define AD_RESOLUTION 10
const float conversion_factor = 3.3f / (1 << AD_RESOLUTION);
float readVsys() {
    int rawVsys = analogRead(A3);                       // VSYS feszültség értékének olvasása (GPIO29-re -> A3 van kötve 3-as ADC csatorna)
    float voltage = rawVsys * conversion_factor * 3.0f; // *3 mert 1/3-as osztóra van kötve
    Serial << "rawVsys: " << rawVsys << ", voltage:" << voltage << endl;

    return voltage;
}

/**
 *
 */
void setup(void) {

    // 'AD_RESOLUTION' bites módba kapcsoljuk az ADC-t (default esetben 10bites módban van)
    analogReadResolution(AD_RESOLUTION);

    Serial.begin(115200);

    tft.reset();
    ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    tft.begin(ID);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
}

/**
 *
 */
void loop(void) {

    while (true) {
        delay(2000);
        Serial.println("\n--- main thread----------");

        // Serial.print(F("Voltage: "));
        Serial.println(readVsys());
        Serial.println("\n--- main thread----------");
    }
}

/**
 *
 */
void setup1(void) {
    Serial1.begin(9600);
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}
/**
 *
 */
void loop1(void) {
    char incoming_byte = 0;

    while (true) {
        while (Serial1.available()) {
            incoming_byte = Serial1.read();
            Serial << incoming_byte;

            if (incoming_byte == '\n') {
                digitalWrite(LED_BUILTIN, LOW);
            } else {
                digitalWrite(LED_BUILTIN, HIGH);
            }
        }
    }
}