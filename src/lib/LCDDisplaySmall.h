#ifndef LCDDISPLAYSAMLL_H
#define LCDDISPLAYSAMLL_H

#include <WString.h>
#include <string>
#include <vector>
#include <array>

#define I2C_SDA 21                              // I2C pins for the LCD display
#define I2C_SCL 22                              // I2C pins for the LCD display

#define LCD_ROWS 2                              // Number of rows on the LCD display
#define LCD_COLS 16                             // Number of columns on the LCD display

#define MESSAGE_DISPLAY_TIME_SECONDS 2          // Time to display each message before scrolling to the next one
#define WELCOME_MESSAGE "Welcome!"              // Initial message to display on the LCD when the system starts up

// Class to manage the LCD display, including a message queue for displaying multiple messages in sequence
class LCDDisplaySmall {
    public:
    LCDDisplaySmall();
    void init();
    void print();
    void printTop(String text);
    void printBottom(String text);
    void setText(String text);
    bool getBacklightState();
    void setBacklightState(bool state);
    void addMessage(const std::string& message, int index);
    void displayCurrentRows();
    std::string padString(const std::string& str, int totalLength);
    void scrollUp();
    void initializeMessages();
    void cycleMessages(int cycles = 0);
    void scrollText(int row, std::string message, int delay_time);
    int getMessageCount() const;

    private:
    std::string text;
    float temperature;
    float humidity;
    bool backlight_on = true;
    std::array<std::string, 6> message_list = {"", "", "", "", "", ""}; // Queue to hold incoming messages for the LCD display
    size_t current_index;
    std::string first_row;
    std::string second_row;
};

#endif