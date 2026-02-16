#include "LCDDisplaySmall.h"
#include <LiquidCrystal_I2C.h>

// Create an instance of the LiquidCrystal_I2C class to control the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Constructor for the LCDDisplaySmall class, initializes the text and message queue
LCDDisplaySmall::LCDDisplaySmall() {
    current_index = 0;
    first_row = "";
    second_row = "";
    text = WELCOME_MESSAGE;
    message_list[0] = WELCOME_MESSAGE;
}

// Load function to initialize the LCD display
void LCDDisplaySmall::init() {
    // Initialize LCD display
    lcd.init(I2C_SDA, I2C_SCL); // initialize the lcd to use user defined I2C pins
	lcd.backlight();            // turn on the backlight
}

// Print function to display the current text on the LCD
void LCDDisplaySmall::print() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(text.c_str());
}

// Function to print text on the top row of the LCD
void LCDDisplaySmall::printTop(String text) {
    lcd.setCursor(0,0);             // Move the cursor to the beginning of the first line
    lcd.print("                "); // Clear the line
    lcd.setCursor(0,0);
    lcd.print(text.c_str());                // Print the new text
}

// Function to print text on the bottom row of the LCD
void LCDDisplaySmall::printBottom(String text) {
    lcd.setCursor(0,1);         // Move the cursor to the beginning of the second line  
    lcd.print("                "); // Clear the line
    lcd.setCursor(0,1);
    lcd.print(text);                // Print the new text
}

// Function to set the text variable (not directly used for printing, but can be useful for storing the current text state)
void LCDDisplaySmall::setText(String text) {
    this->text = text.c_str();
}

// Function to get the current state of the backlight
bool LCDDisplaySmall::getBacklightState() {
    return backlight_on;
}

// Function to set the state of the backlight
void LCDDisplaySmall::setBacklightState(bool state) {
    backlight_on = state;
    if (state) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
}

// Add a message to the queue
void LCDDisplaySmall::addMessage(const std::string& message, int index) {
    message_list[index] = message;
}

// Display current state of both rows
void LCDDisplaySmall::displayCurrentRows() {
    lcd.setCursor(0,0);                                             // Move the cursor to the beginning of the first line
    lcd.print((String)padString(first_row, LCD_COLS).c_str());      // Print the first row

    lcd.setCursor(0,1);                                             // Move the cursor to the beginning of the second line
    lcd.print((String)padString(second_row, LCD_COLS).c_str());     // Print the second row
}

// Helper function to pad or truncate string to fit display width
std::string LCDDisplaySmall::padString(const std::string& str, int width) {
    if (str.length() >= width) {
        return str.substr(0, width);
    } else {
        return str + std::string(width - str.length(), ' ');
    }
}

// Scroll up: move row2 to row1, and load next message into row2
void LCDDisplaySmall::scrollUp() {
    if (message_list.empty()) return; // No more messages to display

    vTaskDelay(300); // Simulate scroll up animation delay
    first_row = second_row;

    current_index = (current_index + 1) % message_list.size();
    second_row = message_list[current_index];
    
    displayCurrentRows();
}

// Initialize display with first two messages
void LCDDisplaySmall::initializeMessages() {
    if (message_list.size() >= 1) {
        current_index = 0;
        first_row = message_list[0];
    }
    
    if (message_list.size() >= 2) {
        current_index = 1;
        second_row = message_list[1];
    }
    
    displayCurrentRows();
}

// Process the queue - display each message for a few seconds
void LCDDisplaySmall::cycleMessages(int cycles) {
    if (message_list.empty()) return; // No messages to display

    // If this is the first time processing the queue, initialize the display with the first two messages
    initializeMessages();

    vTaskDelay(MESSAGE_DISPLAY_TIME_SECONDS * 1000); // Initial display time

    int scroll_count = 0;
    int total_scrolls_per_cycle = getMessageCount(); // Number of messages determines how many scrolls before repeating 
    int max_scrolls = (cycles == 0) ? -1 : (total_scrolls_per_cycle * cycles); // If cycles is 0, scroll indefinitely  

    while (max_scrolls == -1 || scroll_count < max_scrolls) {
        scrollUp();
        scroll_count++;
        
        if (cycles > 0 && scroll_count % total_scrolls_per_cycle == 0) {
            // Implement if you would like to only cycle a certain amount of times through the message queue before stopping or doing something else
        }
        
        vTaskDelay(MESSAGE_DISPLAY_TIME_SECONDS * 1000);
    }

}

void LCDDisplaySmall::scrollText(int row, std::string message, int delay_time) {
    for (int i=0; i < LCD_COLS; i++) {
        message = " " + message;  
    } 
    message = message + " "; 

    for (int pos = 0; pos < message.length(); pos++) {
        std::string output = message.substr(pos, pos + LCD_COLS);
        lcd.setCursor(0, row);
        lcd.print(output.c_str());
        vTaskDelay(delay_time);
    }
}

void displayCustomChar(uint8_t custom_char[8], uint8_t location) {
    lcd.createChar(location, custom_char);
    lcd.home();
    lcd.write(0);
}

// Get the number of messages in queue
int LCDDisplaySmall::getMessageCount() const {
    return message_list.size();
}