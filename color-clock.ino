
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

void led_output(uint8_t idx) {
  digitalWrite(P_LE, HIGH);
  digitalWrite(P_EN, LOW);
  digitalWrite(P_D0, idx & 1 ? HIGH : LOW);
  digitalWrite(P_D1, idx & 2 ? HIGH : LOW);
  SPI.transfer(0x00);
  SPI.transfer(led_d7g_data[idx][2]);
  SPI.transfer(led_d7g_data[idx][1]);
  SPI.transfer(led_d7g_data[idx][0]);
  digitalWrite(P_EN, HIGH);
  digitalWrite(P_LE, LOW);
}

#define RGB_R  0x4
#define RGB_G  0x2
#define RGB_B  0x1

// each num: bit 3:0 is number, bit 6:4 is color, r/g/b
static uint8_t num2led_map[] = {
 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,  // 0-5
 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c,  // 6-b
 0x39, 0x5e, 0x79, 0x71               // c-f
};
void led_update_bits(uint8_t nums[4], uint8_t dps[4]) {
  for (int i = 0; i < 4; i ++) {
    uint8_t dp = dps[i];
    uint8_t val = nums[i] & 0xf;
    uint8_t color = nums[i] >> 4;
    uint8_t led_bits = num2led_map[val & 0xf];
    led_d7g_data[i][0] = ((color & RGB_R) ? led_bits : 0) | (dp & RGB_R ? 0x80 : 0);
    led_d7g_data[i][1] = ((color & RGB_G) ? led_bits : 0) | (dp & RGB_G ? 0x80 : 0);
    led_d7g_data[i][2] = ((color & RGB_B) ? led_bits : 0) | (dp & RGB_B ? 0x80 : 0);
  }
}
static uint8_t led_nums[4], led_dps[4];
void led_update_num(uint8_t idx, uint8_t num, uint8_t rgb) {
  led_nums[idx] = (num & 0xf) | (rgb << 4);
}
void led_update_nums(uint8_t nums[4], uint8_t rgb) {
  for (int i = 0; i < 4; i ++)
    led_update_num(i, nums[i], rgb);
}
void led_update_dp(uint8_t idx, uint8_t rgb) {
  led_dps[idx] = rgb;
}
void led_update_dps(uint8_t dps, uint8_t rgb) {
  for (int i = 0; i < 4; i ++) {
    led_update_dp(i, (dps & (1 << i)) ? rgb : 0);
  }
}

void led_clear() {
  for (int i = 0; i < 4; i ++) {
    led_nums[i] = 0;
    led_dps[i] = 0;
  }
}


void led_display(int cnt) {
  for (int i = 0; i < 4; i ++) {
    uint8_t rgb = (cnt + i) & 7;
    rgb += rgb == 0 ? 1 : 0;
    led_update_num(i, (cnt + i) % 16, rgb);
  }
  uint8_t rgb = cnt & 7;
  rgb += rgb == 0 ? 1 : 0;
  led_update_dps(1 << (cnt & 3), rgb);
}

int loop_cnt = 0;
void loop() {
  delay(1);
  //uint8_t numbs[4] = { 2, 3, 4, 5 };
  //led_update_nums(numbs, RGB_R | RGB_B);
  led_display(loop_cnt / 1024);
  led_update_bits(led_nums, led_dps);
  led_output(loop_cnt & 3);
  loop_cnt ++;
}

