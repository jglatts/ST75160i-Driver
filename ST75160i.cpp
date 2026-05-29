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

static const uint32_t GS_LUT[] = {
  0x00000000, 0x01000000, 0x00010000, 0x01010000,
  0x00000100, 0x01000100, 0x00010100, 0x01010100,
  0x00000001, 0x01000001, 0x00010001, 0x01010001,
  0x00000101, 0x01000101, 0x00010101, 0x01010101
};

static const uint32_t POC_LUT[] = {
  0xFEFEFEFE, 0xFDFDFDFD, 0xFBFBFBFB, 0xF7F7F7F7,
  0xEFEFEFEF, 0xDFDFDFDF, 0xBFBFBFBF, 0x7F7F7F7F
};

static uint8_t vRAM[LCD_BUF_SIZE] __attribute__((aligned(4)));

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

bool ST75160i::IsConnected() {
    Wire.beginTransmission(ST75160I_ADDR);
    return Wire.endTransmission() == 0;
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
    Wire.setClock(400000);

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

    //Flush();
}

void ST75160i::Clear() {
    memset(vRAM, 0x00, sizeof(vRAM));
}

void ST75160i::Flush() {
    SendCmd(0x30);  // 00110000

    // Column range: 0 to 159
    SendCmd(0x15);
    SendDataByte(0x00);
    SendDataByte(0x9F);

    // Row/page range: 0 to 24
    SendCmd(0x75);
    SendDataByte(0x00);
    SendDataByte(0x18);

    // RAM write
    SendCmd(0x5C);

    SendDataBuffer(vRAM, 4000);
}

void ST75160i::FillScreen(uint8_t d) {
    memset(vRAM, d, sizeof(vRAM));
    Flush();
}

void ST75160i::FillRaw(uint8_t d) {
    WriteEnable();
    for (int i = 0; i < 4000; i++) {
        SendDataByte(d);
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

void ST75160i::SetPixel(uint8_t X, uint8_t Y, uint8_t GS) {
    if (X >= LCD_W || Y >= LCD_ACTIVE_H) return;

    // vertical flip only
    Y = (LCD_ACTIVE_H - 1) - Y;

    uint8_t page = Y >> 3;
    uint8_t bit = Y & 0x07;

    uint16_t byteIndex = (page * LCD_W) + X;
    if (byteIndex >= LCD_BUF_SIZE) return;

    uint8_t mask = 1 << (7 - bit);

    if (GS > 0) {
        vRAM[byteIndex] |= mask;
    }
    else {
        vRAM[byteIndex] &= ~mask;
    }
}

uint8_t ST75160i::PutChar(uint8_t x, uint8_t y, uint8_t ch, const Font_TypeDef* Font) {
    uint8_t pX;
    uint8_t pY;
    uint8_t tmpCh;
    uint8_t bL;
    const uint8_t* pCh;

    int lcd_color = 0XFF;

    // If the specified character code is out of bounds should substitute the code of the "unknown" character
    if ((ch < Font->font_MinChar) || (ch > Font->font_MaxChar)) ch = Font->font_UnknownChar;

    // Pointer to the first byte of character in font data array
    // Uses ASCII math to the get the pointer to the font data array
    // see https://www.asciitable.com/ for table
    // i.e, for letter 'a' --> (97 - 32) * 5 = element 325
    pCh = &Font->font_Data[(ch - Font->font_MinChar) * Font->font_BPC];
    //Serial.print("pCh = 0x");
    //Serial.println(*pCh, HEX);

    // Draw character
    if (Font->font_Scan == FONT_V) {
        // Vertical pixels order
        if (Font->font_Height < 9) {
            // Height is 8 pixels or less (one byte per column)
            pX = x;
            while (pX < x + Font->font_Width) {
                pY = y;
                tmpCh = *pCh++;
                while (tmpCh) {
                    if (tmpCh & 0x01) SetPixel(pX, pY, lcd_color);
                    tmpCh >>= 1;
                    pY++;
                }
                pX++;
            }
        }
        else {
            // Height is more than 8 pixels (several bytes per column)
            pX = x;
            while (pX < y + Font->font_Width) {
                pY = y;
                while (pY < y + Font->font_Height) {
                    bL = 8;
                    tmpCh = *pCh++;
                    if (tmpCh) {
                        while (bL) {
                            if (tmpCh & 0x01) SetPixel(pX, pY, lcd_color);
                            tmpCh >>= 1;
                            if (tmpCh) {
                                pY++;
                                bL--;
                            }
                            else {
                                pY += bL;
                                break;
                            }
                        }
                    }
                    else {
                        pY += bL;
                    }
                }
                pX++;
            }
        }
    }
    else {
        // Horizontal pixels order
        if (Font->font_Width < 9) {
            // Width is 8 pixels or less (one byte per row)
            pY = y;
            while (pY < y + Font->font_Height) {
                pX = x;
                tmpCh = *pCh++;
                while (tmpCh) {
                    if (tmpCh & 0x01) SetPixel(pX, pY, lcd_color);
                    tmpCh >>= 1;
                    pX++;
                }
                pY++;
            }
        }
        else {
            // Width is more than 8 pixels (several bytes per row)
            pY = y;
            while (pY < y + Font->font_Height) {
                pX = x;
                while (pX < x + Font->font_Width) {
                    bL = 8;
                    tmpCh = *pCh++;
                    if (tmpCh) {
                        while (bL) {
                            if (tmpCh & 0x01) SetPixel(pX, pY, lcd_color);
                            tmpCh >>= 1;
                            if (tmpCh) {
                                pX++;
                                bL--;
                            }
                            else {
                                pX += bL;
                                break;
                            }
                        }
                    }
                    else {
                        pX += bL;
                    }
                }
                pY++;
            }
        }
    }

    return Font->font_Width + 1;
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

void ST75160i::drawBitmap16x16(uint8_t x, uint8_t y, const uint16_t* bmp, uint8_t gs = 15) {
    for (uint8_t row = 0; row < 16; row++) {
        for (uint8_t col = 0; col < 16; col++) {
            if (bmp[row] & (0x8000 >> col)) {
                SetPixel(x + col, y + row, gs);
            }
        }
    }
}

uint8_t ST75160i::PutStr(uint8_t x, uint8_t y, const char* str, const Font_TypeDef* font) {
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

    // top border - left to right
    for (int x = 0; x < 160; x++) {
        if (x == 0 || x == 159) {
            SendDataByte(0xFF);
        }
        else {
            SendDataByte(0x80);
        }
    }

    // left border - top to bottom
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

