#include "Button.h"
#include <Arduino.h>
#include <Adafruit_ILI9341.h>

Button::Button(int x1, int x2, int y1, int y2, String label)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
    this->label = label;
}

bool Button::isPressed(int touchX, int touchY)
{
    return (touchX >= this->x1 && touchX <= this->x2 && touchY >= this->y1 && touchY <= this->y2);
}

void Button::draw(Adafruit_ILI9341 &display, bool clear)
{
    if (clear)
    {
        display.fillRect(x1, y1, x2 - x1, y2 - y1, ILI9341_BLACK);
    }
    else
    {
        display.drawRect(x1, y1, x2 - x1, y2 - y1, ILI9341_WHITE);
        display.setTextSize(2);
        display.setCursor(x2 - 28, y2 - 32);
        display.print(this->label);
    }
}