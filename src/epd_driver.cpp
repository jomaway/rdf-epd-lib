#include "epd_driver.h"

void EPD_Driver::init()
{
    Serial.println("EPD_DRIVER::init() -> START");
    // init pins and spi in EPD_HAL
    hal->init();

    // EPD hardware init
    this->reset();
    hal->wait_busy();

    hal->send_command(DisplayCmd::SOFTWARE_RESET); // Software reset to default values
    hal->wait_busy();

    // hal->send_command(DISP_UPDATE_CTRL_1);    // Display update command
    // hal->send_data(0x40);       // RED: Inverse RAM content
    // hal->send_data(0x00);       // BW: Normal

    hal->send_command(DisplayCmd::BORDER_WAVEFORM_CTRL); 
    hal->send_data(0x05);           // Default waveform setting for border LUT1

    hal->send_command(DisplayCmd::TEMP_SENSOR_CONTROL);
    hal->send_data(0x80); // Use internal temp sensor

    hal->send_command(DisplayCmd::DATA_ENTRY_MODE); 
    hal->send_data(0x03);         // 0b0011 - x: increment (left to right), y: increment (top to bottom)  : normal mode

    this->set_window(0,0,this->width, this->height);
    this->set_cursor(0,0);

    hal->wait_busy();
    Serial.println("EPD_DRIVER::init() -> END");
}

void EPD_Driver::enable_fast_update()
{
    Serial.println("EPD_DRIVER::en_fast() -> START");
    if (false == this->use_fast_update){

        // Set temp for fast update
        hal->send_command(DisplayCmd::TEMP_REG_WRITE);
        hal->send_data(0x5a); // 90
        
        // Load LUT for temp value
        hal->send_command(DisplayCmd::DISP_UPDATE_CTRL_2);
        hal->send_data(0x91);
        hal->send_command(DisplayCmd::MASTER_ACTIVATION);
        hal->wait_busy();
        
        this->use_fast_update = true;
        Serial.println("EPD_DRIVER::en_fast() -> END");
    }
}

void EPD_Driver::disable_fast_update()
{
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
    Serial.println("EPD_DRIVER::set_window() -> START");
    // check if out of bound
    if ( (x + w) > this->width || (y + h) > this->height)
    {
        Serial.println("EPD_DRIVER::set_window() -> ERROR");
        return false;
    }

    hal->send_command(DisplayCmd::SET_RAM_X_ADDRESS);
    hal->send_data((x >> 3) & 0xFF);    // 1 byte are 8 pixel 
    hal->send_data(((x + w - 1) >> 3) & 0xFF);

    hal->send_command(DisplayCmd::SET_RAM_Y_ADDRESS);
    hal->send_data(y & 0xFF);
    hal->send_data((y >> 8) & 0xFF);
    hal->send_data((y + h - 1) & 0xFF);
    hal->send_data((y + h - 1) >> 8 & 0xFF);

    Serial.println("EPD_DRIVER::set_window() -> END");
    return true;
}


bool EPD_Driver::set_cursor(uint16_t x, uint16_t y)
{
    Serial.println("EPD_DRIVER::set_cursor() -> START");
    // check if out of bound
    if (x > this->width || y > this->height)
    {
        Serial.println("EPD_DRIVER::set_cursor() -> ERROR");
        return false;
    }

    hal->send_command(DisplayCmd::SET_RAM_X_COUNTER);
    hal->send_data((x >> 3) & 0xFF);

    hal->send_command(DisplayCmd::SET_RAM_Y_COUNTER);
    hal->send_data(y & 0xFF);
    hal->send_data((y >> 8) & 0xFF);

    Serial.println("EPD_DRIVER::set_cursor() -> END");
    return true;
}



void EPD_Driver::update(bool fast)
{
    if (fast) 
    {
        Serial.println("EPD_DRIVER::update() -> FAST");
        if (!this->use_fast_update){
            this->enable_fast_update();
        }
   
        hal->send_command(DisplayCmd::DISP_UPDATE_CTRL_2);
        hal->send_data(0xC7); // EN ANALOG, DISP COLOR MODE, DIS ANALOG, DIS OSC
    }
    else 
    {
        Serial.println("EPD_DRIVER::update() -> NORMAL");
        this->use_fast_update = false; // make sure that before the next fast update it gets enabled again.

        hal->send_command(DisplayCmd::DISP_UPDATE_CTRL_2);
        hal->send_data(0xF7); // EN ANALOG, LOAD TEMP, LOAD LUT, DISP COLOR MODE, DIS ANALOG, DIS OSC
    }

    Serial.println("EPD_DRIVER::update() -> ACTIVATE");
    hal->send_command(DisplayCmd::MASTER_ACTIVATION);
    Serial.println("EPD_DRIVER::update() -> ACTIVATE SEND");
    hal->wait_busy();
    Serial.println("EPD_DRIVER::update() -> END");
}

void EPD_Driver::_write_framebuffer(uint8_t value, bool use_red_ram)
{
    Serial.println("EPD_DRIVER::_write_framebuffer() -> START");
    const uint32_t buffer_size = this->height * this->width / 8;

    // set_window(0,0,this->width, this->height);
    // set_cursor(0,0);


    hal->send_command(use_red_ram ? DisplayCmd::WRITE_RAM_RED: DisplayCmd::WRITE_RAM_BW);

    hal->spi_start_transfer();
    for (uint32_t i = 0; i < buffer_size; i++)
    {
        hal->spi_transfer(value);
    }
    hal->spi_end_transfer();
    Serial.println("EPD_DRIVER::_write_framebuffer() -> END");
}

void EPD_Driver::clear()
{
    Serial.println("EPD_DRIVER::clear() -> START");
    // clear BW ram location
    this->_write_framebuffer(0xFF, false);

    // clear red ram location
    this->_write_framebuffer(0x00, true);

    // update screen
    this->update(false); // always make a full update on clear
    Serial.println("EPD_DRIVER::clear() -> END");
}

void EPD_Driver::write_framebuffer(const uint8_t *data, bool use_red_ram)
{
    Serial.println("EPD_DRIVER::write() -> START");
    uint32_t w = (this->width % 8 == 0) ? (this->width / 8) : (this->width / 8 + 1);
    // set_window(0,0,this->width, this->height);
    // set_cursor(0,0);

    // select RAM
    hal->send_command(use_red_ram ? DisplayCmd::WRITE_RAM_RED: DisplayCmd::WRITE_RAM_BW);

    // Send all pixels
    hal->spi_start_transfer();
    for (uint32_t j = 0; j < this->height; j++)
    {
        for (uint32_t i = 0; i < w; i++)
        {
            hal->spi_transfer(data[i + j * w]);
        }
    }
    hal->spi_end_transfer();
    Serial.println("EPD_DRIVER::write() -> END");
}

void EPD_Driver::write_framebuffer_partial(const uint8_t *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    if(!set_window(x,y,w,h) || !set_cursor(x,y)){
        return;
    }

    // TODO
}

void EPD_Driver::display(const uint8_t *image, bool use_red_ram, bool fast)
{
    Serial.println("EPD_DRIVER::display() -> START");
    // write image to display controller ram
    this->write_framebuffer(image, use_red_ram);

    // update display
    this->update(fast);
    Serial.println("EPD_DRIVER::display() -> END");
}


void EPD_Driver::sleep()
{
    Serial.println("EPD_DRIVER::sleep() -> START");
    hal->send_command(DisplayCmd::DEEP_SLEEP_MODE);
    hal->send_data(0x01); // Deep Sleep Mode
    Serial.println("EPD_DRIVER::sleep() -> END");
}

/**
 *  @brief: module reset. 
 *          often used to awaken the module in deep sleep, 
 *          see Epd::Sleep();
 */
void EPD_Driver::reset()
{
    Serial.println("EPD_DRIVER::reset() -> START");
    hal->set_reset_pin(HIGH);
    hal->delay_ms(200);
    hal->set_reset_pin(LOW);
    hal->delay_ms(2);
    hal->set_reset_pin(HIGH);
    hal->delay_ms(200);
    Serial.println("EPD_DRIVER::reset() -> END");
}