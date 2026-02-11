#ifndef LCDDISPLAYSAMLL_H
#define LCDDISPLAYSAMLL_H

#include <WString.h>
#include <string>
#include <vector>
#include <array>

#define I2C_SDA 21
#define I2C_SCL 22

#define LCD_ROWS 2
#define LCD_COLS 16

#define MESSAGE_DISPLAY_TIME_SECONDS 2

class LCDDisplaySmall {
    public:
    LCDDisplaySmall();
    void load();
    void print();
    void printTop(String text);
    void printBottom(String text);
    void SetText(String text);
    bool GetBacklightState();
    void SetBacklightState(bool state);
    void addMessage(const std::string& message, int index);
    void displayCurrentRows();
    std::string padString(const std::string& str, int totalLength);
    void scrollUp();
    void initializeMessages();
    void cycleMessages(int cycles = 0);
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