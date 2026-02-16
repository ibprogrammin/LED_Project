#ifndef LIGHTSTATE_H
#define LIGHTSTATE_H

enum class LightMode {
    LIGHTS_OFF,
    LIGHTS_ON,
    DMX,
    MAX_STATE
};

enum class ColorSource {
    NONE,
    CONTROLS,
    BLE,
    COLOR_TEMPERATURE,
    MAX_STATE
};

// Enum to represent the different states of the LED strip
enum class LightEffect {
    LIGHTS_OFF,
    LIGHTS_ON,
    CHASE,
    DIM_UP_DOWN,
    FLASH,
    STROBE,
    TEMPERATURE_SENSITIVE,
    THERAPY,
    MAX_STATE
};

#endif