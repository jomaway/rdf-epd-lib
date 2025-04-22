#include "epd_hal.h"
#include <SPI.h>

class EPD_HAL_ESP32 : public EPD_HAL {
private:
    int cs, dc, rst, busy;
public:
    EPD_HAL_ESP32(int cs, int dc, int rst, int busy) 
        : cs(cs), dc(dc), rst(rst), busy(busy) {}

    void init() override {
        pinMode(cs, OUTPUT);
        pinMode(dc, OUTPUT);
        pinMode(rst, OUTPUT);
        pinMode(busy, INPUT);

        SPI.begin();
        SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    }
    
    void send_command(uint8_t cmd) override {
        this->set_pin(dc, LOW);
        this->spi_transfer(cmd);
    }

    void send_data(uint8_t data) override {
        this->set_pin(dc, HIGH);
        this->spi_transfer(data);
    }

    /**
     *  @brief: module reset. 
     *          often used to awaken the module in deep sleep, 
     *          see Epd::Sleep();
     */
    void reset() override {
        this->set_pin(rst, HIGH);
        this->delay_ms(200);
        this->set_pin(rst, LOW);
        this->delay_ms(2);
        this->set_pin(rst, HIGH);
        this->delay_ms(200);
    }

    void wait_busy() {
        while(HIGH == this->read_pin(busy)) {   //1: busy, 0: idle
            // Wait
        } 
    }

    void delay_ms(uint8_t ms) {
        delay(ms);
    }

    void set_pin(uint8_t pin, bool state){
        digitalWrite(pin, state);
    }

    bool read_pin(uint8_t pin){
        return digitalRead(pin);
    }

    void spi_transfer(byte data)
    {
        this->set_pin(cs, LOW);
        SPI.transfer(data);
        this->set_pin(cs, HIGH);
    }
};