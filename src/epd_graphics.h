#ifndef EPD_GRAPHICS_H
#define EPD_GRAPHICS_H
#include <Adafruit_GFX.h>
#include <stdint.h>

#define BLACK_PIXEL 0
#define WHITE_PIXEL 1

class Canvas : public Adafruit_GFX
{
public:
    uint8_t framebuffer[300*400/8];

    Canvas(uint16_t width, uint16_t height) : Adafruit_GFX(width, height)
    {
        uint32_t buffer_size = height * width / 8;
        // memset(framebuffer, 0xFF, buffer_size);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override
    {
        if (x < 0 || x > this->WIDTH || y < 0 || y >= this->HEIGHT)
        {
            return;
        }

        if (color)
        {
            // set pixel
            this->framebuffer[(x + y * this->WIDTH) / 8] |= 0x80 >> (x % 8);
        }
        else
        {
            // clear pixel
            this->framebuffer[(x + y * this->WIDTH) / 8] &= ~(0x80 >> (x % 8));
        }
    }

    const byte *getImage()
    {
        return this->framebuffer;
    }
};

#endif