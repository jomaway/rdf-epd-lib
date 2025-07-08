#ifndef epaper_h
#define epaper_h

#include "epd_driver_ssd1683.h"
#include "epd_graphics.h"
#include <Adafruit_GFX.h>
#include <stdint.h>

// Display resolution
namespace EPD
{
    constexpr uint16_t DisplayWidth = 400;
    constexpr uint16_t DisplayHeight = 300;
    constexpr uint32_t BufferSize = DisplayHeight * DisplayWidth / 8;

    namespace Color {
        // this is how the epd treats colors.
        constexpr uint16_t White = 1;
        constexpr uint16_t Black = 0;
        // constexpr uint8_t Red = 0;
    }
}

class WaveShare_EPD_4in2b_V2 : public Adafruit_GFX {
private:
    EPD_Driver_SSD1683 driver;
    uint8_t framebuffer[EPD::BufferSize];
    bool _initialized = false;  
public:
    WaveShare_EPD_4in2b_V2(unsigned int cs_pin, unsigned int dc_pin, unsigned int reset_pin, unsigned int busy_pin);
    
    void begin(); 
    void end();

    void drawPixel(int16_t x, int16_t y, uint16_t color) override;
    void display();
};

#endif // epaper_h