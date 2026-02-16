#ifndef RGBLED_H
#define RGBLED_H

#include "LightState.h"
#include "Adafruit_NeoPixel.h"
#include <string>
#include <vector>
#include "RGBColor.h"

static const std::vector<std::string> LED_MODE_STRINGS = {
    "System Ready",
    "Off Mode",
    "On Mode",
    "Chase Mode",
    "Dim Up/Down Mode",
    "Flash Mode",
    "Strobe Mode",
    "Temperature Mode",
    "Therapy Mode",
    "Invalid Mode"
};

class RGBLed {
    public:
    RGBLed(int num_leds, int led_pin);
    void init();
    void run();
    // TODO this should move to a BLEMessageHandler class
    void handleBLEMessage(String message);
    void setNumLeds(int num_leds);
    void setLedPin(int led_pin);
    void setFrequency(int frequency);
    void switchState();
    void setState(LightEffect light_state);
    LightEffect getState();
    void setColorVal(RGBColor color_val);
    void setColorVal(int red_val, int green_val, int blue_val);
    void setTemperature(float temperature);
    void setColorTemperature(int color_temp_kalvins);

    private:
    const int DEFAULT_FREQUENCY = 5;

    int frequency;
    LightEffect state;
    Adafruit_NeoPixel strip;

    void off();
    void on();
    void chase();
    void dimUpDown();
    void flash();
    void strobe();
    void temperatureSensitive();
    void temperatureSensitiveV2();
    void therapy();
    void rainbowChase();
    void rainbowDimUpDown();
    
    int num_leds;
    int led_pin;

    RGBColor color_val;
    float temperature;

    //static double dim_value = 0.0; // MAX 255
    //static int increment = 0;
    //static int counter = 0;
};


#endif