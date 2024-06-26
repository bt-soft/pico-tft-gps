#include "AnalogMeter.h"

// #########################################################################
//  Draw the analogue meter on the screen
// #########################################################################
void AnalogMeter::draw() {
    // Meter outline
    tft->fillRect(0, 0, 239 + 10, 126, TFT_DARKGREY);
    tft->fillRect(5, 3, 230 + 10, 119, TFT_WHITE);
    tft->drawRect(5, 3, 230 + 10, 119, TFT_BLACK); // Draw bezel line

    tft->setTextColor(TFT_BLACK); // Text colour

    // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
    for (int i = -50; i < 51; i += 5) {

        // Long scale tick length
        int tl = 15;

        // Coodinates of tick to draw
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (100 + tl) + 120;
        uint16_t y0 = sy * (100 + tl) + 140;
        uint16_t x1 = sx * 100 + 120;
        uint16_t y1 = sy * 100 + 140;

        // Coordinates of next tick for zone fill
        float sx2 = cos((i + 5 - 90) * 0.0174532925);
        float sy2 = sin((i + 5 - 90) * 0.0174532925);
        int x2 = sx2 * (100 + tl) + 120;
        int y2 = sy2 * (100 + tl) + 140;
        int x3 = sx2 * 100 + 120;
        int y3 = sy2 * 100 + 140;

        // // Yellow zone limits
        // if (i >= -50 && i < 25) {
        //     tft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
        //     tft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
        // }

        // Green zone limits
        if (i >= 0 && i < 25) {
            tft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
            tft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
        }

        // Red zone limits
        if (i >= 25 && i < 50) {
            tft->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
            tft->fillTriangle(x1, y1, x2, y2, x3, y3, TFT_RED);
        }

        // Short scale tick length
        if (i % 25 != 0) {
            tl = 8;
        }

        // Recalculate coords incase tick lenght changed
        x0 = sx * (100 + tl) + 120;
        y0 = sy * (100 + tl) + 140;
        x1 = sx * 100 + 120;
        y1 = sy * 100 + 140;

        // Draw tick
        tft->drawLine(x0, y0, x1, y1, TFT_BLACK);

        // Check if labels should be drawn, with position tweaks
        if (i % 25 == 0) {
            // Calculate label positions
            x0 = sx * (100 + tl + 10) + 120;
            y0 = sy * (100 + tl + 10) + 140;
            tft->setTextSize(2);
            switch (i / 25) {
            case -2:
                tft->setCursor(x0, y0 - 12);
                tft->print("0");
                break;
            case -1:
                tft->setCursor(x0, y0 - 9);
                tft->print("25");
                break;
            case 0:
                tft->setCursor(x0, y0 - 6);
                tft->print("50");
                break;
            case 1:
                tft->setCursor(x0, y0 - 9);
                tft->print("75");
                break;
            case 2:
                tft->setCursor(x0 - 10, y0 - 12);
                tft->print("100");
                break;
            }
        }

        // Now draw the arc of the scale
        sx = cos((i + 5 - 90) * 0.0174532925);
        sy = sin((i + 5 - 90) * 0.0174532925);
        x0 = sx * 100 + 120;
        y0 = sy * 100 + 140;

        // Draw scale arc, don't draw the last part
        if (i < 50) {
            tft->drawLine(x0, y0, x1, y1, TFT_BLACK);
        }
    }

    //    tft->drawString("%RH", 5 + 230 - 40, 119 - 20, 2); // Units at bottom right
    // tft->drawCentreString("%RH", 120, 70, 4);          // Comment out to avoid font 4

    plotNeedle(0, 0); // Put meter needle at 0
}

/**
 * Update needle position
 * This function is blocking while needle moves, time depends on ms_delay
 * 10ms minimises needle flicker if text is drawn within needle sweep area
 * Smaller values OK if text not in sweep area, zero for instant movement but
 * does not look realistic... (note: 100 increments for full scale deflection)
 */
void AnalogMeter::plotNeedle(int value, byte ms_delay) {

    if (value < -10) {
        value = -10; // Limit value to emulate needle end stops
    }

    if (value > 110) {
        value = 110;
    }

    // Move the needle util new value reached
    while (value != old_analog) {

        if (old_analog < value) {
            old_analog++;
        } else {
            old_analog--;
        }

        if (ms_delay == 0) {
            old_analog = value; // Update immediately id delay is 0
        }

        float sdeg = map(old_analog, -10, 110, -150, -30); // Map value to angle
        // Calcualte tip of needle coords
        float sx = cos(sdeg * 0.0174532925);
        float sy = sin(sdeg * 0.0174532925);

        // Calculate x delta of needle start (does not start at pivot point)
        float tx = tan((sdeg + 90) * 0.0174532925);

        // Erase old needle image
        tft->drawLine(120 + 20 * ltx - 1, 140 - 20, osx - 1, osy, TFT_WHITE);
        tft->drawLine(120 + 20 * ltx, 140 - 20, osx, osy, TFT_WHITE);
        tft->drawLine(120 + 20 * ltx + 1, 140 - 20, osx + 1, osy, TFT_WHITE);

        // Re-plot text under needle
        // tft->setTextColor(TFT_BLACK);
        // tft->drawCentreString("%RH", 120, 70, 4); // // Comment out to avoid font 4

        // Store new needle end coords for next erase
        ltx = tx;
        osx = sx * 98 + 120;
        osy = sy * 98 + 140;

        // Draw the needle in the new postion, magenta makes needle a bit bolder
        // draws 3 lines to thicken needle
        tft->drawLine(120 + 20 * ltx - 1, 140 - 20, osx - 1, osy, TFT_RED);
        tft->drawLine(120 + 20 * ltx, 140 - 20, osx, osy, TFT_MAGENTA);
        tft->drawLine(120 + 20 * ltx + 1, 140 - 20, osx + 1, osy, TFT_RED);

        // Slow needle down slightly as it approaches new postion
        if (abs(old_analog - value) < 10) {
            ms_delay += ms_delay / 5;
        }

        // érték kiírása
        char buf[4];
        dtostrf(value, 3, 0, buf);
        tft->setTextColor(TFT_GREEN, TFT_DARKGREY);
        tft->setCursor(85, 119 - 50);
        tft->setTextSize(4);
        tft->print(buf);

        // Wait before next update
        delay(ms_delay);
    }

#include "AnalogMeter.h"
    AnalogMeter analogMeter = AnalogMeter(&tft);

    void test(void) {
        analogMeter.draw();                        // Draw analogue meter
        analogMeter.plotNeedle(random(0, 100), 6); // Update analogue meter, xms delay per needle increment
    }
}