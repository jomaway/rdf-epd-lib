#include "ssd1683.h"

#ifdef ESP32
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char *TAG = "RDF_EPD_DRIVER";
#endif

namespace SSD1683Command
{
    // System Control
    constexpr uint8_t DRIVER_OUTPUT_CONTROL = 0x01;          // Driver Output control Gate setting
    constexpr uint8_t GATE_DRIVING_VOLTAGE_CONTROL = 0x03;   // Gate Driving voltage Control
    constexpr uint8_t SOURCE_DRIVING_VOLTAGE_CONTROL = 0x04; // Source Driving voltage Control
    constexpr uint8_t INITIAL_CODE_SETTING = 0x08;           // OTP Program Initial Code Setting
    constexpr uint8_t WRITE_INIT_CODE = 0x09;                // Write Register for Initial Code Setting
    constexpr uint8_t READ_INIT_CODE = 0x0A;                 // Read Register for Initial Code Setting
    constexpr uint8_t BOOSTER_SOFT_START = 0x0C;             // Booster Soft start
    constexpr uint8_t DEEP_SLEEP_MODE = 0x10;                // Deep Sleep Mode Control
    constexpr uint8_t DATA_ENTRY_MODE = 0x11;                // Data Entry Mode Setting
    constexpr uint8_t SOFTWARE_RESET = 0x12;                 // SW RESET
    constexpr uint8_t HV_READY_DETECTION = 0x14;             // HV Ready Detection
    constexpr uint8_t VCI_DETECTION = 0x15;                  // VCI Detection

    // OTP Programming
    constexpr uint8_t PROGRAM_WS_PASSWORD = 0x16; // Program WS Password to OTP
    constexpr uint8_t PROGRAM_AUTO_OTP = 0x17;    // Automated OTP programming

    // Temperature Sensor
    constexpr uint8_t TEMP_SENSOR_CONTROL = 0x18; // Temperature Sensor Selection
    constexpr uint8_t TEMP_REG_WRITE = 0x1A;      // Write to Temperature Register
    constexpr uint8_t TEMP_REG_READ = 0x1B;       // Read from Temperature Register
    constexpr uint8_t TEMP_EXT_CMD_WRITE = 0x1C;  // Write Command to External Temp. Sensor

    // Display Update
    constexpr uint8_t MASTER_ACTIVATION = 0x20;  // Master Activation
    constexpr uint8_t DISP_UPDATE_CTRL_1 = 0x21; // Display Update Control 1
    constexpr uint8_t DISP_UPDATE_CTRL_2 = 0x22; // Display Update Control 2

    // RAM Operations
    constexpr uint8_t WRITE_RAM_BW = 0x24;    // Write RAM (Black/White)
    constexpr uint8_t WRITE_RAM_RED = 0x26;   // Write RAM (Red)
    constexpr uint8_t READ_RAM = 0x27;        // Read RAM
    constexpr uint8_t READ_RAM_OPTION = 0x41; // Read RAM Option

    // VCOM Operations
    constexpr uint8_t VCOM_SENSE = 0x28;          // VCOM Sensing
    constexpr uint8_t VCOM_SENSE_DURATION = 0x29; // VCOM Sense Duration
    constexpr uint8_t PROGRAM_VCOM_OTP = 0x2A;    // Program VCOM to OTP
    constexpr uint8_t WRITE_VCOM_REGISTER = 0x2C; // Write VCOM Register

    // OTP Reads
    constexpr uint8_t READ_DISPLAY_OPTION = 0x2D; // OTP Register Read for Display Option
    constexpr uint8_t READ_USER_ID = 0x2E;        // User ID Read

    // Waveform LUT
    constexpr uint8_t PROGRAM_WS_OTP = 0x30;     // Program WS OTP (Waveform Setting)
    constexpr uint8_t LOAD_WS_OTP = 0x31;        // Load WS from OTP
    constexpr uint8_t WRITE_LUT_REGISTER = 0x32; // Write LUT Register

    // CRC
    constexpr uint8_t CRC_CALCULATION = 0x34; // CRC Calculation
    constexpr uint8_t CRC_STATUS_READ = 0x35; // CRC Status Read

    // OTP Selection & Display Option
    constexpr uint8_t PROGRAM_OTP_SELECTION = 0x36; // Program OTP Selection
    constexpr uint8_t WRITE_DISPLAY_OPTION = 0x37;  // Write Register for Display Option
    constexpr uint8_t WRITE_USER_ID = 0x38;         // Write User ID
    constexpr uint8_t OTP_PROGRAM_MODE = 0x39;      // OTP Program mode

    // Border Waveform Control
    constexpr uint8_t BORDER_WAVEFORM_CTRL = 0x3C; // Select border waveform
    // End Option
    constexpr uint8_t END_OPTION = 0x3F; // End Option (EOPT)

    // Window & Pointer
    constexpr uint8_t SET_RAM_X_ADDRESS = 0x44; // Set RAM X Address Start/End
    constexpr uint8_t SET_RAM_Y_ADDRESS = 0x45; // Set RAM Y Address Start/End
    constexpr uint8_t SET_RAM_X_COUNTER = 0x4E; // Set RAM X Address Counter
    constexpr uint8_t SET_RAM_Y_COUNTER = 0x4F; // Set RAM Y Address Counter

    // Auto Write Pattern
    constexpr uint8_t AUTO_WRITE_RED_RAM = 0x46; // Auto Write RED RAM (Regular Pattern)
    constexpr uint8_t AUTO_WRITE_BW_RAM = 0x47;  // Auto Write BW RAM (Regular Pattern)

    // No operation
    constexpr uint8_t NOP = 0x7F; // No operation without effect
}

/**
 * @brief Constructor for the EPD_Driver class.
 *
 * This constructor initializes the EPD driver with a hardware abstraction layer (HAL)
 */
SSD1683::SSD1683(EPD_HAL_INTERFACE *hal, const uint16_t width, const uint16_t height, ColorMode colorMode) : hal(hal), width(width), height(height), colorMode(colorMode)
{
    ESP_LOGD(TAG, "Create EPD_Object");
    hal->init();
}

/**
 * @brief Initializes the EPD driver and the display controller.
 *
 * This function initializes the hardware abstraction layer (HAL) for the EPD,
 * resets the display, and configures various settings such as border waveform,
 * temperature sensor control, and data entry mode.
 */
void SSD1683::init()
{
    ESP_LOGD(TAG, "init EPD");
    this->reset();
    hal->wait_busy();

    hal->send_command(SSD1683Command::SOFTWARE_RESET); // Software reset to default values
    hal->wait_busy();

    if (colorMode == ColorMode::BlackWhite)
    {
        hal->send_command(SSD1683Command::DISP_UPDATE_CTRL_1);
        hal->send_data(0x40); // RED: Bypass RED RAM content (no red color)
        hal->send_data(0x00);
    }

    hal->send_command(SSD1683Command::BORDER_WAVEFORM_CTRL);
    hal->send_data(0x05); // Default waveform setting for border LUT1

    hal->send_command(SSD1683Command::TEMP_SENSOR_CONTROL);
    hal->send_data(0x80); // Use internal temp sensor

    hal->send_command(SSD1683Command::DATA_ENTRY_MODE);
    hal->send_data(0x03); // 0b0011 - x: increment (left to right), y: increment (top to bottom)  : normal mode

    this->set_window(0, 0, this->width, this->height);
    this->set_cursor(0, 0);

    hal->wait_busy();
}

/**
 * @brief Enables fast update mode for the display.
 */
void SSD1683::enable_fast_update()
{
    if (!use_fast_update)
    {
        ESP_LOGD(TAG, "enable fast update");

        // Set temp for fast update
        hal->send_command(SSD1683Command::TEMP_REG_WRITE);
        hal->send_data(0x5A); // 90°

        // Load LUT for temp value
        hal->send_command(SSD1683Command::DISP_UPDATE_CTRL_2);
        hal->send_data((colorMode == ColorMode::BlackWhite) ? 0x99 : 0x91);

        hal->send_command(SSD1683Command::MASTER_ACTIVATION);
        hal->wait_busy();

        use_fast_update = true;
    }
}

/**
 * @brief Disables fast update mode for the display.
 */
void SSD1683::disable_fast_update()
{
    ESP_LOGD(TAG, "disable fast update");
    use_fast_update = false;
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
bool SSD1683::set_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ESP_LOGD(TAG, "set window");
    // check if out of bound
    if ((x + w) > this->width || (y + h) > this->height)
    {
        ESP_LOGE(TAG, "EPD_DRIVER::set_window() -> ERROR");
        return false;
    }

    hal->send_command(SSD1683Command::SET_RAM_X_ADDRESS);
    hal->send_data((x >> 3) & 0xFF); // 1 byte are 8 pixel
    hal->send_data(((x + w - 1) >> 3) & 0xFF);

    hal->send_command(SSD1683Command::SET_RAM_Y_ADDRESS);
    hal->send_data(y & 0xFF);
    hal->send_data((y >> 8) & 0xFF);
    hal->send_data((y + h - 1) & 0xFF);
    hal->send_data((y + h - 1) >> 8 & 0xFF);

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
bool SSD1683::set_cursor(uint16_t x, uint16_t y)
{
    ESP_LOGD(TAG, "set cursor");
    // check if out of bound
    if (x > this->width || y > this->height)
    {
        ESP_LOGD(TAG, "EPD_DRIVER::set_cursor() -> ERROR");
        return false;
    }

    hal->send_command(SSD1683Command::SET_RAM_X_COUNTER);
    hal->send_data((x >> 3) & 0xFF);

    hal->send_command(SSD1683Command::SET_RAM_Y_COUNTER);
    hal->send_data(y & 0xFF);
    hal->send_data((y >> 8) & 0xFF);

    return true;
}

/// @brief Updates the epapers screen with the current RAM content.
/// @param mode selects Normal or Fast update mode
void SSD1683::update(UpdateMode mode)
{
    switch (mode)
    {
    case UpdateMode::Normal:
        ESP_LOGD(TAG, "NORMAL update");
        hal->send_command(SSD1683Command::DISP_UPDATE_CTRL_2);
        // EN CLOCK, EN ANALOG, DISP (0x_F BW MODE/ 0x_7 3C MODE), DIS ANALOG, DIS OSC
        // hal->send_data( (colorMode == ColorMode::BlackWhite) ? 0xFF : 0xF7);
        hal->send_data(0xF7);
        break;

    case UpdateMode::Fast:
        ESP_LOGD(TAG, "FAST update");
        enable_fast_update();

        hal->send_command(SSD1683Command::DISP_UPDATE_CTRL_2);
        // EN CLOCK, EN ANALOG, DISP (0x_F BW MODE/ 0x_7 3C MODE), DIS ANALOG, DIS OSC
        // hal->send_data( (colorMode == ColorMode::BlackWhite) ? 0xCF : 0xC7);
        hal->send_data(0xCF);
        break;
    }

    hal->send_command(SSD1683Command::MASTER_ACTIVATION);
    hal->wait_busy();
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
void SSD1683::fill_framebuffer(uint8_t value, RAM selectedRAM)
{
    ESP_LOGD(TAG, "fill framebuffer with value");
    const uint32_t buffer_size = this->height * this->width / 8;

    set_window(0, 0, this->width, this->height);
    set_cursor(0, 0);

    hal->send_command((selectedRAM == RAM::Red) ? SSD1683Command::WRITE_RAM_RED : SSD1683Command::WRITE_RAM_BW);
    hal->send_data_repeat(value, buffer_size);
}

/**
 * @brief Clears the display by filling the framebuffer with 0xFF (white).
 *
 * This function clears both the black/white and red RAM locations.
 * It then updates the display to reflect the cleared state.
 */
void SSD1683::clear()
{
    ESP_LOGD(TAG, "clear both framebuffers");
    // clear BW ram location
    this->fill_framebuffer(0xFF, RAM::BW);

    // clear red ram location
    this->fill_framebuffer(0x00, RAM::Red);

    // update screen
    this->update(UpdateMode::Normal); // always make a full update on clear;
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
void SSD1683::write_framebuffer(const uint8_t *data, RAM selectedRAM)
{
    ESP_LOGD(TAG, "write framebuffer to EPD.");
    uint32_t w = (this->width % 8 == 0) ? (this->width / 8) : (this->width / 8 + 1);
    const uint32_t buffer_size = this->height * this->width / 8;
    set_window(0, 0, this->width, this->height);
    set_cursor(0, 0);

    // select RAM
    hal->send_command((selectedRAM == RAM::Red) ? SSD1683Command::WRITE_RAM_RED : SSD1683Command::WRITE_RAM_BW);
    // Send all pixels
    hal->send_data(data, buffer_size);
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
void SSD1683::write_framebuffer_partial(const uint8_t *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ESP_LOGD(TAG, "write partial framebuffer to EPD.");
    if (!set_window(x, y, w, h) || !set_cursor(x, y))
    {
        return;
    }

    const uint32_t buffer_size = h * w / 8;
    hal->send_command(SSD1683Command::WRITE_RAM_BW);
    hal->send_data(data, buffer_size);
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
void SSD1683::display(const uint8_t *image, RAM selectedRAM, UpdateMode mode)
{
    ESP_LOGD(TAG, "display full image");
    // write image to display controller ram
    this->write_framebuffer(image, selectedRAM);

    // update display
    this->update(mode);
}

/**
 * @brief Displays a partial image on the screen.
 *
 * This function writes a portion of the framebuffer data to the display controller's RAM
 * and then uses a fast update for updating the display.
 */
void SSD1683::display_partial(const uint8_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    ESP_LOGD(TAG, "Display partial:");
    // write image to display controller ram
    this->write_framebuffer_partial(image, x, y, w, h);

    // update display
    this->update();
}

/**
 * @brief Puts the display into deep sleep mode.
 *
 * This function sends the command to put the display into deep sleep mode,
 * which reduces power consumption when the display is not in use.
 *
 * To wake the display from deep sleep, the init() function should be called again.
 */
void SSD1683::sleep()
{
    ESP_LOGD(TAG, "Send EPD to sleep");
    hal->send_command(SSD1683Command::DEEP_SLEEP_MODE);
    hal->send_data(0x01); // Deep Sleep Mode
}

/**
 * @brief Resets the display controller.
 *
 * This function performs a hardware reset of the display controller,
 * which is typically required after power-up or when waking from deep sleep.
 */
void SSD1683::reset()
{
    ESP_LOGD(TAG, "Reset EPD");
    hal->set_reset_pin(HIGH);
    hal->delay_ms(200);
    hal->set_reset_pin(LOW);
    hal->delay_ms(2);
    hal->set_reset_pin(HIGH);
    hal->delay_ms(200);
}