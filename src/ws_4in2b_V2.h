#pragma once

#include "ssd1683.h"
#include <Adafruit_GFX.h>
#include <inttypes.h>

// Display resolution
namespace EPD
{
    constexpr uint16_t DisplayWidth = 400;
    constexpr uint16_t DisplayHeight = 300;
    constexpr uint32_t BufferSize = DisplayHeight * DisplayWidth / 8;

    namespace Color
    {
        constexpr uint16_t Black = 0;
        constexpr uint16_t White = 1;
        constexpr uint16_t Red = 2;
    }
}

class WS_4in2b_V2 : public Adafruit_GFX
{
private:
    SSD1683 *driver;
    uint8_t framebuffer[EPD::BufferSize];
    bool _initialized = false;

public:
    WS_4in2b_V2(int cs_pin, int dc_pin, int reset_pin, int busy_pin);

    void begin();
    void end();

    void drawPixel(int16_t x, int16_t y, uint16_t color) override;
    void display(UpdateMode mode = UpdateMode::Normal);
    void showBWImage(const uint8_t *image);
};
