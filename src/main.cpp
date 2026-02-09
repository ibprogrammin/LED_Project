#include <Arduino.h>
#include <ezButton.h>
#include "DHT.h"
#include "lib\LCDDisplaySmall.h"
#include "lib\RGBLed.h"
#include "lib\LightState.h"
#include "lib\BLEServiceAdapter.h"

#define DHTPIN 14  // Set the pin connected to the DHT11 data pin
#define DHTTYPE DHT11 // DHT 11 
#define LED_PIN 19 // NeoPixel LED strip
#define NUM_LEDS 8 // Number of LEDs

// Constants for the potentiometer pins
#define RED_POT_PIN 34
#define GREEN_POT_PIN 35
#define BLUE_POT_PIN 39

// Constants for the Button Pins
#define RED_BUTTON_PIN 27
#define GREEN_BUTTON_PIN 26
#define BLUE_BUTTON_PIN 25
#define YELLOW_BUTTON_PIN 33

// Initialize buttons
ezButton redButton(RED_BUTTON_PIN);
ezButton greenButton(GREEN_BUTTON_PIN);
ezButton blueButton(BLUE_BUTTON_PIN);
ezButton yellowButton(YELLOW_BUTTON_PIN);

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

const int STcp = 27;//ST_CP
const int SHcp = 26;//SH_CP 
const int DS = 25; //DS 

// Array to hold the 7-segment display values for digits 0-9 and letters A-Z
//0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ
int datArray[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
0x77,0x7c,0x39,0x5e,0x79,0x71,0x3D,0x76,0x30,0x1e,0xf2,0x38,0x55,
0x54,0x5c,0x73,0x67,0xf7,0xed,0x78,0x3e,0x1c,0x1D,0x64,0x6e,0x5B};

// Create instances of the classes for the LED strip, LCD display, and BLE service adapter
RGBLed* led_strip;            // Create an instance of the RGBLed class
LCDDisplaySmall* lcd_display;  // Create an instance of the LCDDisplaySmall class 
BLEServiceAdapter* ble_service;  // Create an instance of the BLEServiceAdapter class

RGBColor color_value; // Create an instance of the RGBColor struct to hold the current color values 

float last_temperature = 0.0;
int color_temp_kalvins = 0;
static int led_frequency = 5;
//static double dim_value = 0.0; // MAX 255
static int increment = 0;
static int counter = 0;

// put function declarations here:
void initSevenSegment();
void printSevenSegment();
void retrieveColors();
int readPot(int pot_pin);
void readTemperature();
void readButtonState();

// Task wrapper functions
void mainTaskWrapper(void* parameter);
void buttonTaskWrapper(void* parameter);
void lcdTaskWrapper(void* parameter);
void ledTaskWrapper(void* parameter);

// Task handles for the different tasks
TaskHandle_t MainTaskHandle = NULL;
TaskHandle_t ButtonTaskHandle = NULL;
TaskHandle_t LCDTaskHandle = NULL;
TaskHandle_t RGBLedTaskHandle = NULL;

void setup() {
  Serial.begin(115200);
  delay(1000);

  ble_service = new BLEServiceAdapter();
  ble_service->load();

  xTaskCreatePinnedToCore(mainTaskWrapper, "MainTask", 10000, NULL, 1, &MainTaskHandle, 1);
  xTaskCreatePinnedToCore(buttonTaskWrapper, "ButtonTask", 10000, NULL, 1, &ButtonTaskHandle, 1);
  xTaskCreatePinnedToCore(lcdTaskWrapper, "LCDDisplayTask", 10000, NULL, 1, &LCDTaskHandle, 1);
  xTaskCreatePinnedToCore(ledTaskWrapper, "LEDTask", 10000, NULL, 1, &RGBLedTaskHandle, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*double dim_value = (increment ^ 2);
  increment++;
  if (increment == 255) {
    dim_value = 0;
    increment = 0;
  }*/

  /*Serial.print(increment);
  Serial.print("/");
  Serial.print(dim_value);
  Serial.println(" ");*/
  //analogWrite(15, (int)floor(dim_value));
  //delay(33);
}

void mainTaskWrapper(void* parameter) {
  Serial.println("DHT11 test!");
  dht.begin();

  while (true) {

    if (counter == 25) {
      //Serial.printf("Main Task Stack Free: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));

      counter = 0;
    } else {
      counter++;
    }
    
    readTemperature();
    vTaskDelay(3000);
  }
}

void buttonTaskWrapper(void* parameter) {
  //Setup buttons
  redButton.setDebounceTime(20);
  greenButton.setDebounceTime(20);
  blueButton.setDebounceTime(20);
  yellowButton.setDebounceTime(20);

  while (true) {
    readButtonState();
    vTaskDelay(50);
  }
}

void lcdTaskWrapper(void* parameter) {
  lcd_display = new LCDDisplaySmall();
  lcd_display->load();
  lcd_display->printTop("Off Mode        ");

  while (true) {
    //lcd_display->print();
    vTaskDelay(1000);
  }
}

void ledTaskWrapper(void* parameter) {
  led_strip = new RGBLed(NUM_LEDS, LED_PIN);
  led_strip->SetState(LightState::LIGHTS_OFF);    // Set the desired light state (e.g., CHASE, DIM_UP_DOWN, etc.)
  
  while (true) {
    // Check if the color temperature mode is active, if not retrieve the current color values from the potentiometers and update the LED strip accordingly  
    if (color_temp_kalvins == 0) {
      retrieveColors();
      led_strip->SetColorVal(color_value);
    }
    led_strip->Run();

    //Serial.printf("LED task Stack Free: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  }
}

void readButtonState() {
  redButton.loop();
  greenButton.loop();
  blueButton.loop();
  yellowButton.loop();

  // get button state after debounce
  int button1_state = redButton.getState();  // the state after debounce
  int button2_state = greenButton.getState();  // the state after debounce
  int button3_state = blueButton.getState();  // the state after debounce
  int button4_state = yellowButton.getState();  // the state after debounce

  /*
  Serial.print("Red button state: ");
  Serial.println(button1_state);
  Serial.print("Green button state: ");
  Serial.println(button2_state);
  Serial.print("Blue button state: ");
  Serial.println(button3_state);
  Serial.print("Yellow button state: ");
  Serial.println(button4_state);
  */

  if (redButton.isReleased()) {
    vTaskDelay(100); // Add a small delay to ensure the state change is registered before updating the LCD display
    
    color_temp_kalvins = 0;

    led_strip->SwitchState();
    switch (led_strip->GetState()) {
      case LightState::LIGHTS_OFF:
        lcd_display->printTop("Off Mode        ");
        break;
      case LightState::LIGHTS_ON:
        lcd_display->printTop("On Mode         ");
        break;
      case LightState::CHASE:
        lcd_display->printTop("Chase Mode      ");
        break;
      case LightState::DIM_UP_DOWN:
        lcd_display->printTop("Dim Up/Down Mode");
        break;
      case LightState::FLASH:
        lcd_display->printTop("Flash Mode      ");
        break;
      case LightState::STROBE:
        lcd_display->printTop("Strobe Mode     ");
        break;
      case LightState::TEMPERATURE_SENSITIVE:
        lcd_display->printTop("Temp Sensitive  ");
        break;
      case LightState::THERAPY:
        lcd_display->printTop("Therapy Mode    ");
        break;
      default:
        break;
    }
    //lcd_display->print();

    Serial.println("Red button released post exec");
  } else if (greenButton.isReleased()) {
      vTaskDelay(100); // Add a small delay to ensure the state change is registered before updating the LCD display and switching the LED mode

      if(color_temp_kalvins >= 9000 || color_temp_kalvins <= 0) {
        color_temp_kalvins = 500;
      } else {
        color_temp_kalvins += 500;
      }

      led_strip->SetState(LightState::LIGHTS_ON);
      led_strip->SetColorTemperature(color_temp_kalvins);
      lcd_display->printTop(String(color_temp_kalvins) + "K           ");

      Serial.print("Color Temperature: " + String(color_temp_kalvins) + "K");
  } else if (blueButton.isReleased()) {
    Serial.println("Blue button released");
  } else if (yellowButton.isReleased()) {
    Serial.println("Yellow button released");

    // Turn Display on and off with yellow button
    if (lcd_display->GetBacklightState()) {
        lcd_display->SetBacklightState(false);
        lcd_display->printTop("Display Off     ");
    } else {
        lcd_display->SetBacklightState(true);
        lcd_display->printTop("Display On      ");
    }    
  }
}

void retrieveColors() {
    //Serial.print("Red - Pot: ");
    int red_val = readPot(RED_POT_PIN); 
    //Serial.print("Green - Pot: ");
    int green_val = readPot(GREEN_POT_PIN);
    //Serial.print("Blue - Pot: ");
    int blue_val = readPot(BLUE_POT_PIN);

    color_value.red = red_val;
    color_value.green = green_val;
    color_value.blue = blue_val;
}

int readPot(int pot_pin) {
  int pot_value = analogRead(pot_pin);                    // read the value from the potentiometer (0-4095 for 12-bit ADC)
  uint32_t voltage_mV = analogReadMilliVolts(pot_pin);    // convert the raw value to millivolts (0-3300 mV for 3.3V reference)
  int color_val = (int)floor(pot_value / 16);             // scale the raw value to a range of 0-255 (for 8-bit color values)

  /*
  Serial.print(pot_value);
  Serial.print(" | Val: ");
  Serial.print(pot_value);
  Serial.print(" | mV: ");
  Serial.println(voltage_mV);
  */

  return color_val;
}

void readTemperature() {
    float humidity = dht.readHumidity();          // Read humidity (percent)
    float temperature = dht.readTemperature();    // Read temperature as Celsius (the default)

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Print the humidity and temperature
    Serial.print("Humidity: "); 
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.println(" *C");

    // Update LCD display and LED strip only if the temperature has changed since the last reading
    if (last_temperature != temperature) {
      lcd_display->printBottom(String(temperature, 1) + "C, " + String(humidity, 0) + "% H");
      last_temperature = temperature;
      led_strip->SetTemperature(temperature);
    }
}

void initSevenSegment() {
  //set pins to output
  pinMode(STcp,OUTPUT);
  pinMode(SHcp,OUTPUT);
  pinMode(DS,OUTPUT);
}

void printSevenSegment() {
  int randomNum = rand() % 36;
  digitalWrite(STcp,LOW); //ground ST_CP and hold low for as long as you are transmitting
  shiftOut(DS,SHcp,MSBFIRST,datArray[randomNum]);
  digitalWrite(STcp,HIGH); //pull the ST_CPST_CP to save the data
  //delay(1000);
}
