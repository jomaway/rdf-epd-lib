#ifndef EPD_DRIVER_H
#define EPD_DRIVER_H
#include "epd_hal.h"

constexpr byte SET_SLEEP_MODE = 0x10;
constexpr byte DATA_MODE = 0x11;
constexpr byte SW_RESET = 0x12;
constexpr byte SELECT_TEMP_SENSOR = 0x18;
constexpr byte MASTER_ACTIVATION = 0x20;
constexpr byte DISP_UPDATE_CTRL_1 = 0x21;
constexpr byte DISP_UPDATE_CTRL_2 = 0x22;
constexpr byte WRITE_RAM_BW = 0x24;
constexpr byte WRITE_RAM_RED = 0x26;
constexpr byte BORDER_CTRL = 0x3C;
constexpr byte SET_X_RANGE = 0x44;
constexpr byte SET_Y_RANGE = 0x45;
constexpr byte SET_X_COUNTER = 0x4E;
constexpr byte SET_Y_COUNTER = 0xFE;

constexpr uint8_t BLACK_IMAGE = 0;
constexpr uint8_t RED_IMAGE = 1;

typedef enum{
    BLACK = 0,
    RED = 1,
} ImageColor_t;

class EPD_Driver {
    protected:
        EPD_HAL* hal; // Uses hardware abstraction layer
        const int width = 400;  // Display width in pixels
        const int height = 300; // Display height in pixels
    public:
        EPD_Driver(EPD_HAL* hal) : hal(hal) {}
        void init();
        void clear();
        void display(const byte *image, uint8_t image_color, bool update = true);
        void showImage(const byte *image, ImageColor_t color);
        void update();
        void sleep();
};

#endif


