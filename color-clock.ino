
#include <SPI.h>
#include <ThreeWire.h>  // for RTC DS1302
#include <RtcDS1302.h>  // for RTC DS1302

#define P_LE 2
#define P_D1 3
#define P_D0 4
#define P_EN 5

void led_setup() {
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

static uint8_t led_d7g_data[4][3]; // R G B

void led_refrash(uint8_t idx, bool _on) {
  digitalWrite(P_LE, HIGH);
  digitalWrite(P_EN, LOW);
  digitalWrite(P_D0, idx & 1 ? HIGH : LOW);
  digitalWrite(P_D1, idx & 2 ? HIGH : LOW);
  SPI.transfer(0x00);
  SPI.transfer(!_on ? 0 : led_d7g_data[idx][2]);
  SPI.transfer(!_on ? 0 : led_d7g_data[idx][1]);
  SPI.transfer(!_on ? 0 : led_d7g_data[idx][0]);
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
static uint8_t led_nums[4], led_dps[4], led_brightness_mask;
void led_put_num(uint8_t idx, uint8_t num, uint8_t rgb) {
  led_nums[idx] = (num & 0xf) | (rgb << 4);
}
void led_put_nums(uint8_t nums[4], uint8_t rgb) {
  for (int i = 0; i < 4; i ++)
    led_put_num(i, nums[i], rgb);
}
void led_put_dp(uint8_t idx, uint8_t rgb) {
  led_dps[idx] = rgb;
}
void led_put_dps(uint8_t dps, uint8_t rgb) {
  for (int i = 0; i < 4; i ++) {
    led_put_dp(i, (dps & (1 << i)) ? rgb : 0);
  }
}
// 1-8: 8 level brightness, 0: no display
void led_set_brightness(uint8_t brightness) {
    static const uint8_t mask_map[9] = { 0x0, 0x1, 0x11, 0x49, 0x55, 0x57, 0x77, 0x7f, 0xff };
    if (brightness > 8)
        brightness = 8;
    led_brightness_mask = mask_map[brightness];
}

void led_clear() {
  for (int i = 0; i < 4; i ++) {
    led_nums[i] = 0;
    led_dps[i] = 0;
  }
}

void led_display_task() {
    static unsigned long last_update = 0;
    static uint8_t update_cnt = 0;
    unsigned long now = millis();
    if (now - last_update < 1) // update period
        return;
    bool led_on = ((1 << ((update_cnt >> 2) & 7)) & led_brightness_mask) != 0;
    led_update_bits(led_nums, led_dps);
    led_refrash(update_cnt & 3, led_on);
    last_update = now;
    update_cnt ++;
}


// ******** RTC Module ********

// refer to https://electropeak.com/learn/interfacing-ds1302-real-time-clock-rtc-module-with-arduino/
// chinese version: https://blog.jmaker.com.tw/ds1302/
// official: https://github.com/Makuna/Rtc/wiki
// official example: https://github.com/Makuna/Rtc/blob/master/examples/DS1302_Simple/DS1302_Simple.ino

ThreeWire myWire(7, 8, 6); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void rtc_setup() {
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Serial.print("Compile time: ");
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid())
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled)
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled)
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled)
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}
void rtc_print_time(bool nl) {
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    if (nl) Serial.println();

    if (!now.IsValid()) {
        //the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }
}
void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];
    snprintf_P(datestring, 20,
               PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
               dt.Month(),
               dt.Day(),
               dt.Year(),
               dt.Hour(),
               dt.Minute(),
               dt.Second() );
    Serial.print(datestring);
}

// ******** sensor ********
int read_light_sensor() {
    return analogRead(A0);
}

void beep_out(int val) {  // pwm 0 - 255
    analogWrite(A2, val);
}

// ******** Application code ********

void time_display_task() {
    static uint8_t last_minu = 0xff;
    static uint8_t last_sec = 0xff;
    RtcDateTime now = Rtc.GetDateTime();
    uint8_t hour = now.Hour();
    uint8_t minu = now.Minute();
    uint8_t sec  = now.Second();
    led_set_brightness(3);
    if (minu != last_minu) {
        bool pm = hour > 12;
        hour   -= hour > 12 ? 12 : 0;
        uint8_t rgb = minu & 7;
        rgb += rgb == 0 ? 1 : 0;
        led_put_num(0, hour > 9 ? 1 : 0, hour > 9 ? rgb : 0);
        led_put_num(1, hour % 10, rgb);
        led_put_num(2, minu / 10, rgb);
        led_put_num(3, minu % 10, rgb);
        led_put_dp(0, rgb);    // 'PM' mark
        last_minu = minu;
    }
    if (sec != last_sec) {
        uint8_t rgb = sec & 7;
        rgb += rgb == 0 ? 1 : 0;
        led_put_dp(1, sec & 1 ? rgb : 0);
        last_sec = sec;
    }
}

void led_test_task() {
    int cnt = millis() / 1024;
    for (int i = 0; i < 4; i ++) {
        uint8_t rgb = (cnt + i) & 7;
        rgb += rgb == 0 ? 1 : 0;
        led_put_num(i, (cnt + i) % 16, rgb);
    }
    uint8_t rgb = cnt & 7;
    rgb += rgb == 0 ? 1 : 0;
    led_put_dps(1 << (cnt & 3), rgb);
}

void print_task() {
    static unsigned long last_print = 0;
    unsigned long now = millis();
    if (now - last_print < 3000)
        return;
    //Serial.print("beat message ");
    //Serial.println(now, DEC);
    int light = read_light_sensor();
    Serial.print("light=");
    Serial.print(light, DEC);
    Serial.print(", time: ");
    rtc_print_time(true);
    last_print = now;
}

void beep_task() {
    static unsigned long last_beep = 0;
    unsigned long now = millis();
    beep_out(120);
    if (now - last_beep < 1000)
        return;
    int sec = now / 1000;
    //beep_out((sec % 5) * 50);
    last_beep = now;
}

void setup() {
    led_setup();
    Serial.begin(9600);
    rtc_setup();

    Serial.println("setup done.");
}

void loop() {
    //led_test_task();
    time_display_task();
    led_display_task();
    print_task();
    //beep_task();
}

