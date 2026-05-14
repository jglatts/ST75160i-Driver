#include "st75160i.h"
#include "font5x7.h"
#include "logos.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define RST_PIN 23

#define BTN_DISPLAY_IMAGES  12
#define BTN_RUN_TEST_DEMO   13
#define BTN_HI_VINCE_JACK   14

ST75160i display(SDA_PIN, SCL_PIN, RST_PIN);
bool displayInit = false;


void setup() {
  Serial.begin(115200);
  pinMode(BTN_DISPLAY_IMAGES, INPUT_PULLUP);
  pinMode(BTN_RUN_TEST_DEMO, INPUT_PULLUP);
  pinMode(BTN_HI_VINCE_JACK, INPUT_PULLUP);
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

void draw_display() {
  display.FillRaw(0x00);
  display.PutImage(NHD_Logo, 12, 160);	
  delay(1000);
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
  delay(1000);
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
    test_display_loop();
  }

  if (!digitalRead(BTN_DISPLAY_IMAGES)) {
    draw_display();
  }

  if (!digitalRead(BTN_HI_VINCE_JACK)) {
    display.SayHiVinceAndJack();
  }

  delay(10);
}

void loop() {
  demo_with_buttons();
}