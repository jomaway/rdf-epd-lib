#include "epd_hal.h"
#include <SPI.h>

class EPD_HAL_ESP32 : public EPD_HAL {
private:
    int cs, dc, rst, busy;
    SPISettings _spi_settings;
public:
    EPD_HAL_ESP32(int cs, int dc, int rst, int busy) 
        : cs(cs), dc(dc), rst(rst), busy(busy), _spi_settings(2000000, MSBFIRST, SPI_MODE0) {}

    void init() override {

        pinMode(cs, OUTPUT);
        pinMode(dc, OUTPUT);
        pinMode(rst, OUTPUT);
        pinMode(busy, INPUT);

        SPI.begin();
    }
    
    void send_command(uint8_t cmd) override {
        Serial.printf("EPD_HAL::send_cmd(%02x) \n", cmd);
        this->set_pin(dc, LOW);
        this->spi_start_transfer();
        this->spi_transfer(cmd);
        this->spi_end_transfer();
        this->set_pin(dc, HIGH);
    }

    void send_data(uint8_t data) override {
        this->set_pin(dc, HIGH);
        this->spi_start_transfer();
        this->spi_transfer(data);
        this->spi_end_transfer();
    }

    void send_data(const uint8_t *data, uint8_t len){
        this->set_pin(dc, HIGH);
        this->spi_start_transfer();
        for (int i=0; i<len; i++)
        {
            this->spi_transfer(*data++);
        }
        this->spi_end_transfer();
    }

    void set_reset_pin(bool state){
        set_pin(rst, state);
    }

    void wait_busy() {
        while(HIGH == this->read_pin(busy)) {   //1: busy, 0: idle
            // Wait
        } 
    }

    void delay_ms(uint8_t ms) {
        delay(ms);
    }

    void spi_start_transfer()
    {
        SPI.beginTransaction(this->_spi_settings);
        this->set_pin(cs, LOW);
    }

    void spi_transfer(uint8_t data)
    {
        SPI.transfer(data);
    }

    void spi_end_transfer(){
        this->set_pin(cs, HIGH);
        SPI.endTransaction();
    }

    protected:

    void set_pin(uint8_t pin, bool state) override
    {
        digitalWrite(pin, state);
    }

    bool read_pin(uint8_t pin) override
    {
        return digitalRead(pin);
    }
};