#include "Button.h"
#include <Adafruit_ILI9341.h>

class KeyPad
{
private:
    const int countCell = 4;
    String listNameButton[4][5] = {
        {"1", "2", "3", "C", "_"},
        {"4", "5", "6", "D", "_"},
        {"7", "8", "9", "E", "P"},
        {"A", "0", "B", "F", "R"}
    };

public:
    Button *listButton[4][5];
    KeyPad(int posX, int posY, int width, int height, float scale);

    void draw_buttons(Adafruit_ILI9341& display);

    // Verifica se algum botão foi pressionado
    String checkPress(int touchX, int touchY);
};
