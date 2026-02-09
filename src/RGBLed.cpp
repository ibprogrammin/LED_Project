#include <Arduino.h>
#include "RGBLed.h"
#include "LightState.h"
#include "Adafruit_NeoPixel.h"

RGBLed::RGBLed(int num_leds, int led_pin) {
    this->num_leds = num_leds;
    this->led_pin = led_pin;

    load();
}

void RGBLed::load() {
    frequency = DEFAULT_FREQUENCY;

    strip = Adafruit_NeoPixel(num_leds, led_pin, NEO_GRB + NEO_KHZ800);

    strip.begin();                  // Initialize the NeoPixel library
    strip.show();                   // Turn off all the LEDs initially
}

void RGBLed::Run() {
    //if (frequency <= 25) frequency++;
    //else frequency = 5;

    switch(state) {
        case LightState::LIGHTS_OFF:
            off();
            break;
        case LightState::LIGHTS_ON:
            on();
            break;
        case LightState::CHASE:
            chase();
            break;
        case LightState::DIM_UP_DOWN:
            dimUpDown();
            break;
        case LightState::FLASH:
            flash();
            break;
        case LightState::STROBE:
            strobe();
            break;
        case LightState::TEMPERATURE_SENSITIVE:
            temperatureSensitive();
            break;
        case LightState::THERAPY:
            therapy();
            break;
        default:
            break;
    }
    
}

void RGBLed::SwitchState() {
    int current_state = (int)this->state;
    if (current_state < (int)LightState::MAX_STATE - 1) {
        current_state++;
    } else {
        current_state = 0;
    }

    this->state = (LightState)current_state;
}

void RGBLed::SetState(LightState light_state) {
    Serial.println("Setting Light State: " + String((int)light_state));
    this->state = light_state;
}

LightState RGBLed::GetState() {
    return this->state;
}

void RGBLed::SetFrequency(int frequency) {
    this->frequency = frequency;
}

void RGBLed::SetColorVal(RGBColor color_val) {
    this->color_val = color_val;
}

void RGBLed::SetColorVal(int red_val, int green_val, int blue_val) {
    this->color_val.red = red_val;
    this->color_val.green = green_val;
    this->color_val.blue = blue_val;
}

void RGBLed::SetNumLeds(int num_leds) {
    this->num_leds = num_leds;
}

void RGBLed::SetLedPin(int led_pin) {
    this->led_pin = led_pin;
}

void RGBLed::SetTemperature(float temperature) {
    this->temperature = temperature;
}

void RGBLed::off() {
    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, 0, 0, 0); // Set the color of the i-th LED to black (turn it off)
    }
    strip.show(); // Update the LED strip with the new colors
}

void RGBLed::on() {
    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, color_val.red, color_val.green, color_val.blue); // Set the color of the i-th LED to red
    }
    strip.show(); // Update the LED strip with the new colors
}

void RGBLed::chase() {
// Turn on LEDs one by one
  for (int i = 0; i < num_leds; i++) {
    strip.setPixelColor(i, color_val.red, color_val.green, color_val.blue); // Set the color of the i-th LED to red
    strip.show(); // Update the LED strip with the new colors
    vTaskDelay(100); // Wait for 100 milliseconds
  }
  
  // Turn off LEDs one by one
  for (int i = 0; i < num_leds; i++) {
    strip.setPixelColor(i, 0, 0, 0); // Set the color of the i-th LED to black (turn it off)
    strip.show(); // Update the LED strip with the new colors
    vTaskDelay(100); // Wait for 100 milliseconds
  }
}

void RGBLed::dimUpDown() {
    // Dim up LED strip
    for (int dim_value = 0; dim_value < 255; dim_value++) {
        for (int i = 0; i < num_leds; i++) {
            strip.setPixelColor(i, (int)floor(color_val.red * ((float)dim_value / 255)), (int)floor(color_val.green * ((float)dim_value / 255)), (int)floor(color_val.blue * ((float)dim_value / 255)) ); // Set the color of the i-th LED to red
        }
        strip.show(); // Update the LED strip with the new colors
        vTaskDelay(10); // Wait for 10 milliseconds
    }

    // Dim down LED strip
    for (int dim_value = 255; dim_value > 0; dim_value--) {
        for (int i = 0; i < num_leds; i++) {
            strip.setPixelColor(i, (int)floor(color_val.red * ((float)dim_value / 255)), (int)floor(color_val.green * ((float)dim_value / 255)), (int)floor(color_val.blue * ((float)dim_value / 255)) ); // Set the color of the i-th LED to red
        }
        strip.show(); // Update the LED strip with the new colors
        vTaskDelay(10); // Wait for 10 milliseconds
    }
}

void RGBLed::flash() {
    /*int flicker = floor((1000 / frequency) / 2);
    for (int i = 0; i < frequency; i++) {
        digitalWrite(LEDPIN, HIGH);
        delay(flicker);
        digitalWrite(LEDPIN, LOW);
        delay(flicker);
    }
    delay(200);*/

    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, 255, 255, 255); // Set the color of the i-th LED to red
    }
    strip.show(); // Update the LED strip with the new colors
    vTaskDelay(30); // Wait for 300 milliseconds

    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, 0, 0, 0); // Turn off the i-th LED
    }
    strip.show(); // Update the LED strip with the new colors
    vTaskDelay(2000); // Wait for 2 seconds
}

void RGBLed::strobe() {
    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, color_val.red, color_val.green, color_val.blue); // Set the color of the i-th LED to red
    }
    strip.show(); // Update the LED strip with the new colors
    vTaskDelay(100); // Wait for 300 milliseconds

    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, 0, 0, 0); // Turn off the i-th LED
    }
    strip.show(); // Update the LED strip with the new colors
    vTaskDelay(300); // Wait for 100 milliseconds
}

void RGBLed::temperatureSensitive() {
    // should go from blue (cold) to red (hot)
}

void RGBLed::therapy() {
    // should cycle through colors that are good for light therapy
}

void RGBLed::SetColorTemperature(int color_temp_kalvins) {
    switch (color_temp_kalvins) {
        case 0 ... 1000:
            SetColorVal(255, 56, 0); // Candlelight/Fire
            break;
        case 1001 ... 1900:
            SetColorVal(255, 147, 41); // Candlelight Tungsten
            break;
        case 1901 ... 2700:
            SetColorVal(255, 169, 87); // Soft white/Incandescent
            break;
        case 2701 ... 3000:
            SetColorVal(255, 180, 107); // Warm white
            break;
        case 3001 ... 4000:
            SetColorVal(255, 209, 163); // Neutral white
            break;
        case 4001 ... 5000:
            SetColorVal(255, 228, 206); // Daylight/Flourescent
            break;
        case 5001 ... 6500:
            SetColorVal(255, 249, 253); // Clear Daylight
            break;
        case 6501 ... 9000:
            SetColorVal(214, 255, 255); // Clear blue sky
            break;
        default:
            SetColorVal(255, 255, 255); // Default to pure white if color temperature is not recognized
            break;
    }
    SetState(LightState::LIGHTS_ON);
}