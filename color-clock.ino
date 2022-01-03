
#include <SPI.h>

#define P_LE 2
#define P_D1 3
#define P_D0 4
#define P_EN 5

void setup() {
  pinMode(P_LE, OUTPUT);
  pinMode(P_D1, OUTPUT);
  pinMode(P_D0, OUTPUT);
  pinMode(P_EN, OUTPUT);
  digitalWrite(P_LE, LOW);
  digitalWrite(P_D0, LOW);
  digitalWrite(P_D1, LOW);
  digitalWrite(P_EN, LOW);
  SPI.begin();
}

void led_output(uint8_t idx, uint8_t num) {
  digitalWrite(P_LE, LOW);
  digitalWrite(P_EN, HIGH);
  digitalWrite(P_D0, idx & 1 ? HIGH : LOW);
  digitalWrite(P_D1, idx & 2 ? HIGH : LOW);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  //digitalWrite(P_EN, LOW);
  digitalWrite(P_LE, HIGH);
}

void loop() {
  delay(1000);
  led_output(0, 1);
}

