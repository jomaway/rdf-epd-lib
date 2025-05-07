
#ifndef EPD_HAL_H
#define EPD_HAL_H

#include <Arduino.h>

class EPD_HAL {
public:
    // more abstracted view
    virtual void init() = 0;
    virtual void send_command(uint8_t cmd) = 0;
    virtual void send_data(uint8_t data) = 0;
    virtual void send_data(const uint8_t* data, uint8_t len) = 0;
    virtual void wait_busy() = 0;

    // more low level
    virtual void spi_start_transfer() = 0;
    virtual void spi_transfer(uint8_t data) = 0;
    virtual void spi_end_transfer() = 0;
    virtual void set_reset_pin(bool state) = 0;
    virtual void delay_ms(uint8_t ms) = 0;

protected:
    // low level peripheral access.
    virtual void set_pin(uint8_t pin_number, bool state) = 0;
    virtual bool read_pin(uint8_t pin_number) = 0;

};

#endif
