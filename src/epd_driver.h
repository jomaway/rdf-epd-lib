#ifndef EPD_DRIVER_H
#define EPD_DRIVER_H
#include "epd_hal.h"


namespace DisplayCmd {
      // System Control
      constexpr uint8_t DRIVER_OUTPUT_CONTROL         = 0x01; // Driver Output control Gate setting
      constexpr uint8_t GATE_DRIVING_VOLTAGE_CONTROL  = 0x03; // Gate Driving voltage Control
      constexpr uint8_t SOURCE_DRIVING_VOLTAGE_CONTROL= 0x04; // Source Driving voltage Control
      constexpr uint8_t INITIAL_CODE_SETTING          = 0x08; // OTP Program Initial Code Setting
      constexpr uint8_t WRITE_INIT_CODE               = 0x09; // Write Register for Initial Code Setting
      constexpr uint8_t READ_INIT_CODE                = 0x0A; // Read Register for Initial Code Setting
      constexpr uint8_t BOOSTER_SOFT_START            = 0x0C; // Booster Soft start
      constexpr uint8_t DEEP_SLEEP_MODE               = 0x10; // Deep Sleep Mode Control
      constexpr uint8_t DATA_ENTRY_MODE               = 0x11; // Data Entry Mode Setting
      constexpr uint8_t SOFTWARE_RESET                = 0x12; // SW RESET
      constexpr uint8_t HV_READY_DETECTION            = 0x14; // HV Ready Detection
      constexpr uint8_t VCI_DETECTION                 = 0x15; // VCI Detection
  
      // OTP Programming
      constexpr uint8_t PROGRAM_WS_PASSWORD           = 0x16; // Program WS Password to OTP
      constexpr uint8_t PROGRAM_AUTO_OTP              = 0x17; // Automated OTP programming
  
      // Temperature Sensor
      constexpr uint8_t TEMP_SENSOR_CONTROL           = 0x18; // Temperature Sensor Selection
      constexpr uint8_t TEMP_REG_WRITE                = 0x1A; // Write to Temperature Register
      constexpr uint8_t TEMP_REG_READ                 = 0x1B; // Read from Temperature Register
      constexpr uint8_t TEMP_EXT_CMD_WRITE            = 0x1C; // Write Command to External Temp. Sensor
  
      // Display Update
      constexpr uint8_t MASTER_ACTIVATION             = 0x20; // Master Activation
      constexpr uint8_t DISP_UPDATE_CTRL_1            = 0x21; // Display Update Control 1
      constexpr uint8_t DISP_UPDATE_CTRL_2            = 0x22; // Display Update Control 2
  
      // RAM Operations
      constexpr uint8_t WRITE_RAM_BW                  = 0x24; // Write RAM (Black/White)
      constexpr uint8_t WRITE_RAM_RED                 = 0x26; // Write RAM (Red)
      constexpr uint8_t READ_RAM                      = 0x27; // Read RAM
      constexpr uint8_t READ_RAM_OPTION               = 0x41; // Read RAM Option
  
      // VCOM Operations
      constexpr uint8_t VCOM_SENSE                    = 0x28; // VCOM Sensing
      constexpr uint8_t VCOM_SENSE_DURATION           = 0x29; // VCOM Sense Duration
      constexpr uint8_t PROGRAM_VCOM_OTP              = 0x2A; // Program VCOM to OTP
      constexpr uint8_t WRITE_VCOM_REGISTER           = 0x2C; // Write VCOM Register
  
      // OTP Reads
      constexpr uint8_t READ_DISPLAY_OPTION           = 0x2D; // OTP Register Read for Display Option
      constexpr uint8_t READ_USER_ID                  = 0x2E; // User ID Read
  
      // Waveform LUT
      constexpr uint8_t PROGRAM_WS_OTP                = 0x30; // Program WS OTP (Waveform Setting)
      constexpr uint8_t LOAD_WS_OTP                   = 0x31; // Load WS from OTP
      constexpr uint8_t WRITE_LUT_REGISTER            = 0x32; // Write LUT Register
  
      // CRC
      constexpr uint8_t CRC_CALCULATION               = 0x34; // CRC Calculation
      constexpr uint8_t CRC_STATUS_READ               = 0x35; // CRC Status Read
  
      // OTP Selection & Display Option
      constexpr uint8_t PROGRAM_OTP_SELECTION         = 0x36; // Program OTP Selection
      constexpr uint8_t WRITE_DISPLAY_OPTION          = 0x37; // Write Register for Display Option
      constexpr uint8_t WRITE_USER_ID                 = 0x38; // Write User ID
      constexpr uint8_t OTP_PROGRAM_MODE              = 0x39; // OTP Program mode
  
      // Border Waveform Control
      constexpr uint8_t BORDER_WAVEFORM_CTRL          = 0x3C; // Select border waveform
      // End Option
      constexpr uint8_t END_OPTION                    = 0x3F; // End Option (EOPT)
  
      // Window & Pointer
      constexpr uint8_t SET_RAM_X_ADDRESS             = 0x44; // Set RAM X Address Start/End
      constexpr uint8_t SET_RAM_Y_ADDRESS             = 0x45; // Set RAM Y Address Start/End
      constexpr uint8_t SET_RAM_X_COUNTER             = 0x4E; // Set RAM X Address Counter
      constexpr uint8_t SET_RAM_Y_COUNTER             = 0x4F; // Set RAM Y Address Counter
  
      // Auto Write Pattern
      constexpr uint8_t AUTO_WRITE_RED_RAM            = 0x46; // Auto Write RED RAM (Regular Pattern)
      constexpr uint8_t AUTO_WRITE_BW_RAM             = 0x47; // Auto Write BW RAM (Regular Pattern)
  
      // No operation
      constexpr uint8_t NOP                           = 0x7F; // No operation without effect
}

namespace ImageColor {
    constexpr uint8_t BLACK = 0;
    constexpr uint8_t RED = 1;
}

namespace Status
{
    constexpr bool FAILURE = false;
    constexpr bool SUCCESS = true;
} // namespace Status


class EPD_Driver {
    protected:
        EPD_HAL* hal; // Uses hardware abstraction layer
        const int width = 400;  // Display width in pixels
        const int height = 300; // Display height in pixels
        bool use_fast_update = false;
        void enable_fast_update();
        void disable_fast_update();
        bool set_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        bool set_cursor(uint16_t x, uint16_t y);
        void _write_framebuffer(uint8_t value, bool use_red_ram);
    public:
        EPD_Driver(EPD_HAL* hal) : hal(hal) {}
        void init();
        void update(bool fast);
        void clear();
        void write_framebuffer(const uint8_t *data, bool use_red_ram);
        void write_framebuffer_partial(const uint8_t *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void display(const uint8_t *image, bool use_red_ram, bool fast);
        void sleep();
        void reset();
};

#endif


