#include "st75160i.h"
#include "font5x7.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define RST_PIN 23

ST75160i display(SDA_PIN, SCL_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  display.Init();
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

void loop() {
  test_seq_display();
}