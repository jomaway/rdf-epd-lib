#pragma once

#include <Arduino.h>
#include <SPI.h>

constexpr uint32_t EPD_SPI_CLOCK = 2000000;

class EPD_HAL_INTERFACE
{
public:
    // more abstracted view
    virtual void init() = 0;
    virtual void send_command(const uint8_t cmd) = 0;
    virtual void send_data_byte(const uint8_t data) = 0;
    virtual void send_data_bulk(const uint8_t *data, uint32_t len) = 0;
    virtual void send_data_repeat(const uint8_t data, uint32_t repeat) = 0;
    virtual void wait_busy() = 0;
    virtual bool wait_busy(uint64_t timeout) = 0;

    // more low level
    virtual void set_reset_pin(bool state) = 0;
    virtual void delay_ms(uint32_t ms) = 0;
    virtual void spi_start_transfer() = 0;
    virtual void spi_end_transfer() = 0;
    // virtual void spi_write_byte(uint8_t byte) = 0;
};



class EPD_HAL_Arduino : public EPD_HAL_INTERFACE
{
public:
    // more abstracted view
    EPD_HAL_Arduino(int cs, int dc, int rst, int busy);
    void init() override;
    void send_command(const uint8_t cmd) override;
    void send_data_byte(const uint8_t data) override;
    void send_data_bulk(const uint8_t *data, uint32_t len) override;
    void send_data_repeat(const uint8_t data, uint32_t repeat) override;
    void wait_busy() override;
    bool wait_busy(uint64_t timeout) override;

    // more low level
    void set_reset_pin(bool state) override;
    void delay_ms(uint32_t ms) override;
    void spi_start_transfer() override;
    void spi_end_transfer() override;
    void select_spi(SPIClass& spi);

protected:
    int _cs, _dc, _rst, _busy;
    SPIClass *_spi;
    SPISettings _spi_settings;
};
