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

//const int STcp = 27;//ST_CP
//const int SHcp = 26;//SH_CP 
//const int DS = 25; //DS 

// Array to hold the 7-segment display values for digits 0-9 and letters A-Z
//0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ
int datArray[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
0x77,0x7c,0x39,0x5e,0x79,0x71,0x3D,0x76,0x30,0x1e,0xf2,0x38,0x55,
0x54,0x5c,0x73,0x67,0xf7,0xed,0x78,0x3e,0x1c,0x1D,0x64,0x6e,0x5B};

// Create instances of the classes for the LED strip, LCD display, and BLE service adapter
RGBLed* led_strip;            // Create an instance of the RGBLed class
LCDDisplaySmall* lcd_display;  // Create an instance of the LCDDisplaySmall class 
BLEServiceAdapter* ble_service_adapter;  // Create an instance of the BLEServiceAdapter class

RGBColor color_value; // Create an instance of the RGBColor struct to hold the current color values 

// put function declarations here:
void initSevenSegment();
void printSevenSegment();
void retrieveColors();
int readPot(int pot_pin);
void readTemperature();
void readButtonState();

// Task wrapper functions
void mainTaskWrapper(void* parameter);
void colorTaskWrapper(void* parameter);
void buttonTaskWrapper(void* parameter);
void lcdTaskWrapper(void* parameter);
void ledTaskWrapper(void* parameter);
void bleServiceTaskWrapper(void* parameter);

// Task handles for the different tasks
TaskHandle_t MainTaskHandle = NULL;
TaskHandle_t ColorTaskHandle = NULL;
TaskHandle_t ButtonTaskHandle = NULL;
TaskHandle_t LCDTaskHandle = NULL;
TaskHandle_t RGBLedTaskHandle = NULL;
TaskHandle_t BLEServiceTaskHandle = NULL;

float last_temperature = 0.0;
int color_temp_kalvins = 0;
bool led_strip_initialized = false;
std::string current_ble_value = "";

void setup() {
  Serial.begin(115200);
  delay(1000);
  analogReadResolution(8); // Set ADC resolution to 8 bits (0-255)

  xTaskCreatePinnedToCore(bleServiceTaskWrapper, "BLEServiceTask", 10000, NULL, 1, &BLEServiceTaskHandle, 1);
  xTaskCreatePinnedToCore(mainTaskWrapper, "MainTask", 10000, NULL, 1, &MainTaskHandle, 1);
  xTaskCreatePinnedToCore(colorTaskWrapper, "ColorTask", 10000, NULL, 2, &ColorTaskHandle, 1);
  xTaskCreatePinnedToCore(buttonTaskWrapper, "ButtonTask", 10000, NULL, 1, &ButtonTaskHandle, 1);
  xTaskCreatePinnedToCore(lcdTaskWrapper, "LCDDisplayTask", 10000, NULL, 1, &LCDTaskHandle, 1);
  xTaskCreatePinnedToCore(ledTaskWrapper, "LEDTask", 10000, NULL, 1, &RGBLedTaskHandle, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void mainTaskWrapper(void* parameter) {
  Serial.println("DHT11 test!");
  dht.begin();

  vTaskDelay(2000); // Delay to allow other tasks to initialize properly

  while (true) {
    readTemperature();   
    vTaskDelay(5000);
  }
}

void colorTaskWrapper(void* parameter) {
  color_value = {0, 0, 0}; // Initialize color values to 0 (off)
  vTaskDelay(5000); // Delay to allow other tasks to initialize properly

  while (true) {
    // Check if the color temperature mode is active, if not retrieve the current color values from the potentiometers and update the LED strip accordingly  
    if (color_temp_kalvins == 0) {
      retrieveColors();
      if (led_strip != nullptr) {
        led_strip->SetColorVal(color_value);
      } else {
        Serial.println("LED strip instance is not initialized yet!");
      }
    }
    vTaskDelay(1000);
  }
}

void buttonTaskWrapper(void* parameter) {
  //Setup buttons
  redButton.setDebounceTime(20);
  greenButton.setDebounceTime(20);
  blueButton.setDebounceTime(20);
  yellowButton.setDebounceTime(20);

  vTaskDelay(5000); // Delay to allow other tasks to initialize properly

  while (true) {
    readButtonState();
    vTaskDelay(50);
  }
}

void lcdTaskWrapper(void* parameter) {
  lcd_display = new LCDDisplaySmall();
  lcd_display->load();
  lcd_display->printTop("Off Mode        ");

  vTaskDelay(2000); // Delay to allow other tasks to initialize properly

  while (true) {
    //lcd_display->print();
    lcd_display->cycleMessages(0);
    vTaskDelay(10000);
  }
}

void ledTaskWrapper(void* parameter) {
  led_strip = new RGBLed(NUM_LEDS, LED_PIN);      // Initialize LED strip
  vTaskDelay(1000);                                // Wait for LED strip to initialize
  led_strip->SetState(LightState::LIGHTS_OFF);    // Set the desired initial light state (e.g., CHASE, DIM_UP_DOWN, etc.)
  
  while (true) {
    led_strip->Run();
    //Serial.printf("LED task Stack Free: %u bytes\n", uxTaskGetStackHighWaterMark(NULL));
  }
}

void bleServiceTaskWrapper(void* parameter) {
    Serial.println("Loading BLE Service Adapter");
    ble_service_adapter = new BLEServiceAdapter();
    vTaskDelay(500);
    Serial.println("Initializing BLE Service Adapter");
    ble_service_adapter->init();
    Serial.println("BLE Service Adapter loaded successfully");

    vTaskDelay(2000);     // Delay to allow other tasks to initialize properly

    while (true) {
        // Check if a new value has been received via BLE and update the LED strip state accordingly
        if (ble_service_adapter != nullptr) {
            std::string new_value = ble_service_adapter->GetValue();
            if (new_value != current_ble_value) {
                Serial.println("New BLE value received: " + String(new_value.c_str()));
                current_ble_value = new_value;

                // Check if the led_strip and lcd_display instance is initialized before calling HandleBLEMessage to avoid potential null pointer dereference
                if (led_strip != nullptr) {
                    led_strip->HandleBLEMessage(String(current_ble_value.c_str()));
                } else {
                    Serial.println("LED strip instance is not initialized yet!");
                }
                if (lcd_display != nullptr) {
                    lcd_display->addMessage(LED_MODE_STRINGS[(int)led_strip->GetState() + 1], 1);
                    //lcd_display->cycleMessages();
                } else {
                    Serial.println("LCD display instance is not initialized yet!");
                }
            }
        }

        vTaskDelay(2000);  
    }
}

void readButtonState() {
  redButton.loop();
  greenButton.loop();
  blueButton.loop();
  yellowButton.loop();

  /*
  // get button state after debounce
  int button1_state = redButton.getState();  // the state after debounce
  int button2_state = greenButton.getState();  // the state after debounce
  int button3_state = blueButton.getState();  // the state after debounce
  int button4_state = yellowButton.getState();  // the state after debounce

  
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
    /*switch (led_strip->GetState()) {
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
    }*/
    //lcd_display->print();
    
    lcd_display->addMessage(LED_MODE_STRINGS[(int)led_strip->GetState() + 1], 1);
    //lcd_display->cycleMessages();

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
      //lcd_display->printTop(String(color_temp_kalvins) + "K           ");

      lcd_display->addMessage(std::to_string(color_temp_kalvins) + "K", 4);
      //lcd_display->cycleMessages();

      Serial.println("Color Temperature: " + String(color_temp_kalvins) + "K");
  } else if (blueButton.isReleased()) {
      Serial.println("Blue button released");
  } else if (yellowButton.isReleased()) {
      Serial.println("Yellow button released");

      // Turn Display on and off with yellow button
      if (lcd_display->GetBacklightState()) {
          lcd_display->SetBacklightState(false);
          //lcd_display->printTop("Display Off     ");
          lcd_display->addMessage("Display Off", 5);
      } else {
          lcd_display->SetBacklightState(true);
          //lcd_display->printTop("Display On      ");
          lcd_display->addMessage("Display On", 5);
      }    
      
      //lcd_display->cycleMessages();
  }
}

void retrieveColors() {
    // Read potentiometer values for RGB color components
    int red_val = readPot(RED_POT_PIN); 
    int green_val = readPot(GREEN_POT_PIN);
    int blue_val = readPot(BLUE_POT_PIN);

    // Update color values only if they have changed
    if (red_val != color_value.red || green_val != color_value.green || blue_val != color_value.blue) {
      color_value.red = red_val;
      color_value.green = green_val;
      color_value.blue = blue_val;
      
      // Update LCD display with new color values
      if (lcd_display != nullptr) {
        lcd_display->addMessage("R:" + std::to_string(red_val) + " G:" + std::to_string(green_val) + " B:" + std::to_string(blue_val), 3);
        //lcd_display->cycleMessages();
      } else {
        Serial.println("LCD display instance is not initialized yet!");
      }
      //lcd_display->cycleMessages();
      
      // Debug output to Serial Monitor
      Serial.print("New Color Values - R: ");
      Serial.print(red_val);
      Serial.print(" G: ");
      Serial.print(green_val);
      Serial.print(" B: ");
      Serial.println(blue_val);
    }
}

int readPot(int pot_pin) {
  int pot_value = analogRead(pot_pin);                    // read the value from the potentiometer (0-4095 for 12-bit ADC)
  uint32_t voltage_mV = analogReadMilliVolts(pot_pin);    // convert the raw value to millivolts (0-3300 mV for 3.3V reference)
  //int color_val = (int)floor(pot_value / 16);             // scale the raw value to a range of 0-255 (for 8-bit color values)
  int color_val = map(pot_value, 0, 255, 0, 255);       // scale the raw value to a range of 0-255 (for 8-bit color values)

  /*
  Serial.print(pot_value);
  Serial.print(" | Val: ");
  Serial.print(pot_value);
  Serial.print(" | mV: ");
  Serial.println(voltage_mV);
  */

  return color_val;
}

//TODO there might be an issue with switching to temperature mode on the LED strip before the temperature has been set in the class
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
      //lcd_display->printBottom(String(temperature, 1) + "C, " + String(humidity, 0) + "% H");
      last_temperature = temperature;
      led_strip->SetTemperature(temperature);

      std::string temp_str = (String(temperature, 1) + "C, " + String(humidity, 0) + "% H").c_str();

      lcd_display->addMessage(temp_str, 2);
      //lcd_display->cycleMessages();

      if (ble_service_adapter != nullptr) {
        //ble_service_adapter->SendValue(temp_str);
      }
    }
}

/*
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
*/
