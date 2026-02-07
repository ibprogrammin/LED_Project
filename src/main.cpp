#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "DHT.h"

#define DHTPIN 14  // Set the pin connected to the DHT11 data pin
#define DHTTYPE DHT11 // DHT 11 
#define LED_PIN 19 // NeoPixel LED strip
#define NUM_LEDS 8 // Number of LEDs

DHT dht(DHTPIN, DHTTYPE);

const int STcp = 27;//ST_CP
const int SHcp = 26;//SH_CP 
const int DS = 25; //DS 
const int LEDpin = 18;
const int RPotPin = 34;
const int GPotPin = 35;
const int BPotPin = 39;

//0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ
int datArray[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,
0x77,0x7c,0x39,0x5e,0x79,0x71,0x3D,0x76,0x30,0x1e,0xf2,0x38,0x55,
0x54,0x5c,0x73,0x67,0xf7,0xed,0x78,0x3e,0x1c,0x1D,0x64,0x6e,0x5B};

static int led_frequency = 5;
//static double dim_value = 0.0; // MAX 255
static int increment = 0;
static int counter = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800); 

// put function declarations here:
int myFunction(int, int);
void initSevenSegment();
void printSevenSegment();

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);

  initSevenSegment();
  pinMode(LEDpin, OUTPUT);

  strip.begin();
  strip.show();

  Serial.begin(9600);
  Serial.println("DHT11 test!");
  dht.begin();
  delay(2000);
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

void loop() {
  // put your main code here, to run repeatedly:
  
  printSevenSegment();
  
  /*
  if (led_frequency <= 25) led_frequency++;
  else led_frequency = 5;

  int flicker = floor((1000 / led_frequency) / 2);
  for (int i = 0; i < led_frequency; i++) {
      digitalWrite(LEDpin, HIGH);
      delay(flicker);
      digitalWrite(LEDpin, LOW);
      delay(flicker);
  }
  delay(200);*/

  int r_potValue = analogRead(RPotPin); 
  uint32_t r_voltage_mV = analogReadMilliVolts(RPotPin);
  int red_val = (int)floor(r_potValue / 16);

  int g_potValue = analogRead(GPotPin); 
  uint32_t g_voltage_mV = analogReadMilliVolts(GPotPin);
  int green_val = (int)floor(g_potValue / 16);

  int b_potValue = analogRead(BPotPin); 
  uint32_t b_voltage_mV = analogReadMilliVolts(BPotPin);
  int blue_val = (int)floor(b_potValue / 16);

  Serial.print("Red - Pot: ");
  Serial.print(r_potValue);
  Serial.print(" | Val: ");
  Serial.print(red_val);
  Serial.print(" | mV: ");
  Serial.println(r_voltage_mV);

  Serial.print("Green - Pot: ");
  Serial.print(g_potValue);
  Serial.print(" | Val: ");
  Serial.print(green_val);
  Serial.print(" | mV: ");
  Serial.println(g_voltage_mV);

  Serial.print("Blue - Pot: ");
  Serial.print(b_potValue);
  Serial.print(" | Val: ");
  Serial.print(blue_val);
  Serial.print(" | mV: ");
  Serial.println(b_voltage_mV);

  // Turn on LEDs one by one
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, red_val, green_val, blue_val); // Set the color of the i-th LED to red
    strip.show(); // Update the LED strip with the new colors
    delay(100); // Wait for 100 milliseconds
  }
  
  // Turn off LEDs one by one
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0); // Set the color of the i-th LED to black (turn it off)
    strip.show(); // Update the LED strip with the new colors
    delay(100); // Wait for 100 milliseconds
  }

  
  double dim_value = (increment ^ 2);
  increment++;
  if (increment == 255) {
    dim_value = 0;
    increment = 0;
  }
  /*Serial.print(increment);
  Serial.print("/");
  Serial.print(dim_value);
  Serial.println(" ");*/
  analogWrite(15, (int)floor(dim_value));
  delay(33);

  /*for(int num = 0; num <10; num++)
  {
    digitalWrite(STcp,LOW); //ground ST_CP and hold low for as long as you are transmitting
    shiftOut(DS,SHcp,MSBFIRST,datArray[num]);
    digitalWrite(STcp,HIGH); //pull the ST_CPST_CP to save the data
    delay(1000);
  }*/

  if (counter == 150) {
    float humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float temperature = dht.readTemperature();

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

    counter = 0;
  } else {
    counter++;
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}