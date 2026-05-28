/**
*
*   ST75160i Gray Scale Dot Matrix Driver for Arduino family
*
*   Author: John Glatts
*   Date:   May-12-2026
*
*/
#ifndef __ST75160I_H
#define __ST75160I_H

#include <Arduino.h>
#include <Wire.h>

#define ST75160I_ADDR 0x3F
#define ST75160I_CMD  0x00
#define ST75160I_DATA 0x40

#define LCD_W 160
#define LCD_H 128
#define LCD_ACTIVE_H 100
#define LCD_PAGES 13
#define LCD_PAGE_WIDTH 160
#define LCD_PAGE_BYTES (LCD_PAGE_WIDTH * 4)
#define LCD_BUF_SIZE 4000

#define SCR_ORIENT_NORMAL 0x01
#define SCR_ORIENT_180    0x02
#define SCR_ORIENT_CW     0x04
#define SCR_ORIENT_CCW    0x08

typedef struct {
	uint8_t font_Width;
	uint8_t font_Height;
	uint8_t font_BPC;
	uint8_t font_Scan;
	uint8_t font_MinChar;
	uint8_t font_MaxChar;
	uint8_t font_UnknownChar;
	uint8_t font_Data[475];
} Font_TypeDef;

#define FONT_V 0
#define FONT_H 1

class ST75160i {
public:
	ST75160i(uint8_t sda, uint8_t scl, uint8_t rst);

	void Init();
	void Clear();
	void Flush();

	bool IsConnected();

	void SayHiVinceAndJack();
	void SetPixel(uint8_t x, uint8_t y, uint8_t GS);
	uint8_t PutChar(uint8_t x, uint8_t y, uint8_t ch, const Font_TypeDef* Font);
	uint8_t PutStr(uint8_t x, uint8_t y, const char* str, const Font_TypeDef* font);

	void PutImage(const unsigned char* image, uint8_t pages, uint8_t imgCols);

	void TestChar(uint8_t x, uint8_t y);
	void FillScreen(uint8_t d);
	void FillRaw(uint8_t d);
	void OnePixelBorder();

	void SendCmd(uint8_t cmd);
	void SendDataByte(uint8_t data);

private:
	uint8_t _sda, _scl, _rst;
	uint8_t _orientation = SCR_ORIENT_CW;

	void WriteEnable();
	void SendDataBuffer(const uint8_t* data, uint16_t len);
};

#endif