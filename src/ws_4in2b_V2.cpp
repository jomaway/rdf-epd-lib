#include "ws_4in2b_V2.h"
#include "epd_hal.h"

WS_4in2b_V2::WS_4in2b_V2(int cs_pin, int dc_pin, int reset_pin, int busy_pin) : Adafruit_GFX(EPD::DisplayWidth, EPD::DisplayHeight), _initialized(false)
{
  auto hal = new EPD_HAL_Arduino(cs_pin, dc_pin, reset_pin, busy_pin);

  #if CONFIG_IDF_TARGET_ESP32
  SPIClass* mySPI = new SPIClass(VSPI);
  hal->select_spi(*mySPI);
  #elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
  SPIClass* mySPI = new SPIClass(FSPI);
  hal->select_spi(*mySPI);
  #endif
  
  driver = new SSD1683(hal);
}



void WS_4in2b_V2::begin()
{
  Serial.println("BEGIN");
  driver->init();
  driver->clear();
  memset(framebuffer, 0xFF, EPD::BufferSize);
  _initialized = true;
  Serial.println("END");
}

void WS_4in2b_V2::end()
{
  _initialized = false;
  /* Deep sleep */
  driver->sleep();
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

void WS_4in2b_V2::display(UpdateMode mode)
{
  Serial.println("display(...)");
  if (!_initialized)
  {
    driver->init();
    _initialized = true;
  }

  driver->display(framebuffer, RAM::BW, mode);
}

void WS_4in2b_V2::showBWImage(const uint8_t *image)
{
  Serial.println("show(...)");
  if (!_initialized)
  {
    driver->init();
    _initialized = true;
  }

  driver->display(image);

  end();
}
