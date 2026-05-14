/**
*
*   ST75160i Gray Scale Dot Matrix Driver for Arduino family
*
*   ToDo:
*       Get VRAM buffer support working
*       Right now, every pixel change is sent immediately to the display, which is very slow. 
        Instead, we should be able to write to a local buffer and then flush it to the display in one go.

*   Author: John Glatts
*   Date:   May-12-2026
*
*/
#include "st75160i.h"
#include "simple_font.h"
#include "logos.h"

ST75160i::ST75160i(uint8_t sda, uint8_t scl, uint8_t rst) {
    _sda = sda;
    _scl = scl;
    _rst = rst;
}

void ST75160i::SendCmd(uint8_t cmd) {
    Wire.beginTransmission(ST75160I_ADDR);
    Wire.write(ST75160I_CMD);
    Wire.write(cmd);
    Wire.endTransmission();
    delay(1);
}

void ST75160i::SendDataByte(uint8_t data) {
    Wire.beginTransmission(ST75160I_ADDR);
    Wire.write(ST75160I_DATA);
    Wire.write(data);
    Wire.endTransmission();
    delayMicroseconds(100);
}

void ST75160i::SendDataBuffer(const uint8_t* data, uint16_t len) {
    const uint8_t chunk = 28;

    while (len > 0) {
        uint8_t n = (len > chunk) ? chunk : len;

        Wire.beginTransmission(ST75160I_ADDR);
        Wire.write(ST75160I_DATA);

        for (uint8_t i = 0; i < n; i++) {
            Wire.write(*data++);
        }

        Wire.endTransmission();

        len -= n;
    }
}

void ST75160i::WriteEnable() {
    SendCmd(0x75);
    SendDataByte(0x00);
    SendDataByte(0x18);
    SendCmd(0x5C);
}

void ST75160i::Init() {
    Wire.begin(_sda, _scl);
    Wire.setClock(100000);

    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
    delay(200);
    digitalWrite(_rst, HIGH);
    delay(100);

    SendCmd(0x31);
    SendCmd(0xD7);
    SendDataByte(0x9F);

    SendCmd(0xE0);
    SendDataByte(0x00);
    delay(10);

    SendCmd(0xE3);
    delay(20);

    SendCmd(0xE1);

    SendCmd(0x30);
    SendCmd(0x94);
    SendCmd(0xAE);
    delay(50);

    SendCmd(0x20);
    SendDataByte(0x0B);
    delay(100);

    SendCmd(0x81);
    SendDataByte(0x08);
    SendDataByte(0x03);

    SendCmd(0x31);
    SendCmd(0x20);

    const uint8_t gray[] = {
      0x00, 0x00, 0x00,
      0x17, 0x17, 0x17,
      0x00, 0x00,
      0x1D,
      0x00, 0x00,
      0x1D, 0x1D, 0x1D,
      0x00, 0x00
    };

    SendDataBuffer(gray, sizeof(gray));

    SendCmd(0x32);
    SendDataByte(0x00);
    SendDataByte(0x01);
    SendDataByte(0x03);

    SendCmd(0x51);
    SendDataByte(0xFB);

    SendCmd(0x30);

    SendCmd(0xF0);
    SendDataByte(0x10);

    SendCmd(0xCA);
    SendDataByte(0x00);
    SendDataByte(0x63);
    SendDataByte(0x00);

    SendCmd(0xBC);
    SendDataByte(0x00);

    SendCmd(0xA6);

    SendCmd(0x31);
    SendCmd(0x40);

    SendCmd(0x30);
    SendCmd(0x77);

    SendCmd(0x15);
    SendDataByte(0x00);
    SendDataByte(0x9F);

    SendCmd(0x76);
    SendCmd(0x30);

    SendCmd(0x75);
    SendDataByte(0x00);
    SendDataByte(0x18);

    SendCmd(0xAF);

    delay(20);
    Clear();
    Flush();
}

void ST75160i::Clear() {
    memset(_buffer, 0x00, sizeof(_buffer));
}

void ST75160i::Flush() {
    WriteEnable();
    SendDataBuffer(_buffer, LCD_BUF_SIZE);
}

void ST75160i::FillScreen(uint8_t d) {
    memset(_buffer, d, sizeof(_buffer));
    Flush();
}

void ST75160i::FillRaw(uint8_t d) {
    WriteEnable();
    for (int i = 0; i < 4000; i++) {
        SendDataByte(d);
    }
}

void ST75160i::PutCharNew(uint8_t x, uint8_t y, char ch, uint8_t scale) {
    const SimpleFontChar* fc = nullptr;

    for (uint8_t i = 0; i < simpleFontCount; i++) {
        if (simpleFont[i].c == ch) {
            fc = &simpleFont[i];
            break;
        }
    }

    if (!fc) return;

    for (uint8_t row = 0; row < 7; row++) {
        uint8_t col = 0;
        while (fc->rows[row][col] != '\0') {
            if (fc->rows[row][col] == '1') {
                for (uint8_t sx = 0; sx < scale; sx++) {
                    for (uint8_t sy = 0; sy < scale; sy++) {
                        SetPixel(
                            x + (col * scale) + sx,
                            y + (row * scale) + sy,
                            true
                        );
                    }
                }
            }
            col++;
        }
    }
}

void ST75160i::PutStrNew(uint8_t x, uint8_t y, const char* str, uint8_t scale) {
    uint8_t cursorX = x;

    while (*str) {
        if (*str == '\n') {
            y += (8 * scale);
            cursorX = x;
            str++;
            continue;
        }
        PutCharNew(cursorX, y, *str, scale);
        cursorX += (6 * scale);
        str++;
    }
}

void ST75160i::SayHiVinceAndJack() {
    // 5x7-ish native packed columns, one blank byte between chars
    const uint8_t H[] = { 0xF8, 0x20, 0x20, 0x20, 0xF8 };
    const uint8_t I[] = { 0x88, 0x88, 0xF8, 0x88, 0x88 };
    const uint8_t V[] = { 0x78, 0x80, 0x80, 0x80, 0x78 };
    const uint8_t N[] = { 0xF8, 0x10, 0x20, 0x40, 0xF8 };
    const uint8_t C[] = { 0x70, 0x88, 0x80, 0x88, 0x50 };
    const uint8_t E[] = { 0xF8, 0xA8, 0xA8, 0xA8, 0x88 };
    const uint8_t A[] = { 0xF0, 0x28, 0x28, 0x28, 0xF0 };
    const uint8_t D[] = { 0xF8, 0x88, 0x88, 0x88, 0x70 };
    const uint8_t J[] = { 0x20, 0x40, 0x80, 0x80, 0x78 };
    const uint8_t K[] = { 0xF8, 0x20, 0x50, 0x88, 0x00 };
    const uint8_t SPACE[] = { 0x00, 0x00, 0x00 };

    auto writeChar = [&](const uint8_t* ch, uint8_t w) {
        for (uint8_t i = 0; i < w; i++) {
            SendDataByte(ch[i]);
            SendDataByte(ch[i]);   // double width
        }

        SendDataByte(0x00);
        SendDataByte(0x00);
        };

    FillRaw(0x00);
    delay(10);

    SendCmd(0x30);
    SendCmd(0x75);
    SendDataByte(0x00);
    SendDataByte(0x18);
    SendCmd(0x5C);

    // skip to page 5, x 10
    int start = (5 * 160) + 10;
    for (int i = 0; i < start; i++) {
        SendDataByte(0x00);
    }

    writeChar(A, 5);
    writeChar(N, 5);
    writeChar(D, 5);
    for (int i = 0; i < 6; i++) SendDataByte(0x00);
    writeChar(J, 5);
    writeChar(A, 5);
    writeChar(C, 5);
    writeChar(K, 5);
    // move to next page row
    int writtenLine1 = start + (2 * 6) + 3 + (5 * 6);
    int nextLine = (7 * 160) + 10;

    for (int i = writtenLine1; i < nextLine; i++) {
        SendDataByte(0x00);
    }

    writeChar(H, 5);
    writeChar(I, 5);
    for (int i = 0; i < 6; i++) SendDataByte(0x00);
    writeChar(V, 5);
    writeChar(I, 5);
    writeChar(N, 5);
    writeChar(C, 5);
    writeChar(E, 5);

    // finish screen
    int writtenLine2 = nextLine + (3 * 6) + 3 + (4 * 6);
    for (int i = writtenLine2; i < 4000; i++) {
        SendDataByte(0x00);
    }
}

void ST75160i::SetPixel(uint8_t x, uint8_t y, bool on) {
    if (x >= LCD_W || y >= LCD_H) return;

    uint16_t index = ((y / 4) * LCD_W) + x;

    uint8_t bit;
    switch (y & 0x03) {
    case 0: bit = 0x11; break;
    case 1: bit = 0x22; break;
    case 2: bit = 0x44; break;
    default: bit = 0x88; break;
    }

    if (on) {
        _buffer[index] |= bit;
    }
    else {
        _buffer[index] &= ~bit;
    }
}

uint8_t ST75160i::PutChar(uint8_t x, uint8_t y, uint8_t ch, const Font_TypeDef* font) {
    if (ch < font->font_MinChar || ch > font->font_MaxChar) {
        ch = font->font_UnknownChar;
    }

    const uint8_t* p = &font->font_Data[(ch - font->font_MinChar) * font->font_BPC];

    for (uint8_t col = 0; col < font->font_Width; col++) {
        uint8_t line = *p++;
        for (uint8_t row = 0; row < font->font_Height; row++) {
            if (line & 0x01) {
                SetPixel(x + col, y + row, true);
            }
            line >>= 1;
        }
    }

    return font->font_Width + 1;
}

void ST75160i::TestChar(uint8_t x, uint8_t y) {
    const char* rows[7] = {
        "01110",
        "10001",
        "10001",
        "11111",
        "10001",
        "10001",
        "10001"
    };

    for (uint8_t row = 0; row < 7; row++) {
        for (uint8_t col = 0; col < 5; col++) {
            if (rows[row][col] == '1') {
                SetPixel(x + col, y + row, true);
            }
        }
    }
}

uint16_t ST75160i::PutStr(uint8_t x, uint8_t y, const char* str, const Font_TypeDef* font) {
    uint8_t startX = x;

    while (*str) {
        x += PutChar(x, y, *str++, font);
    }

    return x - startX;
}

void ST75160i::PutImage(const unsigned char* image, uint8_t pages, uint8_t imgCols) {
    WriteEnable();
    const unsigned char* p = image;
    for (uint8_t page = 0; page < 12; page++) {
        // actual image data
        for (uint8_t col = 0; col < 160; col++) {
            SendDataByte(*p++);
        }
    }
}

void ST75160i::OnePixelBorder() {
    Clear();

    WriteEnable();
    for (int x = 0; x < 160; x++) {
        if (x == 0 || x == 159) {
            SendDataByte(0xFF);
        }
        else {
            SendDataByte(0x80);
        }
    }
    for (int y = 0; y < 11; y++) {
        for (int i = 0; i < 160; i++) {
            if (i == 0 || i == 159) {
                SendDataByte(0xFF);
            }
            else {
                SendDataByte(0x00);
            }
        }
    }
    for (int x = 0; x < 160; x++) {
        if (x == 0 || x == 159) {
            SendDataByte(0xFF);
        }
        else {
            SendDataByte(0x10);
        }
    }
}

