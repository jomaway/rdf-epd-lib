#include "epd_hal.h"
#include <SPI.h>
#include "esp_log.h"

static const char *TAG = "RDF_EPD_HAL";

class EPD_HAL_ESP32 : public EPD_HAL
{
private:
    int cs, dc, rst, busy;
    SPISettings _spi_settings;

public:
    EPD_HAL_ESP32(int cs, int dc, int rst, int busy)
        : cs(cs), dc(dc), rst(rst), busy(busy), _spi_settings(2000000, SPI_MSBFIRST, SPI_MODE0) {}

    void init() override
    {

        pinMode(cs, OUTPUT);
        pinMode(dc, OUTPUT);
        pinMode(rst, OUTPUT);
        pinMode(busy, INPUT);

        SPI.begin(SCK, MISO, MOSI, cs);
    }

    void send_command(const uint8_t cmd) override
    {
        ESP_LOGI(TAG, "send_cmd(%02x)", cmd);
        this->set_pin(dc, LOW);
        this->spi_start_transfer();
        SPI.write(cmd);
        this->spi_end_transfer();
        this->set_pin(dc, HIGH);
    }

    void send_data_byte(const uint8_t data) override
    {
        ESP_LOGD(TAG, "send_data(%02x)", data);
        this->set_pin(dc, HIGH);
        this->spi_start_transfer();
        SPI.write(data);
        this->spi_end_transfer();
    }

    void send_data_bulk(const uint8_t *data, uint32_t len)
    {
        this->set_pin(dc, HIGH);
        this->spi_start_transfer();
        SPI.writeBytes(data, len);
        this->spi_end_transfer();
    }

    void send_data_repeat(const uint8_t value, uint32_t repeat)
    {
        this->set_pin(dc, HIGH);
        this->spi_start_transfer();
        SPI.writePattern(&value, 1, repeat);
        this->spi_end_transfer();
    }

    inline void set_reset_pin(bool state)
    {
        set_pin(rst, state);
    }

    void wait_busy()
    {
        while (HIGH == this->read_pin(busy))
        { // 1: busy, 0: idle
          // Wait
        }
    }

    inline void delay_ms(uint8_t ms)
    {
        delay(ms);
    }

protected:
    inline void spi_start_transfer()
    {
        SPI.beginTransaction(this->_spi_settings);
        this->set_pin(cs, LOW);
    }

    inline void spi_end_transfer()
    {
        this->set_pin(cs, HIGH);
        SPI.endTransaction();
    }

    inline void set_pin(uint8_t pin, bool state) override
    {
        digitalWrite(pin, state);
    }

    inline bool read_pin(uint8_t pin) override
    {
        return digitalRead(pin);
    }
};