#ifndef __ANALOGMETER_H__
#define __ANALOGMETER_H__

#include <MCUFRIEND_kbv.h>

class AnalogMeter {

private:
    MCUFRIEND_kbv *tft;

    int old_analog = -999; // Value last displayed
    float ltx = 0;         // Saved x coord of bottom of needle
    uint16_t osx = 120;    // Saved x & y coords
    uint16_t osy = 120;    // Saved x & y coords

public:
    AnalogMeter(MCUFRIEND_kbv *_tft) {
        tft = _tft;
    }
    void draw();
    void plotNeedle(int value, byte ms_delay);
};

#endif //__ANALOGMETER_H__