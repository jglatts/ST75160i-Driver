#include "st75160i.h"
#include "font5x7.h"
#include "logos.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define RST_PIN 23

ST75160i display(SDA_PIN, SCL_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  display.Init();
  delay(5);
  display.FillRaw(0x00);
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

void test_display_loop() {
  display.FillRaw(0x00);
  display.PutImage(ZAXIS_Testing_Logo, 12, 160);	
  delay(2000);
  display.FillRaw(0xFF);
  delay(100);
  display.FillRaw(0xAA);
  delay(100);
  display.FillRaw(0x55);
  display.PutImage(ZAXIS_Testing_Passed_Logo, 12, 160);	
  delay(2000);
}

void loop() {
  test_display_loop();
}