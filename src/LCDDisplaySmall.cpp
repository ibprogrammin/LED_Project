#include "LCDDisplaySmall.h"
#include <LiquidCrystal_I2C.h>
#include <WString.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

LCDDisplaySmall::LCDDisplaySmall() {
    text = "Welcome!";
}

void LCDDisplaySmall::load() {
    // Initialize LCD display
    lcd.init(I2C_SDA, I2C_SCL); // initialize the lcd to use user defined I2C pins
	lcd.backlight();            // turn on the backlight
}

void LCDDisplaySmall::print() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text);
}

void LCDDisplaySmall::printTop(String text) {
    lcd.setCursor(0,0);             // Move the cursor to the beginning of the first line
    lcd.print("                "); // Clear the line
    lcd.setCursor(0,0);
    lcd.print(text);                // Print the new text
}

void LCDDisplaySmall::printBottom(String text) {
    lcd.setCursor(0,1);         // Move the cursor to the beginning of the second line  
    lcd.print("                "); // Clear the line
    lcd.setCursor(0,1);
    lcd.print(text);                // Print the new text
}

void LCDDisplaySmall::SetText(String text) {
    this->text = text;
}

bool LCDDisplaySmall::GetBacklightState() {
    return backlight_on;
}

void LCDDisplaySmall::SetBacklightState(bool state) {
    backlight_on = state;
    if (state) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
}