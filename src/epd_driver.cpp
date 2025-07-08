#include "epd_driver.h"

#ifdef ESP32
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char *TAG = "RDF_EPD_DRIVER";
#endif

/**
 * @brief Constructor for the EPD_Driver class.
 *
 * This constructor initializes the EPD driver with a hardware abstraction layer (HAL)
 */
EPD_Driver::EPD_Driver(EPD_HAL *hal) : hal(hal), use_fast_update(false)
{
    ESP_LOGD(TAG, "EPD_DRIVER()");
    hal->init();
}

/**
 * @brief Initializes the EPD driver and the display controller.
 *
 * This function initializes the hardware abstraction layer (HAL) for the EPD,
 * resets the display, and configures various settings such as border waveform,
 * temperature sensor control, and data entry mode.
 */
void EPD_Driver::init()
{
    ESP_LOGD(TAG, "EPD_DRIVER::init() -> START");
    this->reset();
    hal->wait_busy();

    hal->send_command(DisplayCmd::SOFTWARE_RESET); // Software reset to default values
    hal->wait_busy();

    hal->send_command(DisplayCmd::BORDER_WAVEFORM_CTRL);
    hal->send_data_byte(0x05); // Default waveform setting for border LUT1

    hal->send_command(DisplayCmd::TEMP_SENSOR_CONTROL);
    hal->send_data_byte(0x80); // Use internal temp sensor

    hal->send_command(DisplayCmd::DATA_ENTRY_MODE);
    hal->send_data_byte(0x03); // 0b0011 - x: increment (left to right), y: increment (top to bottom)  : normal mode

    this->set_window(0, 0, this->width, this->height);
    this->set_cursor(0, 0);

    hal->wait_busy();
    ESP_LOGD(TAG, "EPD_DRIVER::init() -> END");
}

/**
 * @brief Enables fast update mode for the display.
 */
void EPD_Driver::enable_fast_update()
{
    if (false == this->use_fast_update)
    {
        ESP_LOGD(TAG, "EPD_DRIVER::en_fast() -> START");

        // Set temp for fast update
        hal->send_command(DisplayCmd::TEMP_REG_WRITE);
        hal->send_data_byte(0x5a); // 90

        // Load LUT for temp value
        hal->send_command(DisplayCmd::DISP_UPDATE_CTRL_2);
        hal->send_data_byte(0x91);
        hal->send_command(DisplayCmd::MASTER_ACTIVATION);
        hal->wait_busy();

        this->use_fast_update = true;
        ESP_LOGD(TAG, "EPD_DRIVER::en_fast() -> END");
    }
}

/**
 * @brief Disables fast update mode for the display.
 */
void EPD_Driver::disable_fast_update()
{
    ESP_LOGD(TAG, "EPD_DRIVER::disable_fast()");
    this->use_fast_update = false;
}

/**
 * @brief Sets the drawing window area in display RAM.
 *
 * This function configures the X and Y address ranges on the display controller,
 * effectively defining a rectangular area where pixel data will be written.
 * The coordinates are specified in pixels, and X values are automatically
 * aligned to byte boundaries (8 pixels per byte).
 *
 * @param x The starting X coordinate (in pixels).
 * @param y The starting Y coordinate (in pixels).
 * @param w The width of the window (in pixels).
 * @param h The height of the window (in pixels).
 *
 * @return true if the window was successfully set, false if the specified
 *         region exceeds the display bounds.
 *
 * @note X coordinates are converted to byte-aligned values (1 byte = 8 pixels).
 *
 * @example
 * @code
 * // Set the full window on a 400x300 display
 * set_window(0, 0, 400, 300);
 * @endcode
 */
bool EPD_Driver::set_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ESP_LOGD(TAG, "EPD_DRIVER::set_window() -> START");
    // check if out of bound
    if ((x + w) > this->width || (y + h) > this->height)
    {
        ESP_LOGE(TAG, "EPD_DRIVER::set_window() -> ERROR");
        return false;
    }

    hal->send_command(DisplayCmd::SET_RAM_X_ADDRESS);
    hal->send_data_byte((x >> 3) & 0xFF); // 1 byte are 8 pixel
    hal->send_data_byte(((x + w - 1) >> 3) & 0xFF);

    hal->send_command(DisplayCmd::SET_RAM_Y_ADDRESS);
    hal->send_data_byte(y & 0xFF);
    hal->send_data_byte((y >> 8) & 0xFF);
    hal->send_data_byte((y + h - 1) & 0xFF);
    hal->send_data_byte((y + h - 1) >> 8 & 0xFF);

    ESP_LOGD(TAG, "EPD_DRIVER::set_window() -> END");
    return true;
}

/**
 * @brief Sets the cursor position for drawing on the display.
 *
 * This function sets the cursor position in the display RAM, allowing
 * subsequent pixel data to be written at the specified coordinates.
 * The coordinates are specified in pixels.
 *
 * @param x The X coordinate (in pixels).
 * @param y The Y coordinate (in pixels).
 *
 * @return true if the cursor was successfully set, false if the specified
 *         coordinates exceed the display bounds.
 */
bool EPD_Driver::set_cursor(uint16_t x, uint16_t y)
{
    ESP_LOGD(TAG, "EPD_DRIVER::set_cursor() -> START");
    // check if out of bound
    if (x > this->width || y > this->height)
    {
        ESP_LOGD(TAG, "EPD_DRIVER::set_cursor() -> ERROR");
        return false;
    }

    hal->send_command(DisplayCmd::SET_RAM_X_COUNTER);
    hal->send_data_byte((x >> 3) & 0xFF);

    hal->send_command(DisplayCmd::SET_RAM_Y_COUNTER);
    hal->send_data_byte(y & 0xFF);
    hal->send_data_byte((y >> 8) & 0xFF);

    ESP_LOGD(TAG, "EPD_DRIVER::set_cursor() -> END");
    return true;
}

/**
 * * @brief Updates the epapers screen with the current RAM content.
 */
void EPD_Driver::update(bool fast)
{
    if (fast)
    {
        ESP_LOGD(TAG, "EPD_DRIVER::update() -> FAST");
        if (!this->use_fast_update)
        {
            this->enable_fast_update();
        }

        hal->send_command(DisplayCmd::DISP_UPDATE_CTRL_2);
        hal->send_data_byte(0xC7); // EN ANALOG, DISP COLOR MODE, DIS ANALOG, DIS OSC
    }
    else
    {
        ESP_LOGD(TAG, "EPD_DRIVER::update() -> NORMAL");
        this->use_fast_update = false; // make sure that before the next fast update it gets enabled again.

        hal->send_command(DisplayCmd::DISP_UPDATE_CTRL_2);
        hal->send_data_byte(0xF7); // EN ANALOG, LOAD TEMP, LOAD LUT, DISP COLOR MODE, DIS ANALOG, DIS OSC
    }

    ESP_LOGD(TAG, "EPD_DRIVER::update() -> ACTIVATE");
    hal->send_command(DisplayCmd::MASTER_ACTIVATION);
    ESP_LOGD(TAG, "EPD_DRIVER::update() -> ACTIVATE SEND");
    hal->wait_busy();
    ESP_LOGD(TAG, "EPD_DRIVER::update() -> END");
}

/**
 * @brief Writes a single value to the framebuffer.
 *
 * This function fills the entire framebuffer with the specified value.
 * It is typically used for clearing the display or setting a uniform color.
 *
 * @param value The value to write to the framebuffer (0x00 for black/red, 0xFF for white).
 * @param use_red_ram If true, writes to the red RAM; otherwise, writes to the black/white RAM.
 */
void EPD_Driver::_write_framebuffer(uint8_t value, bool use_red_ram)
{
    ESP_LOGD(TAG, "EPD_DRIVER::_write_framebuffer() -> START");
    const uint32_t buffer_size = this->height * this->width / 8;

    set_window(0, 0, this->width, this->height);
    set_cursor(0, 0);

    hal->send_command(use_red_ram ? DisplayCmd::WRITE_RAM_RED : DisplayCmd::WRITE_RAM_BW);
    hal->send_data_repeat(value, buffer_size);
    ESP_LOGD(TAG, "EPD_DRIVER::_write_framebuffer() -> END");
}

/**
 * @brief Clears the display by filling the framebuffer with 0xFF (white).
 *
 * This function clears both the black/white and red RAM locations.
 * It then updates the display to reflect the cleared state.
 */
void EPD_Driver::clear()
{
    ESP_LOGD(TAG, "EPD_DRIVER::clear() -> START");
    // clear BW ram location
    this->_write_framebuffer(0xFF, false);

    // clear red ram location
    this->_write_framebuffer(0x00, true);

    // update screen
    this->update(false); // always make a full update on clear
    ESP_LOGD(TAG, "EPD_DRIVER::clear() -> END");
}

/**
 * @brief Writes a framebuffer to the display.
 *
 * This function sends the framebuffer data to the display controller's RAM.
 * It can write to either the black/white or red RAM, depending on the use_red_ram parameter.
 *
 * @param data Pointer to the framebuffer data. This needs to match the display resolution.
 *             The data should be in the format expected by the display (1 bit per pixel).
 * @param use_red_ram If true, writes to the red RAM; otherwise, writes to the black/white RAM.
 */
void EPD_Driver::write_framebuffer(const uint8_t *data, bool use_red_ram)
{
    ESP_LOGD(TAG, "EPD_DRIVER::write() -> START");
    uint32_t w = (this->width % 8 == 0) ? (this->width / 8) : (this->width / 8 + 1);
    const uint32_t buffer_size = this->height * this->width / 8;
    set_window(0, 0, this->width, this->height);
    set_cursor(0, 0);

    // select RAM
    hal->send_command(use_red_ram ? DisplayCmd::WRITE_RAM_RED : DisplayCmd::WRITE_RAM_BW);
    // Send all pixels
    hal->send_data_bulk(data, buffer_size);
    ESP_LOGD(TAG, "EPD_DRIVER::write() -> END");
}

/**
 * @brief Writes a partial framebuffer to the display.
 *
 * This function sends a portion of the framebuffer data to the display controller's RAM.
 * It is only supported for the black/white RAM.
 *
 * @param data Pointer to the framebuffer data. This needs to match the display resolution.
 *             The data should be in the format expected by the display (1 bit per pixel).
 * @param x The starting X coordinate (in pixels).
 * @param y The starting Y coordinate (in pixels).
 * @param w The width of the window (in pixels).
 * @param h The height of the window (in pixels).
 */
void EPD_Driver::write_framebuffer_partial(const uint8_t *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ESP_LOGD(TAG, "EPD_DRIVER::write_partial() -> START");
    if (!set_window(x, y, w, h) || !set_cursor(x, y))
    {
        return;
    }

    const uint32_t buffer_size = h * w / 8;
    hal->send_command(DisplayCmd::WRITE_RAM_BW);
    hal->send_data_bulk(data, buffer_size);
    ESP_LOGD(TAG, "EPD_DRIVER::write_partial() -> END");
}

/**
 * @brief Displays the image on the screen.
 *
 * This function writes the framebuffer data to the display controller's RAM
 * and then updates the display.
 *
 * @param image Pointer to the framebuffer data. This needs to match the display resolution.
 *              The data should be in the format expected by the display (1 bit per pixel).
 * @param use_red_ram If true, writes to the red RAM; otherwise, writes to the black/white RAM.
 * @param fast If true, uses fast update mode for the display.
 */
void EPD_Driver::display(const uint8_t *image, bool use_red_ram, bool fast)
{
    ESP_LOGD(TAG, "EPD_DRIVER::display() -> START");
    // write image to display controller ram
    this->write_framebuffer(image, use_red_ram);

    // update display
    this->update(fast);
    ESP_LOGD(TAG, "EPD_DRIVER::display() -> END");
}

/**
 * @brief Displays a partial image on the screen.
 *
 * This function writes a portion of the framebuffer data to the display controller's RAM
 * and then uses a fast update for updating the display.
 */
void EPD_Driver::display_partial(const uint8_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ESP_LOGD(TAG, "EPD_DRIVER::display_partial() -> START");
    // write image to display controller ram
    this->write_framebuffer_partial(image, x, y, w, h);

    // update display
    this->update(true);
    ESP_LOGD(TAG, "EPD_DRIVER::display_partial() -> END");
}

/**
 * @brief Puts the display into deep sleep mode.
 *
 * This function sends the command to put the display into deep sleep mode,
 * which reduces power consumption when the display is not in use.
 *
 * To wake the display from deep sleep, the init() function should be called again.
 */
void EPD_Driver::sleep()
{
    ESP_LOGD(TAG, "EPD_DRIVER::sleep() -> START");
    hal->send_command(DisplayCmd::DEEP_SLEEP_MODE);
    hal->send_data_byte(0x01); // Deep Sleep Mode
    ESP_LOGD(TAG, "EPD_DRIVER::sleep() -> END");
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */

/**
 * @brief Resets the display controller.
 *
 * This function performs a hardware reset of the display controller,
 * which is typically required after power-up or when waking from deep sleep.
 */
void EPD_Driver::reset()
{
    ESP_LOGD(TAG, "EPD_DRIVER::reset() -> START");
    hal->set_reset_pin(HIGH);
    hal->delay_ms(200);
    hal->set_reset_pin(LOW);
    hal->delay_ms(2);
    hal->set_reset_pin(HIGH);
    hal->delay_ms(200);
    ESP_LOGD(TAG, "EPD_DRIVER::reset() -> END");
}