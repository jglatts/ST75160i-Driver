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
  display.Clear();
  Serial.print("ZAXIS bytes: ");
  Serial.println(sizeof(ZAXIS_Logo_2));
  Serial.print("NHD bytes: ");
  Serial.println(sizeof(NHD_Logo));

  display.FillRaw(0x00);
  display.PutImage(ZAXIS_Testing_Logo, 12, 160);
}

void test_raw() {
  display.FillRaw(0x00);
  delay(50);

  display.SendCmd(0x30);
  display.SendCmd(0x75);
  display.SendDataByte(0x00);
  display.SendDataByte(0x18);
  display.SendCmd(0x5C);

  // move to page 5, x 10
  for (int i = 0; i < 5 * 160 + 10; i++) {
    display.SendDataByte(0x00);
  }

  // "A" pattern
  display.SendDataByte(0x20);
  display.SendDataByte(0x50);
  display.SendDataByte(0x88);
  display.SendDataByte(0xF8);
  display.SendDataByte(0x88);
  display.SendDataByte(0x88);
  display.SendDataByte(0x88);

  // fill rest of screen 
  for (int i = (5 * 160 + 10 + 7); i < 4000; i++) {
    display.SendDataByte(0x00);
  }
}

void test_new_str() {
    display.Clear();
    display.PutStrNew(10, 10, "HI", 1);
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

void loop() {
  /*
  display.FillRaw(0x00);
  display.PutImage(NHD_Logo, 12, 160);
  delay(2000);
  display.FillRaw(0x00);
  display.PutImage(ZAXIS_Logo_3, 12, 160);
  delay(2000);
  display.Clear();
  display.Flush();
  delay(100);
  */
}