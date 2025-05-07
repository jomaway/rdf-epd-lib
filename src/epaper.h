#ifndef epaper_h
#define epaper_h

#include "epd_driver.h"
#include "epd_graphics.h"

// Display resolution
#define EPD_WIDTH       400
#define EPD_HEIGHT      300

class EPaper {
private:
    EPD_HAL *hal;
    EPD_Driver *driver;
public:
    EPaper(unsigned int cs_pin, unsigned int dc_pin, unsigned int reset_pin, unsigned int busy_pin);
    
    void begin(); 
    void showImage(const byte *image, bool use_red_color);
};

#endif // epaper_h