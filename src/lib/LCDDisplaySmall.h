#ifndef LCDDISPLAYSAMLL_H
#define LCDDISPLAYSAMLL_H

#include <WString.h>

#define I2C_SDA 21
#define I2C_SCL 22

class LCDDisplaySmall {
    public:
    LCDDisplaySmall();
    void load();
    void print();
    void printTop(String text);
    void printBottom(String text);
    void SetText(String text);
    bool GetBacklightState();
    void SetBacklightState(bool state);

    private:
    String text;
    float temperature;
    float humidity;
    bool backlight_on = true;
};

#endif