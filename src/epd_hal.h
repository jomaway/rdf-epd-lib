
#ifndef EPD_HAL_H
#define EPD_HAL_H

#include <Arduino.h>

class EPD_HAL {
public:
    // more abstracted view
    virtual void init() = 0;
    virtual void send_command(byte cmd) = 0;
    virtual void send_data(byte data) = 0;
    virtual void reset() = 0;
    virtual void wait_busy() = 0;
protected:
    // low level peripheral access.
    virtual void set_pin(uint8_t pin_number, bool state) = 0;
    virtual bool read_pin(uint8_t pin_number) = 0;
    virtual void delay_ms(uint8_t ms) = 0;
    virtual void spi_transfer(byte data) = 0;
};

#endif
