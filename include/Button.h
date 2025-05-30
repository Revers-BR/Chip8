#include <Arduino.h>
#include <Adafruit_ILI9341.h>

class Button
{
public:
    int x1, y1, x2, y2;
    String label;

    Button(int x1, int x2, int y1, int y2, String label);

    // Verifica se o toque está dentro dos limites do botão
    bool isPressed(int touchX, int touchY);

    // Desenha o botão na tela
    void draw(Adafruit_ILI9341& display, bool clear = false);
};
