// ROMs can be found at https://github.com/kripod/chip8-roms
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Wire.h>
#include <SPIFFS.h>
#include <Chip8.h>

#define TFT_BL 21
#define TFT_CS 15
#define TFT_DC 2
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_RST -1

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

// Note: the ESP32 has 2 SPI ports, to have ESP32-2432S028R work with the TFT and Touch on different SPI ports each needs to be defined and passed to the library
SPIClass hspi = SPIClass(HSPI);
SPIClass vspi = SPIClass(VSPI);

XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

Adafruit_ILI9341 display = Adafruit_ILI9341(&hspi, TFT_DC, TFT_CS, TFT_RST);

#define TICKS_PER_FRAME 30
Chip8 chip8;

#define SCALE 3.5
const uint16_t SCREEN_LENGTH = chip8.SCREEN_WIDTH * chip8.SCREEN_HEIGHT;

#define OLED_RESET -1
#define BEEPER 13

#define ROWS 4
#define COLS 4

const char hexaKeys[ROWS][COLS] = {
    {'C', 'D', 'E', 'F'},
    {'3', '6', '9', 'B'},
    {'2', '5', '8', '0'},
    {'1', '4', '7', 'A'}};

// R1, R2, R3, R4
byte rowPins[ROWS] = {33, 32, 15, 4};
// C1, C2, C3, C4
byte colPins[COLS] = {25, 26, 27, 14};

class Button
{
public:
    int x1, y1, x2, y2;
    String label;

    Button(int x1, int x2, int y1, int y2, String label)
    {
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->label = label;
    }

    // Verifica se o toque está dentro dos limites do botão
    bool isPressed(int touchX, int touchY)
    {
        return (touchX >= x1 && touchX <= x2 && touchY >= y1 && touchY <= y2);
    }

    // Desenha o botão na tela
    void draw()
    {
        display.drawRect(x1, y1, x2 - x1, y2 - y1, ILI9341_WHITE);
        display.setTextSize(2);
        display.setCursor(x2 - 35, y2 - 35);
        display.print(label);
    }
};

class KeyPad
{
private:
    const int countCell = 4;
    String listNameButton[4][4] = {
        {"1", "2", "3", "C"},
        {"4", "5", "6", "D"},
        {"7", "8", "9", "E"},
        {"A", "0", "B", "F"}};

public:
    Button *listButton[4][4];
    KeyPad(int posX, int posY, int width, int height, float scale)
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

    void draw_buttons()
    {
        const int NUM_ROWS = 4; // Número de linhas
        const int NUM_COLS = 4; // Número de colunas

        for (int y = 0; y < NUM_ROWS; y++)
        {
            for (int x = 0; x < NUM_COLS; x++)
            {
                listButton[y][x]->draw();
            }
        }
    }

    // Verifica se algum botão foi pressionado
    String checkPress(int touchX, int touchY)
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
};

KeyPad *keypad = nullptr;

void load_file(File rom)
{
    // load rom to chip8 memory
    uint16_t bytes_read = 0;
    while (rom.available())
    {
        byte data = rom.read();
        chip8.load_byte(data, bytes_read++);
    }
    rom.close();

    if (bytes_read == 0)
    {
        Serial.println(F("ROM not loaded! Restart device manually."));
        tone(BEEPER, 300);
        while (true)
            ;
    }

    Serial.println(F("ROM loaded."));
    Serial.print(bytes_read);
    Serial.println(F(" bytes read."));
    display.fillScreen(ILI9341_BLACK);
}

void draw_screen()
{
    bool *screen_buf = chip8.get_display();
    for (unsigned int i = 0; i < SCREEN_LENGTH; i++)
    {
        uint16_t color = ILI9341_BLACK;

        unsigned int x = i % chip8.SCREEN_WIDTH;
        unsigned int y = i / chip8.SCREEN_WIDTH;

        if (screen_buf[i])
        {
            color = ILI9341_WHITE;
        }
        display.fillRect(x * SCALE, y * SCALE, SCALE, SCALE, color);
    }
}

// verify if the file ext is ".ch8"
bool is_rom(File file)
{
    String name = file.name();
    int8_t dot_index = name.lastIndexOf('.');
    if (dot_index != -1)
    {
        return (name[dot_index + 1] == 'c') && (name[dot_index + 2] == 'h') && (name[dot_index + 3] == '8');
    }
    else
        return false;
}

byte count_files(File dir)
{
    if (!dir.isDirectory())
        return 0;

    byte count = 0;
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            dir.rewindDirectory();
            return count;
        }

        if (!entry.isDirectory() && is_rom(entry))
        {
            count++;
        }
    }
}

void show_text(byte x, byte y, byte text_size, String text)
{
    display.setTextColor(ILI9341_WHITE);
    display.setCursor(x, y);
    display.setTextSize(text_size);
    display.print(text);
}

String verificaBotaoPressionado()
{
    if (ts.touched())
    {
        TS_Point p = ts.getPoint();

        int touchX = map(p.x, 200, 3700, 1, display.width());
        int touchY = map(p.y, 240, 3800, 1, display.height());

        // Verifica se algum botão foi pressionado
        String pressed = keypad->checkPress(touchX, touchY);

        if (pressed != "")
        {
            return pressed;

            delay(100);
        }
    }

    return "";
}

void game_select()
{
    File dir = SPIFFS.open("/");
    byte filecount = count_files(dir);
    byte game_index = 0;
    while (true)
    {
        File file = dir.openNextFile();
        game_index++;

        while (!is_rom(file))
            file = dir.openNextFile();

        display.fillScreen(ILI9341_BLACK);
        show_text(0, 0, 1, "Select Game:");
        show_text(98, 0, 1, String(game_index) + "/" + String(filecount));
        show_text(20, 48, 1, "Top(2) Select(5)");
        show_text(22, 56, 1, "Next(6) Skip(8)");

        // remove brackets
        String game_name = file.name();
        int8_t start = game_name.indexOf('[');
        int8_t end = game_name.lastIndexOf(']') - start + 2;
        if (start != -1 && end != -1)
            game_name.remove(start - 1, end);

        show_text(0, 16, 1, game_name);

        keypad->draw_buttons();

        while (true)
        {
            String key = verificaBotaoPressionado();

            if (key == "2")
            {
                dir.rewindDirectory();
                game_index = 0;
                break;
            }
            else if (key == "5")
            {
                load_file(file);
                return;
            }
            else if (key == "6")
            {
                if (game_index == filecount)
                {
                    dir.rewindDirectory();
                    game_index = 0;
                }
                break;
            }
            else if (key == "8")
            {
                for (byte i = 0; i < 9; i++)
                {
                    if (game_index != filecount)
                    {
                        file = dir.openNextFile();
                        game_index++;
                    }
                    else
                    {
                        dir.rewindDirectory();
                        game_index = 0;
                    }
                }
                break;
            }
        }
    }
}

byte get_key_code(String key)
{
    switch (key[0])
    {
    case '1':
        return 0x1;
    case '2':
        return 0x2;
    case '3':
        return 0x3;
    case '4':
        return 0x4;
    case '5':
        return 0x5;
    case '6':
        return 0x6;
    case '7':
        return 0x7;
    case '8':
        return 0x8;
    case '9':
        return 0x9;
    case '0':
        return 0x0;
    case 'A':
        return 0xA;
    case 'B':
        return 0xB;
    case 'C':
        return 0xC;
    case 'D':
        return 0xD;
    case 'E':
        return 0xE;
    case 'F':
        return 0xF;
    }
    return 0X0;
}

// void keypadEvent(KeypadEvent key)
// {
//     KeyState key_state = keypad.getState();

//     if (key)
//     {
//         byte key_code = get_key_code(key);

//         if (key_state == PRESSED)
//         {
//             chip8.keypress(key_code, true);
//         }
//         else if (key_state == RELEASED)
//         {
//             chip8.keypress(key_code, false);
//         }
//     }
// }

void setup()
{
    Serial.begin(115200);
    display.begin();
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    // keypad.addEventListener(keypadEvent);

    vspi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    ts.begin(vspi);
    ts.setRotation(0);
    delay(250); // wait for display to power up
    display.fillScreen(ILI9341_BLACK);

    if (!SPIFFS.begin())
    {
        show_text(10, 26, 1, "Erro ao montar SPIFFS");
        tone(BEEPER, 300);
        while (true)
            ;
    }

    keypad = new KeyPad(15, 115, 10, 10, 5); // Posicao (30,50), tamanho de 50x50, escala 1.5

    game_select();
}

void loop()
{
    // char key = keypad.getKey();

    String key = verificaBotaoPressionado();

    byte key_code = 0;

    if (key != "")
    {
        key_code = get_key_code(key);

        chip8.keypress(key_code, true);
    }
    else
    {
        chip8.keypressReset();
    }

    for (int i = 0; i < TICKS_PER_FRAME; i++)
    {
        chip8.tick();
    }

    chip8.tick_timers();

    if (chip8.beeping)
    {
        tone(BEEPER, 7000, 50);
    }

    draw_screen();
    keypad->draw_buttons();
}
