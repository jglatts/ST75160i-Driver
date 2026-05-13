#include "st75160i.h"

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
    Wire.beginTransmission(ST75160I_ADDR);
    Wire.write(ST75160I_DATA);

    for (uint16_t i = 0; i < len; i++) {
        Wire.write(*data++);
    }

    Wire.endTransmission();
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
    SendDataByte(0x10); // monochrome mode

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

    delay(200);
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
    //Flush();
}

void ST75160i::SetPixel(uint8_t x, uint8_t y, bool on) {
    if (x >= LCD_W || y >= LCD_H) return;

    uint16_t index = ((y / 4) * LCD_W) + x;
    uint8_t bit = 0x80 >> (y & 0x03); // y0=0x80, y1=0x40, y2=0x20, y3=0x10

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

uint16_t ST75160i::PutStr(uint8_t x, uint8_t y, const char* str, const Font_TypeDef* font) {
    uint8_t startX = x;

    while (*str) {
        x += PutChar(x, y, *str++, font);
    }

    return x - startX;
}

void ST75160i::OnePixelBorder() {
    Clear();

    for (uint8_t x = 0; x < LCD_W; x++) {
        SetPixel(x, 0, true);
        SetPixel(x, LCD_H - 1, true);
    }

    for (uint8_t y = 0; y < LCD_H; y++) {
        SetPixel(0, y, true);
        SetPixel(LCD_W - 1, y, true);
    }

    Flush();
}