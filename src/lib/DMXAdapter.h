#ifndef DMXADAPTER_H
#define DMXADAPTER_H    

#include <easydmx.h>
#include "RGBColor.h"

#define RED_CHANNEL 1
#define GREEN_CHANNEL 2 
#define BLUE_CHANNEL 3

#define DMX_REFRESH_RATE_HZ 44
#define DMX_REFRESH_INTERVAL_MS (1000 / DMX_REFRESH_RATE_HZ)

#define DMX_START_ADDRESS 1

class DMXAdapter {
    public:
        DMXAdapter();
        void init();
        void run();
        void setColor(uint8_t r, uint8_t g, uint8_t b);

    private:
        EasyDMX dmx;
        RGBColor current_dmx_color;
};

#endif