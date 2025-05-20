#ifndef LCD_MODULE_H
#define LCD_MODULE_H

#include "Particle.h" // For String, millis etc. if used within LCD module

namespace LCD
{
    // LCD module reset funtion
    void setup_lcd_display();

    // LCD display update funtion
    void update_lcd_display(); // basic LCD update
}
#endif