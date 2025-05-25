#include "Button.h"
#include <Adafruit_ILI9341.h>

class KeyPad
{
private:
    const int countCell = 4;
    String listNameButton[4][4] = {
        {"1", "2", "3", "C"},
        {"4", "5", "6", "D"},
        {"7", "8", "9", "E"},
        {"A", "0", "B", "F"}
    };

public:
    Button *listButton[4][4];
    KeyPad(int posX, int posY, int width, int height, float scale);

    void draw_buttons(Adafruit_ILI9341& display);

    // Verifica se algum botão foi pressionado
    String checkPress(int touchX, int touchY);
};
