#ifndef LIGHTSTATE_H
#define LIGHTSTATE_H

enum class LightState {
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