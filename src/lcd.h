#include <LiquidCrystal_I2C.h>

struct LCD : public LiquidCrystal_I2C {

    LCD(): 
        LiquidCrystal_I2C(0x27, 16, 2)
    {}

    void setup() {
        init();
        backlight();
        createChar(1, up);
        createChar(2, down);
        createChar(4, right);
        createChar(3, left);
        createChar(5, pause);
        createChar(6, bullet);
        createChar(7, agrave);
    }
};