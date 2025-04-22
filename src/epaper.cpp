#include "epaper.h"
#include "epd_hal_esp32.h"

EPaper::EPaper(unsigned int cs_pin, unsigned int dc_pin, unsigned int reset_pin, unsigned int busy_pin)
{
  this->hal = new EPD_HAL_ESP32(cs_pin, dc_pin, reset_pin, busy_pin);
  this->driver = new EPD_Driver(hal);
}

void EPaper::begin()
{
  Serial.println("EPAPER::begin()");
  driver->init();
  driver->clear();
}

void EPaper::showImage(const byte *image, ImageColor_t color)
{
  Serial.println("EPAPER::showImage()");
  /* This clears the SRAM of the e-paper display */
  this->driver->clear();

  // draw here!
  this->driver->display(image, color);

  /* Deep sleep */
  this->driver->sleep();
  Serial.println("EPAPER::showImage -> END");
}
