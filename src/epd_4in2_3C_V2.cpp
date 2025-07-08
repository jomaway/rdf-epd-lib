#include "epd_4in2_3C_V2.h"
#include "epd_hal_esp32.h"

WaveShare_EPD_4in2b_V2::WaveShare_EPD_4in2b_V2(unsigned int cs_pin, unsigned int dc_pin, unsigned int reset_pin, unsigned int busy_pin): 
  Adafruit_GFX(400, 300),driver(new EPD_HAL_ESP32(cs_pin, dc_pin, reset_pin, busy_pin)), _initialized(false)
{
  // this->hal = new EPD_HAL_ESP32(cs_pin, dc_pin, reset_pin, busy_pin);
  // this->driver = new EPD_Driver_SSD1683(hal);
}

void WaveShare_EPD_4in2b_V2::begin()
{
  driver.init();
  driver.clear();
  memset(framebuffer, 0xFF, EPD::BufferSize);
  _initialized = true;
}

void WaveShare_EPD_4in2b_V2::end()
{
  _initialized = false;
  /* Deep sleep */
  this->driver.sleep();
}

void WaveShare_EPD_4in2b_V2::drawPixel(int16_t x, int16_t y, uint16_t color)
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

        if (color == EPD::Color::White)
        {
            // set pixel (as the epd interprets white as 1)
            this->framebuffer[(x + y * this->WIDTH) / 8] |= 0x80 >> (x % 8);
        }
        else
        {
            // clear pixel
            this->framebuffer[(x + y * this->WIDTH) / 8] &= ~(0x80 >> (x % 8));
        }
    }

void WaveShare_EPD_4in2b_V2::display()
{
  Serial.println("display(...)");
  if (!_initialized) {
    this->begin();
  }

  driver.display(framebuffer, false, false);
}