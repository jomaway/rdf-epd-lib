#include "epd_driver.h"

void EPD_Driver::init()
{
    // init pins and spi in EPD_HAL
    hal->init();

    // EPD hardware init
    hal->reset();
    hal->wait_busy();

    hal->send_command(SW_RESET); // Software reset to default values
    hal->wait_busy();

    // hal->send_command(DISP_UPDATE_CTRL_1);    // Display update command
    // hal->send_data(0x40);       // RED: Inverse RAM content
    // hal->send_data(0x00);       // BW: Normal

    hal->send_command(BORDER_CTRL); // 0x3C Border Waveform Control
    hal->send_data(0x05);           // Default waveform setting for border LUT1

    hal->send_command(SELECT_TEMP_SENSOR); // 0x18
    hal->send_data(0x80); // Use internal temp sensor

    hal->send_command(DATA_MODE); // 0x11 - Data Entry Mode
    hal->send_data(0x03);         // 0b0011 - x: increment (left to right) y: increment (top to bottom)

    hal->send_command(SET_X_RANGE); // Set RAM x address
    hal->send_data(0x00);           // A[5:0] start: 0x00  [default]
    hal->send_data(0x31);           // B[5:0] end: 0x31    [default]  value: (width / 8-1)

    hal->send_command(SET_Y_RANGE); // Set RAM y address
    hal->send_data(0x00);           // A[7:0]
    hal->send_data(0x00);           // A[8] -> start: 0x00  [default]
    hal->send_data(0x2B);           // B[7:0]                               value: ((height -1)%256)
    hal->send_data(0x01);           // B[8] -> end: 0x12B    [default]      value: ((height-1)/ 256)

    hal->send_command(SET_X_COUNTER); // Set RAM x address counter
    hal->send_data(0x00);             // A[5:0]

    hal->send_command(SET_Y_COUNTER); // Set RAM y address counter
    hal->send_data(0x00);             // A[7:0]
    hal->send_data(0x00);             // A[8]
    hal->wait_busy();
}

void EPD_Driver::update()
{
    hal->send_command(DISP_UPDATE_CTRL_2);
    hal->send_data(0xF7); // EN ANALOG, LOAD TEMP, DISP MODE 1, DIS ANALOG, DIS OSC
    hal->send_command(MASTER_ACTIVATION);
    hal->wait_busy();
}

void EPD_Driver::clear()
{
    uint32_t w = (this->width % 8 == 0) ? (this->width / 8) : (this->width / 8 + 1);

    hal->send_command(WRITE_RAM_BW);
    for (uint32_t j = 0; j < this->height; j++)
    {
        for (uint32_t i = 0; i < w; i++)
        {
            hal->send_data(0xff);
        }
    }

    hal->send_command(WRITE_RAM_RED);
    for (uint32_t j = 0; j < this->height; j++)
    {
        for (uint32_t i = 0; i < w; i++)
        {
            hal->send_data(0x00);
        }
    }
    this->update();
}

void EPD_Driver::display(const byte *image, uint8_t image_color, bool update)
{
    uint32_t w = (this->width % 8 == 0) ? (this->width / 8) : (this->width / 8 + 1);

    // select RAM
    if (RED_IMAGE == image_color)
    {
        hal->send_command(WRITE_RAM_RED);
    }
    else if (BLACK_IMAGE == image_color)
    {
        hal->send_command(WRITE_RAM_BW);
    }
    else
    {
        return; // return early because of error.
    }

    // Send all pixels
    for (uint32_t j = 0; j < this->height; j++)
    {
        for (uint32_t i = 0; i < w; i++)
        {
            hal->send_data(image[i + j * w]);
        }
    }

    // update display
    if (update)
    {
        this->update();
    }
}

void EPD_Driver::showImage(const byte *image, ImageColor_t color)
{
     /* This clears the SRAM of the e-paper display */
    this->clear();

    // draw here!
    this->display(image, color);

    /* Deep sleep */
    this->sleep();
}

void EPD_Driver::sleep()
{
    hal->send_command(SET_SLEEP_MODE);
    hal->send_data(0x01); // Deep Sleep Mode
}
