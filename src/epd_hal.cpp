#include "epd_hal.h"

static const char *TAG = "EPD_HAL_ARDUINO";

EPD_HAL_Arduino::EPD_HAL_Arduino(int cs, int dc, int rst, int busy) : _cs(cs), _dc(dc), _rst(rst), _busy(busy), _spi(&SPI), _spi_settings(EPD_SPI_CLOCK, SPI_MSBFIRST, SPI_MODE0)
{
}

void EPD_HAL_Arduino::init()
{
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);

    pinMode(_dc, OUTPUT);
    digitalWrite(_dc, HIGH);

    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);

    pinMode(_busy, INPUT);

    _spi->begin(SCK, MISO, MOSI, _cs);
}

void EPD_HAL_Arduino::send_command(const uint8_t cmd)
{
    ESP_LOGD(TAG, "CMD: %02x", cmd);
    digitalWrite(_dc, LOW);
    this->spi_start_transfer();
    _spi->write(cmd);
    this->spi_end_transfer();
    digitalWrite(_dc, HIGH);
}

void EPD_HAL_Arduino::send_data(const uint8_t data)
{
    ESP_LOGD(TAG, "DATA: %02x", data);
    digitalWrite(_dc, HIGH);
    this->spi_start_transfer();
    _spi->write(data);
    this->spi_end_transfer();
}

void EPD_HAL_Arduino::send_data(const uint8_t *data, uint32_t len)
{
    digitalWrite(_dc, HIGH);
    this->spi_start_transfer();
    _spi->writeBytes(data, len);
    this->spi_end_transfer();
}

void EPD_HAL_Arduino::send_data_repeat(const uint8_t data, uint32_t repeat)
{
    digitalWrite(_dc, HIGH);
    this->spi_start_transfer();
    _spi->writePattern(&data, 1, repeat);
    this->spi_end_transfer();
}

void EPD_HAL_Arduino::wait_busy()
{
    while (HIGH == digitalRead(_busy))
    { // 1: busy, 0: idle
        // Wait till epd is not busy anymore
    }
}

/// @brief wait until the epaper display is ready again
/// @param timeout in ms 
/// @return true if display is ready, false if timeout occured.
bool EPD_HAL_Arduino::wait_busy(uint64_t timeout)
{
    ESP_LOGD(TAG, "Wait busy");
    timeout += millis();
    while (HIGH == digitalRead(_busy))
    { // 1: busy, 0: idle
        // Wait till epd is not busy anymore or timeout
        if (millis() > timeout)
        {
            return false;
        }
    }
    return true;
}

inline void EPD_HAL_Arduino::set_reset_pin(bool state)
{
    digitalWrite(_rst, state);
}

inline void EPD_HAL_Arduino::delay_ms(uint32_t ms)
{
    delay(ms);
}

inline void EPD_HAL_Arduino::spi_start_transfer()
{
    _spi->beginTransaction(this->_spi_settings);
    digitalWrite(_cs, LOW);
}

inline void EPD_HAL_Arduino::spi_end_transfer()
{
    digitalWrite(_cs, HIGH);
    _spi->endTransaction();
}


void EPD_HAL_Arduino::select_spi(SPIClass &spi)
{
    _spi = &spi;
}
