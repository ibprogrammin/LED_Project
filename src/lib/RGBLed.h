#ifndef RGBLED_H
#define RGBLED_H

#include "LightState.h"
#include "Adafruit_NeoPixel.h"

struct RGBColor {
    int red;
    int green;
    int blue;
};

class RGBLed {
    public:
    RGBLed(int num_leds, int led_pin);
    void load();
    void Run();
    void SetNumLeds(int num_leds);
    void SetLedPin(int led_pin);
    void SetFrequency(int frequency);
    void SwitchState();
    void SetState(LightState light_state);
    LightState GetState();
    void SetColorVal(RGBColor color_val);
    void SetColorVal(int red_val, int green_val, int blue_val);
    void SetTemperature(float temperature);
    void SetColorTemperature(int color_temp_kalvins);

    private:
    const int DEFAULT_FREQUENCY = 5;

    int frequency;
    LightState state;
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
    
    int num_leds;
    int led_pin;

    RGBColor color_val;
    float temperature;

    //static double dim_value = 0.0; // MAX 255
    //static int increment = 0;
    //static int counter = 0;
};


#endif