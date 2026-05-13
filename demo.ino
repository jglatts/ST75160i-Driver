#include "st75160i.h"
#include "font5x7.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define RST_PIN 23

ST75160i display(SDA_PIN, SCL_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("Init ST75160i...");
  display.Init();

  display.Clear();
  display.PutStr(10, 20, "HELLO WORLD", fnt5x7);
  display.Flush();

  Serial.println("Done");
}

void loop() {
  delay(1000);
}