#include <Arduino.h>
#include "RGBLed.h"
#include "LightState.h"
#include "Adafruit_NeoPixel.h"

// Constructor for the RGBLed class, initializes the number of LEDs and the pin they are connected to
RGBLed::RGBLed(int num_leds, int led_pin) {
    this->num_leds = num_leds;
    this->led_pin = led_pin;

    load();
}

// Load function to initialize the NeoPixel library and set up the LED strip
void RGBLed::load() {
    frequency = DEFAULT_FREQUENCY;

    strip = Adafruit_NeoPixel(num_leds, led_pin, NEO_GRB + NEO_KHZ800);

    strip.begin();                  // Initialize the NeoPixel library
    strip.show();                   // Turn off all the LEDs initially
}

// Run function to update the LED strip based on the current state
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

// Function to set the color temperature of the LEDs based on a given color temperature in kalvins
void RGBLed::SetNumLeds(int num_leds) {
    this->num_leds = num_leds;
}

// Function to set the pin number that the LED strip is connected to
void RGBLed::SetLedPin(int led_pin) {
    this->led_pin = led_pin;
}

// Function to switch to the next light state in a cyclic manner
void RGBLed::SwitchState() {
    int current_state = (int)this->state;
    if (current_state < (int)LightState::MAX_STATE - 1) {
        current_state++;
    } else {
        current_state = 0;
    }

    this->state = (LightState)current_state;
}

// Function to set the current light state
void RGBLed::SetState(LightState light_state) {
    Serial.println("Setting Light State: " + String((int)light_state));
    this->state = light_state;
}

// Function to get the current light state
LightState RGBLed::GetState() {
    return this->state;
}

// Function to set the frequency of the light effects (if applicable)
void RGBLed::SetFrequency(int frequency) {
    this->frequency = frequency;
}

// Function to set the color value of the LEDs using an RGBColor struct
void RGBLed::SetColorVal(RGBColor color_val) {
    this->color_val = color_val;
}

// Function to set the color value of the LEDs using individual red, green, and blue values
void RGBLed::SetColorVal(int red_val, int green_val, int blue_val) {
    this->color_val.red = red_val;
    this->color_val.green = green_val;
    this->color_val.blue = blue_val;
}

// Function to set the color temperature of the LEDs based on a given color temperature in kalvins
void RGBLed::SetTemperature(float temperature) {
    this->temperature = temperature;
}

// Function to set the LED strip in to the off state
void RGBLed::off() {
    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, 0, 0, 0); // Set the color of the i-th LED to black (turn it off)
    }
    strip.show(); // Update the LED strip with the new colors
}

// Function to set the LED strip in to the on state with the current color value
void RGBLed::on() {
    for (int i = 0; i < num_leds; i++) {
        strip.setPixelColor(i, color_val.red, color_val.green, color_val.blue); // Set the color of the i-th LED to red
    }
    strip.show(); // Update the LED strip with the new colors
}

// Function to create a chase effect by turning on the LEDs one by one and then turning them off one by one
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

// Function to create a dim up and down effect by gradually increasing and decreasing the brightness of the LEDs
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

// Function to create a flash effect by quickly turning on and off the LEDs
void RGBLed::flash() {
    

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

// Function to create a strobe effect by quickly turning on and off the LEDs in a loop
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

// Function to create a temperature sensitive effect by changing the color of the LEDs based on the current temperature
void RGBLed::temperatureSensitive() {
    // Color will go from blue (cold) to red (hot)
    RGBColor temp_color;
    if (temperature <= -15) {
        temp_color = {0, 0, 255}; 
    } else if (temperature > -15 && temperature <= -13) {
        temp_color = {0, 51, 255};
    } else if (temperature > -13 && temperature <= -10) {
    temp_color = {0, 102, 255};   
    } else if (temperature > -10 && temperature <= -7) {
    temp_color = {0, 153, 255}; 
    } else if (temperature > -7 && temperature <= -5) {
        temp_color = {0, 204, 255}; 
    } else if (temperature > -5 && temperature <= -3) {
        temp_color = {0, 255, 255};
    } else if (temperature > -3 && temperature <= 0) {
        temp_color = {51, 255, 204};
    } else if (temperature > 0 && temperature <= 3) {
        temp_color = {102, 255, 153};
    } else if (temperature > 3 && temperature <= 5) {
        temp_color = {153, 255, 102};
    } else if (temperature > 5 && temperature <= 7) {
        temp_color = {204, 255, 51};
    } else if (temperature > 7 && temperature <= 10) { 
        temp_color = {255, 255, 0};
    } else if (temperature > 10 && temperature <= 13) {
        temp_color = {255, 229, 0};
    } else if (temperature > 13 && temperature <= 15) {
        temp_color = {255, 204, 0};
    } else if (temperature > 15 && temperature <= 17) {
        temp_color = {255, 178, 0};
    } else if (temperature > 17 && temperature <= 20) {
        temp_color = {255, 153, 0};
    } else if (temperature > 20 && temperature <= 23) {
        temp_color = {255, 127, 0};
    } else if (temperature > 23 && temperature <= 25) {
        temp_color = {255, 102, 0};
    } else if (temperature > 25 && temperature <= 27) {
        temp_color = {255, 76, 0};
    } else if (temperature > 27 && temperature <= 30) {
        temp_color = {255, 51, 0};
    } else if (temperature > 30 && temperature <= 33) { 
        temp_color = {255, 25, 0};
    } else if (temperature > 33 && temperature <= 35) {
        temp_color = {255, 0, 0};
    } else if (temperature > 35 && temperature <= 37) {
        temp_color = {230, 0, 0};
    } else if (temperature > 37 && temperature <= 40) {
        temp_color = {204, 0, 0};
    } else {
        temp_color = {179, 0, 0};
    }

    for (int j = 0; j < num_leds; j++) strip.setPixelColor(j, temp_color.red, temp_color.green, temp_color.blue); // Set the j-th LED to the temperature-sensitive color
    strip.show(); // Update the LED strip with the new colors
    vTaskDelay(500);
}

void RGBLed::temperatureSensitiveV2() {
    // Define temperature bounds
    const float MIN_TEMP = -20.0f;
    const float MAX_TEMP = 45.0f;
    
    // Clamp temperature to valid range
    float clamped_temp = constrain(temperature, MIN_TEMP, MAX_TEMP);
    
    // Calculate normalized value 0.0 (cold) to 1.0 (hot)
    float ratio = (clamped_temp - MIN_TEMP) / (MAX_TEMP - MIN_TEMP);
    
    // Interpolate between blue (cold) and red (hot)
    uint8_t red   = (uint8_t)(255 * ratio);          // 0 at cold, 255 at hot
    uint8_t green = (uint8_t)(255 * (1 - abs(2*ratio - 1))); // Peak in middle for gradient
    uint8_t blue  = (uint8_t)(255 * (1 - ratio));    // 255 at cold, 0 at hot
    
    for (int j = 0; j < num_leds; j++) {
        strip.setPixelColor(j, red, green, blue);
    }
    strip.show();
    vTaskDelay(500);
}

// Function to create a therapy effect by cycling through colors that are good for light therapy
void RGBLed::therapy() {
    // should cycle through colors that are good for light therapy
    if (frequency <= 25) frequency++;
    else frequency = 5;

    int flicker = floor((1000 / frequency) / 2);
    for (int i = 0; i < frequency; i++) {
        for (int j = 0; j < num_leds; j++) strip.setPixelColor(j, 255, 255, 255); // Set the j-th LED to white
        strip.show(); // Update the LED strip with the new colors
        vTaskDelay(flicker);
        for (int j = 0; j < num_leds; j++) strip.setPixelColor(j, 0, 0, 0); // Turn off the j-th LED
        strip.show(); // Update the LED strip with the new colors
        vTaskDelay(flicker);
    }
    //vTaskDelay(200);
}

// Function to set the color temperature of the LEDs based on a given color temperature in kalvins
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