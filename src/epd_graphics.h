#ifndef EPD_GRAPHICS_H
#define EPD_GRAPHICS_H
#include <Adafruit_GFX.h>
#include <stdint.h>

#define BLACK_PIXEL 0
#define WHITE_PIXEL 1

class Canvas : public Adafruit_GFX
{
public:
    uint8_t framebuffer[300 * 400 / 8];

    Canvas(uint16_t width = 400, uint16_t height = 300) : Adafruit_GFX(width, height)
    {
        const uint32_t buffer_size = height * width / 8;
        memset(framebuffer, 0xFF, buffer_size);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override
    {
        if (x < 0 || x > this->_width || y < 0 || y >= this->_height)
        {
            return;
        }

        if (this->rotation == 1)
        {
            auto temp = y;
            y = x;
            x = this->WIDTH - temp;
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