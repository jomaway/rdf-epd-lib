#pragma once

#include "epd_hal.h"

enum class UpdateMode {
    Normal,
    Fast,
};

enum class ColorMode {
    BlackWhite,
    ThreeColor,
};

enum class RAM {
    BW,
    Red,
};

/**
 * @brief Interface for controlling an 400x300 e-paper display with SSD1683 controller.
 *
 * This class provides methods for sending a full or partial frame to an e-paper display,
 * abstracting hardware-specific implementation details.
 */
class SSD1683 {
    protected:
        EPD_HAL_INTERFACE* hal; // Uses hardware abstraction layer
        const uint16_t width;  // Display width in pixels
        const uint16_t height; // Display height in pixels
        ColorMode colorMode;
        bool use_fast_update = false;
        void enable_fast_update();
        void disable_fast_update();
        bool set_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        bool set_cursor(uint16_t x, uint16_t y);
        void fill_framebuffer(uint8_t value, RAM ram);
        void reset();
    public:
        SSD1683(EPD_HAL_INTERFACE* hal, const uint16_t width = 400, const uint16_t height = 300, ColorMode colorMode = ColorMode::BlackWhite);
        void init();
        void clear();
        void write_framebuffer(const uint8_t *data, RAM ram = RAM::BW);
        void write_framebuffer_partial(const uint8_t *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void update(UpdateMode mode = UpdateMode::Normal);
        void display(const uint8_t *image, RAM ram = RAM::BW, UpdateMode mode = UpdateMode::Normal);
        void display_partial(const uint8_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void sleep();

        uint16_t get_width() { return this->width; }
        uint16_t get_height() { return this->height; }
};
