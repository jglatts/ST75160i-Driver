#include "st75160i.h"
#include "font5x7.h"
#include "font7x10.h"
#include "logos.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define RST_PIN 23

#define BTN_DISPLAY_IMAGES  12
#define BTN_RUN_TEST_DEMO   13
#define BTN_HI_VINCE_JACK   14

// -----------------------------------------------------------------------------
// 16x16 Alien bitmap
// -----------------------------------------------------------------------------
const uint16_t alien16x16[16] = {
    0b0000011111100000,
    0b0001111111111000,
    0b0011111111111100,
    0b0111101101101110,
    0b1111111111111111,
    0b1110011111100111,
    0b1111111111111111,
    0b0011111111111100,
    0b0001111111111000,
    0b0011011111101100,
    0b0110011001100110,
    0b1100000000000011,
    0b0000011001100000,
    0b0000110000110000,
    0b0001100000011000,
    0b0011000000001100
};

// -----------------------------------------------------------------------------
// 16x16 Rocket bitmap
// -----------------------------------------------------------------------------
const uint16_t rocket16x16[16] = {
    0b0000000110000000,
    0b0000001111000000,
    0b0000011111100000,
    0b0000111111110000,
    0b0001111111111000,
    0b0001110010011000,
    0b0011110010011100,
    0b0011111111111100,
    0b0011111111111100,
    0b0001111111111000,
    0b0000111111110000,
    0b0000111111110000,
    0b0001110000111000,
    0b0011100000011100,
    0b0011000000001100,
    0b0000000000000000
};

ST75160i display(SDA_PIN, SCL_PIN, RST_PIN);
bool displayInit = false;

void setup() {
  Serial.begin(115200);
  pinMode(BTN_DISPLAY_IMAGES, INPUT_PULLUP);
  pinMode(BTN_RUN_TEST_DEMO, INPUT_PULLUP);
  pinMode(BTN_HI_VINCE_JACK, INPUT_PULLUP);

  display.Init();
  delay(5);
}

void drawBitmap16x16(uint8_t x, uint8_t y, const uint16_t bmp[16], uint8_t gs = 15) {
    for (uint8_t row = 0; row < 16; row++) {
        for (uint8_t col = 0; col < 16; col++) {
            if (bmp[row] & (0x8000 >> col)) {
                display.SetPixel(x + col, y + row, gs);
            }
        }
    }
}

void dancing_names() {
    const char* names[] = {
        "HEY VINCE",
        "HEY JACK",
        "HEY JOHN"
    };

    const uint8_t spacing = 8;
    const uint8_t nameCount = 3;
    const uint8_t baseYs[] = { 24, 45, 66 };

    for (int frame = 0; frame < 100; frame++) {
        display.Clear();

        for (uint8_t n = 0; n < nameCount; n++) {
            const char* msg = names[n];

            uint8_t len = 0;
            while (msg[len] != '\0') {
                len++;
            }

            uint8_t startX = (160 - (len * spacing)) / 2;
            uint8_t baseY = baseYs[n];

            for (uint8_t i = 0; msg[i] != '\0'; i++) {
                int wave = (frame + (i * 4) + (n * 6)) % 20;

                uint8_t yOffset;

                if (wave < 10) {
                    yOffset = wave;
                }
                else {
                    yOffset = 20 - wave;
                }

                display.PutChar(
                    startX + (i * spacing),
                    baseY + yOffset,
                    msg[i],
                    fnt7x10
                );
            }
        }

        // Rocket top left-to-right
        uint8_t rocketX = frame % 144;
        drawBitmap16x16(rocketX, 4, rocket16x16);

        // Invader bottom right-to-left
        uint8_t alienX = 144 - (frame % 144);
        drawBitmap16x16(alienX, 82, alien16x16);

        display.Flush();
        delay(25);
    }
}
  
void dancing_hello_john() {
    const char* msg = "HELLO JOHN";
    const uint8_t startX = 22;
    const uint8_t baseY = 45;
    const uint8_t spacing = 8;

    for (int frame = 0; frame < 80; frame++) {
        display.Clear();

        for (uint8_t i = 0; msg[i] != '\0'; i++) {
            int wave = (frame + i * 3) % 16;

            uint8_t yOffset;
            if (wave < 8) {
                yOffset = wave;
            } else {
                yOffset = 16 - wave;
            }

            display.PutChar(startX + i * spacing, baseY + yOffset, msg[i], fnt7x10);
        }

        display.Flush();
        delay(60);
    }
}

void test_vram_text() {
    display.Clear();

    display.PutStr(5, 5, "ST75160I", fnt5x7);
    display.PutStr(5, 20, "VRAM TEXT TEST", fnt5x7);
    display.PutStr(5, 35, "HELLO JOHN", fnt5x7);
    display.PutStr(5, 50, "VINCE + JACK", fnt5x7);

    display.Flush();
}

void test_seq_display() {
  display.Clear();
  display.FillRaw(0x00);
  delay(1000);
  display.FillRaw(0xFF);
  delay(1000);
  display.FillRaw(0xAA);
  delay(1000);
  display.FillRaw(0x55);  
  delay(1000);
}

void draw_display() {
  display.FillRaw(0x00);
  display.PutImage(ZAXIS_Logo_Good, 12, 160);	
}

void test_vram_raw_pattern() {
    display.FillRaw(0x00);
    display.Clear();
    delay(200);

    display.FillRaw(0xAA);
    display.Clear();
    delay(200);

    display.FillRaw(0x00);
    display.Clear();
    delay(200);


    // Directly fill VRAM with a known pattern
    display.FillScreen(0xAA);   
}

void test_vram_pixels() {
    display.Clear();

    // left to right booter
    // only drawing at btm
    for (int x = 0; x < 160; x++) {
        display.SetPixel(x, 0, 15);   
        display.SetPixel(x, 99, 15);
    }

    // top to bottom border 
    for (int y = 0; y < 100; y++) {
        display.SetPixel(0, y, 15);
        display.SetPixel(159, y, 15);
    }

    display.Flush();
}

// needs testing with new GS param
// but test it out
void test_str() {
    display.Clear();
    display.PutStr(10, 20, "HELLO JOHN", fnt5x7);
    display.Flush();
}

void test_display_loop() {
  display.PutImage(ZAXIS_Testing_Logo, 12, 160);	
  delay(1000);
  /*
  display.FillRaw(0x50);
  delay(50);
  display.FillRaw(0xAA);
  delay(50);
  display.FillRaw(0x00);
  */
  display.PutImage(ZAXIS_Testing_Passed_Logo, 12, 160);	
}

void flexibletest_demo() {
    display.Clear();

    // Header
    display.PutStr(18, 4, "FLEXIBLETEST", fnt7x10);
    display.PutStr(20, 20, "FFC INTERPOSER", fnt5x7);

    // Divider
    for (int x = 8; x < 152; x++) {
        display.SetPixel(x, 34, 15);
    }

    // Test info
    display.PutStr(12, 42, "DISPLAY FFC LINK", fnt5x7);
    display.PutStr(12, 54, "CONTACT CHECK", fnt5x7);

    display.Flush();
    delay(500);

    // Progress bar
    for (int w = 0; w <= 116; w += 4) {
        // Clear bar area
        for (int y = 72; y < 86; y++) {
            for (int x = 20; x < 140; x++) {
                display.SetPixel(x, y, 0);
            }
        }

        // Border
        for (int x = 20; x < 140; x++) {
            display.SetPixel(x, 72, 15);
            display.SetPixel(x, 85, 15);
        }

        for (int y = 72; y < 86; y++) {
            display.SetPixel(20, y, 15);
            display.SetPixel(139, y, 15);
        }

        // Fill
        for (int x = 22; x < 22 + w && x < 138; x++) {
            for (int y = 74; y < 84; y++) {
                display.SetPixel(x, y, 15);
            }
        }

        display.Flush();
        delay(5);
    }

    delay(200);

    display.Clear();

    // Final screen
    display.PutStr(18, 6, "FLEXIBLETEST", fnt7x10);

    for (int x = 18; x < 142; x++) {
        display.SetPixel(x, 28, 15);
        display.SetPixel(x, 76, 15);
    }

    for (int y = 28; y < 77; y++) {
        display.SetPixel(18, y, 15);
        display.SetPixel(141, y, 15);
    }

    display.PutStr(48, 40, "FFC PASS", fnt7x10);
    display.PutStr(24, 84, "INTERPOSER READY", fnt5x7);

    display.Flush();
    delay(1200);
}

void demo_with_buttons() {
  bool displayConnected = display.IsConnected();

  if (displayConnected && !displayInit) {
    display.Init();
    delay(5);
    display.FillRaw(0x00);
  }

  displayInit = displayConnected;

  if (!displayConnected) {
    delay(50);
    return;
  }

  if (!digitalRead(BTN_RUN_TEST_DEMO)) {
    //test_display_loop();
    flexibletest_demo();
  }

  if (!digitalRead(BTN_DISPLAY_IMAGES)) {
    //draw_display();
     flexibletest_demo();
  }

  if (!digitalRead(BTN_HI_VINCE_JACK)) {
    //display.SayHiVinceAndJack();
    dancing_names();
  }

  delay(10);
}

void loop() {
  demo_with_buttons();
}