
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

uint8_t led_d7g_data[4][3]; // R G B

void led_output(uint8_t idx, uint8_t num) {
  //digitalWrite(P_LE, LOW);
  digitalWrite(P_LE, HIGH);
  digitalWrite(P_EN, LOW);
  digitalWrite(P_D0, idx & 1 ? HIGH : LOW);
  digitalWrite(P_D1, idx & 2 ? HIGH : LOW);
  //SPI.transfer(0x00);
  //SPI.transfer(0x00);
  //SPI.transfer(0x00);
  //SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(led_d7g_data[idx][2]);
  SPI.transfer(led_d7g_data[idx][1]);
  SPI.transfer(led_d7g_data[idx][0]);
  //digitalWrite(P_EN, LOW);
  digitalWrite(P_EN, HIGH);
  //digitalWrite(P_LE, HIGH);
  digitalWrite(P_LE, LOW);
}

// each num: bit 3:0 is number, bit 6:4 is color, r/g/b
static uint8_t num2led_map[] = {
 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,  // 0-5
 0x7d, 0x07, 0x7f, 0x4f, 0x77, 0x7c,  // 6-b
 0x39, 0x5e, 0x79, 0x71 };            // c-f
void led_update(uint8_t nums[4]) {
  for (int i = 0; i < 4; i ++) {
    uint8_t num = nums[i];
    uint8_t led_bits = num2led_map[num & 0xf];
    led_d7g_data[i][0] = (num & 0x40) ? led_bits : 0;
    led_d7g_data[i][1] = (num & 0x20) ? led_bits : 0;
    led_d7g_data[i][2] = (num & 0x10) ? led_bits : 0;
  }
}

int loop_cnt = 0;
void loop() {
  delay(1);
  uint8_t nums[4] = { 0x70, 0x61, 0x32, 0x53 };
  //uint8_t nums[4] = { 0xf0, 0xf1, 0xf2, 0xf3 };
  led_update(nums);
  led_output(loop_cnt & 3, 1);
  loop_cnt ++;
}

