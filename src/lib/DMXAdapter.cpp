#include "DMXAdapter.h"

DMXAdapter::DMXAdapter() {
    // Initialize the current DMX color values to 0
    current_dmx_color.red = 0;
    current_dmx_color.green = 0;
    current_dmx_color.blue = 0;
}

void DMXAdapter::init() {
    // Initialize the DMX driver in receive mode, using the standard Serial2 RX pin for receiving DMX data and no TX pin since we are only receiving data in this mode
    dmx.begin(DMXMode::Receive, DMXPin::Serial2Rx, DMXPin::NoTx);
}

void DMXAdapter::run() {
    // Update the current DMX color values by reading the corresponding channels from the DMX driver
    current_dmx_color.red = dmx.getChannel(RED_CHANNEL);
    current_dmx_color.green = dmx.getChannel(GREEN_CHANNEL);
    current_dmx_color.blue = dmx.getChannel(BLUE_CHANNEL);

    vTaskDelay(DMX_REFRESH_INTERVAL_MS);        // Delay to achieve a DMX refresh rate of 44Hz (1000ms / 44 ≈ 22.7272ms)
}

void DMXAdapter::setColor(uint8_t r, uint8_t g, uint8_t b) {
    dmx.setChannel(1, r);
    dmx.setChannel(2, g);
    dmx.setChannel(3, b);
}

