#include "ws_4in2b_V2.h"
#include "epd_hal_esp32.h"

WS_4in2b_V2::WS_4in2b_V2(unsigned int cs_pin, unsigned int dc_pin, unsigned int reset_pin, unsigned int busy_pin) : Adafruit_GFX(EPD::DisplayWidth, EPD::DisplayHeight), driver(new EPD_HAL_ESP32(cs_pin, dc_pin, reset_pin, busy_pin)), _initialized(false)
{
}

void WS_4in2b_V2::begin()
{
  driver.init();
  driver.clear();
  memset(framebuffer, 0xFF, EPD::BufferSize);
  _initialized = true;
}

void WS_4in2b_V2::end()
{
  _initialized = false;
  /* Deep sleep */
  driver.sleep();
}

void WS_4in2b_V2::drawPixel(int16_t x, int16_t y, uint16_t color)
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

void WS_4in2b_V2::display()
{
  Serial.println("display(...)");
  if (!_initialized)
  {
    this->begin();
  }

  driver.display(framebuffer, false, false);
}

void WS_4in2b_V2::showBWImage(const uint8_t *image)
{
  if (!_initialized)
  {
    begin();
  }

  driver.display(image,false,false);

  end();
}
