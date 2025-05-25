#include "KeyPad.h"

KeyPad::KeyPad(int posX, int posY, int width, int height, float scale)
{
    // Inicializa os botões
    for (int y = 0; y < countCell; y++)
    {
        for (int x = 0; x < countCell; x++)
        {
            int x1 = posX + (x * width) * scale;
            int x2 = posX + ((x * width) + width) * scale;
            int y1 = posY + (y * height) * scale;
            int y2 = posY + ((y * height) + height) * scale;

            Button *button = new Button(x1, x2, y1, y2, listNameButton[y][x]);
            listButton[y][x] = button;
        }
    }
}

void KeyPad::draw_buttons(Adafruit_ILI9341& display)
{
    const int NUM_ROWS = 4; // Número de linhas
    const int NUM_COLS = 4; // Número de colunas

    for (int y = 0; y < NUM_ROWS; y++)
    {
        for (int x = 0; x < NUM_COLS; x++)
        {
            listButton[y][x]->draw(display);
        }
    }
}

// Verifica se algum botão foi pressionado
String KeyPad::checkPress(int touchX, int touchY)
{
    for (int y = 0; y < countCell; y++)
    {
        for (int x = 0; x < countCell; x++)
        {
            if (listButton[y][x]->isPressed(touchX, touchY))
            {
                return listButton[y][x]->label;
            }
        }
    }
    return "";
}
