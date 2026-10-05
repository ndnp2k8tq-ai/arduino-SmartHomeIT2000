/**
 * =====================================================================================
 *  NHÀ THÔNG MINH ARDUINO
 *  Nhóm 14: Nguyễn Đoàn Nhất Phong, Nguyễn Duy Quang
 *  HUST - Đại học Bách Khoa Hà Nội  |  Trường CNTT&TT (SoICT)
 *  Kit: Keyestudio Smart Home Kit for Arduino (KS0085) - Arduino UNO
 * =====================================================================================
 *
 *  SƠ ĐỒ CHÂN
 *   A0 gas MQ-2 | A1 ánh sáng | A2 độ ẩm đất | A3 hơi nước/mưa | D2 PIR
 *   D3 còi | D4 nút 1 | D8 nút 2 | D5 LED vàng (PWM) | D13 LED trắng
 *   D6/D7 quạt | D12 relay | SDA/SCL LCD 0x27
 *   SERVO: mặc định (SWAP_SERVOS=1) cửa chính = D10, cửa sổ = D9
 *          (nếu vẫn bị nhầm thì đổi SWAP_SERVOS thành 0)
 *          Servo cửa sổ bị lỗi, nên chỉ có cửa chính
 *  BẢNG LỆNH SERIAL (9600 baud)
 *   a/b LED trắng bật/tắt   p/q LED vàng bật/tắt   v<0-255># độ sáng LED vàng
 *   c/d relay bật/tắt       r/s quạt max/tắt       w<0-255># tốc độ quạt
 *   +/- tăng/giảm quạt      F đảo quạt             L đảo LED trắng     Y đảo LED vàng
 *   l/m mở/đóng cửa         D đảo cửa              t<0-180># góc cửa
 *   n/o đóng/mở cửa sổ      W đảo cửa sổ           u<0-180># góc cửa sổ
 *   e Happy Birthday        f Ode to Joy           K<1-11># chọn bài theo số
 *   N bài kế   B bài trước  g dừng nhạc            G bật/tắt phát liên tục
 *   < > chậm/nhanh nhạc     Q yên lặng bật/tắt     A<phút># hẹn giờ (A0# = huỷ)
 *   h ánh sáng  i gas  j đất  k hơi nước           y báo cáo  z JSON  T telemetry  E hộp đen
 *   S tắt còi 60s           R reset khoá/báo động  H đi vắng bật/tắt   P<mã># đổi mật mã
 *   V chào khách bật/tắt    C test servo           X về chế độ TỰ ĐỘNG
 *   M đổi chân servo cửa chính<->cửa sổ (lưu EEPROM)   U quạt theo hơi nước bật/tắt   Z xem ngưỡng hơi nước
 *   x DỪNG TẤT CẢ (reset)   ? trợ giúp
 */

// ============================== CẤU HÌNH (chỉnh ở đây) ===============================
#define FW_VERSION              "6.0"
#define SERIAL_BAUD             9600
#define LCD_I2C_ADDR            0x27

#define USE_WATCHDOG            0
#define ENABLE_AUTO_PUMP        1
#define TELEMETRY_DEFAULT_ON    0
#define TELEMETRY_PERIOD_MS     2000UL

#define WARMUP_MS               20000UL
#define RESET_GRACE_MS          15000UL
#define SENSOR_PERIOD_MS        40UL

// --- Khí gas (0..1023). HÃY HIỆU CHUẨN bằng lệnh 'i' trong không khí sạch ---
#define GAS_WARN_ON             450
#define GAS_WARN_OFF            400
#define GAS_DANGER_ON           700
#define GAS_DANGER_OFF          600
#define FAN_RUNON_MS            30000UL

// --- Mưa / hơi nước (HÃY HIỆU CHUẨN bằng lệnh 'k') ---
#define RAIN_ON                 800
#define RAIN_OFF                400
#define RAIN_CLEAR_DELAY_MS     8000UL
#define HUMID_ENABLE_DEFAULT    1        // 1 = quạt tự bật khi hơi nước nhiều
#define VAPOR_HIGH_ON           500      // >= 500 = hơi nước NHIỀU: đóng cửa sổ + quạt chạy (KỂ CẢ MƯA)
#define VAPOR_HIGH_OFF          450      // < 450 = coi như HẾT hơi nước: tắt quạt + mở lại cửa sổ
#define HUMID_FAN_LEVEL         150      // tốc độ quạt khi hơi nước nhiều (0..255)

// --- Ánh sáng & đèn ---
#define LIGHT_DARK_ON           300
#define LIGHT_DARK_OFF          380
#define LIGHT_LED_COMP          100      // cộng thêm vào ngưỡng "hết tối" khi đèn đang bật (tránh đèn tự chớp)
#define LIGHT_HOLD_MS           15000UL
#define NIGHT_LIGHT_LEVEL       6
#define DARK_ALWAYS_ON          1        // 1 = tối là bật cả 2 đèn | 0 = chỉ sáng khi có người
#define DARK_YELLOW_LEVEL       180      // độ sáng đèn vàng khi tối mà không có người (có người = 255)
#define GAS_LED_BLINK           0        // 0 = gas nguy hiểm: LED sáng liên tục | 1 = nhấp nháy cùng nhau

// --- Chào khách (có người đến gần) ---
#define WELCOME_DEFAULT_ON      1
#define WELCOME_LED_MS          15000UL  // v3.3: không còn bật đèn trắng (chỉ dùng tính thời gian chào khách)
#define WELCOME_COOLDOWN_MS     30000UL  // cần yên ắng bao lâu mới chào lần nữa

// --- Đất & bơm ---
#define SOIL_HIGH_MEANS_DRY     1
#define SOIL_DRY_ENTER          50
#define SOIL_DRY_EXIT           10
#define SOIL_REMIND_MS          60000UL
#define PUMP_RUN_MS             5000UL
#define PUMP_REST_MS            60000UL

// --- Servo ---
#define SWAP_SERVOS             1        // 1 = cửa chính D10, cửa sổ D9 | 0 = cửa chính D9, cửa sổ D10
#define DOOR_CLOSED_ANGLE       0
#define DOOR_OPEN_ANGLE         100
#define WINDOW_OPEN_ANGLE       5        // chừa lề để servo không đẩy vào điểm chặn (gây nóng)
#define WINDOW_CLOSED_ANGLE     175
#define DOOR_HOLD_MS            5000UL
#define DOOR_MANUAL_HOLD_MS     60000UL  // cửa mở từ menu / lệnh D giữ mở bao lâu
// Giam toc do servo de han che sut ap (nguyen nhan: man LCD toi khi cua so quay)
// Neu van sut ap: cap nguon ngoai 5V >= 2A cho VCC shield (KHONG qua Arduino)
#define SERVO_STEP_DEG          2       // reduced from 4 -> smoother, less current spike
#define SERVO_STEP_MS           20UL    // increased from 15 -> longer gap between steps
#define SERVO_DETACH_IDLE_MS    700UL    // ngắt xung servo sau khi đứng yên (0 = không ngắt)

// --- An ninh ---
#define PASSWORD_DEFAULT        ".--.-."
#define MAX_PW_LEN              12
#define DASH_MIN_MS             500UL
#define CLEAR_HOLD_MS           1500UL
#define DEBOUNCE_MS             30UL
#define INPUT_TIMEOUT_MS        10000UL
#define MAX_FAILS               3
#define LOCKOUT_BASE_MS         30000UL
#define LOCKOUT_SIREN_MS        5000UL
#define ARM_DELAY_MS            10000UL
#define INTRUDER_ALARM_MS       30000UL
#define SILENCE_MS              60000UL

// --- Nút tổ hợp & menu ---
#define COMBO_MIN_MS            800UL
#define RESET_HOLD_MS           3000UL
#define MUSIC_STOP_HOLD_MS      4000UL   // trong menu: giữ nút 1 từng này = tắt nhạc
#define MENU_TIMEOUT_MS         20000UL
#define MENU_ITEMS              13

// --- Khác ---
#define MANUAL_TIMEOUT_MS       0UL
#define FAN_RAMP_STEP           12
#define LCD_REFRESH_MS          200UL
#define LCD_PAGE_MS             3000UL
#define LCD_BACKLIGHT_TIMEOUT_MS 60000UL
#define NUM_SONGS               11
#define PAGE_COUNT              7
#define LOG_SIZE                10

// ================================== THƯ VIỆN =========================================
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#if USE_WATCHDOG
#include <avr/wdt.h>
#endif

#define TIME_REACHED(now, t) ((long)((now) - (t)) >= 0)
#define SNP(buf, fmt, ...) snprintf_P((buf), sizeof(buf), PSTR(fmt), ##__VA_ARGS__)
#define SHOWMSG(a, b, ms) showMsgP(PSTR(a), PSTR(b), (ms))
#define ONOFF(c) ((c) ? PSTR("ON") : PSTR("OFF"))

// ================================== CHÂN KẾT NỐI =====================================
const uint8_t PIN_GAS = A0;
const uint8_t PIN_LIGHT = A1;
const uint8_t PIN_SOIL = A2;
const uint8_t PIN_WATER = A3;
const uint8_t PIN_PIR = 2;
const uint8_t PIN_BUZZER = 3;
const uint8_t PIN_BTN1 = 4;
const uint8_t PIN_LED_Y = 5;
const uint8_t PIN_FAN_PWM = 6;
const uint8_t PIN_FAN_DIR = 7;
const uint8_t PIN_BTN2 = 8;
uint8_t pinServoDoor = SWAP_SERVOS ? 10 : 9;     // đổi được lúc chạy (lệnh M / menu SERVO SWAP)
uint8_t pinServoWin = SWAP_SERVOS ? 9 : 10;
bool servoSwap = SWAP_SERVOS;
const uint8_t PIN_RELAY = 12;
const uint8_t PIN_LED_W = 13;

// ================================ KIỂU DỮ LIỆU =======================================
enum GasState : uint8_t { GAS_WARMUP, GAS_OK, GAS_WARN, GAS_DANGER };
enum PumpState : uint8_t { PUMP_IDLE, PUMP_RUN, PUMP_REST };
enum EventCode : uint8_t {
  EV_NONE = 0, EV_GAS_WARN, EV_GAS_DANGER, EV_GAS_CLEAR, EV_RAIN, EV_RAIN_STOP,
  EV_SOIL_DRY, EV_SOIL_OK, EV_DOOR_OPEN, EV_PW_WRONG, EV_LOCKOUT, EV_INTRUDER,
  EV_ARMED, EV_DISARMED, EV_RESET, EV_TIMER_DONE, EV_PUMP, EV_VISITOR, EV_HUMID
};

struct Override {
  bool active;
  unsigned long since;
};

struct Button {
  uint8_t pin;
  bool rawLast;
  bool stable;
  unsigned long changedAt;
  unsigned long pressedAt;
};

struct LogEntry {
  uint32_t sec;
  uint8_t code;
};

// ============================== KHAI BÁO NGUYÊN MẪU ==================================
void setOv(Override &o);
bool isOv(Override &o);
bool anyOverride();
void clearOverrides();
void queueBeep(uint8_t n, uint16_t freq, uint16_t dur);
void showMsgP(const char *a, const char *b, unsigned long ms);
void openDoor(unsigned long holdMs);
void closeDoor();
void armAway();
void disarmAway();
void setRelay(bool on);
void startMelody(const uint16_t *notes, const uint8_t *durs, uint8_t len);
void startChime();
void stopMusic();
void playSong(uint8_t id);
void songName(uint8_t id, char *out);
void logEvent(uint8_t code);
void factoryReset();
void startServoTest();
void applyServoMap(bool swap, bool save);
void printStatus();
void printJson();
void printHelp();
void printBanner();
void printLog();

// ================================ ĐỐI TƯỢNG PHẦN CỨNG ================================
LiquidCrystal_I2C mylcd(LCD_I2C_ADDR, 16, 2);
Servo servoDoor;
Servo servoWin;

// ================================ BIẾN TRẠNG THÁI ====================================
// --- Cảm biến (đã lọc) ---
int gasVal = 0, lightVal = 0, soilVal = 0, waterVal = 0;
int pirVal = 0;
long gasAcc = 0, lightAcc = 0, soilAcc = 0, waterAcc = 0;
bool accInit = false;
unsigned long sensorNextAt = 0;

// --- Khí gas ---
GasState gasState = GAS_WARMUP;
unsigned long warmupEndAt = 0;
unsigned long dangerPrintAt = 0, warnBeepAt = 0;
bool silenceOn = false;
unsigned long silenceUntil = 0;
bool fanRunOn = false;
unsigned long fanRunOnUntil = 0;

// --- Mưa / ẩm / đất ---
bool raining = false, rainClearing = false;
unsigned long rainClearSince = 0;
bool humidEnabled = HUMID_ENABLE_DEFAULT;
bool vaporHigh = false;   // hơi nước nhiều (>=VAPOR_HIGH_ON, kể cả mưa): đóng cửa sổ; quạt chạy nếu humidEnabled
bool soilDry = false;
unsigned long soilRemindAt = 0;

// --- Đèn ---
bool isDark = false, motionSeen = false;
unsigned long lastMotionAt = 0, fadeNextAt = 0;
// PIR latch: giữ tín hiệu HIGH thêm 1.5s sau khi PIR trả về LOW
// Giải quyết: PIR cho xung ngắn -> Arduino bỏ sót
#define PIR_LATCH_MS  1500UL
bool pirLatched = false;
unsigned long pirLatchUntil = 0;
uint8_t ledYCur = 0;
bool ledWManual = false;
uint8_t ledYManual = 0;

// --- Chào khách ---
bool welcomeOn = WELCOME_DEFAULT_ON, welcomeActive = false;
unsigned long welcomeUntil = 0;

// --- Quạt ---
uint8_t fanCur = 0, fanTarget = 0, fanManual = 0;
char fanWhy = '-';      // lý do quạt chạy: D gas nguy hiểm | M thủ công | W gas cảnh báo | H hơi ẩm | R chạy thêm
unsigned long fanNextAt = 0;

// --- Servo ---
int doorCur = DOOR_CLOSED_ANGLE, doorTarget = DOOR_CLOSED_ANGLE;
int winCur = WINDOW_OPEN_ANGLE, winTarget = WINDOW_OPEN_ANGLE;
int winManualAngle = WINDOW_OPEN_ANGLE;
bool doorTimerOn = false;
unsigned long doorCloseAt = 0, servoNextAt = 0;
bool doorAttached = true, winAttached = true;
unsigned long doorMoveAt = 0, winMoveAt = 0;
uint8_t testStage = 0;
unsigned long testAt = 0;

// --- Relay / bơm ---
bool relayOn = false;
PumpState pumpState = PUMP_IDLE;
unsigned long pumpStart = 0, pumpRestUntil = 0;
bool pumpWasOv = false;

// --- Ghi đè thủ công ---
Override ovLedW = {false, 0}, ovLedY = {false, 0}, ovFan = {false, 0}, ovWin = {false, 0}, ovRelay = {false, 0};

// --- Mật mã / nút bấm ---
char password[MAX_PW_LEN + 1];
char inputBuf[MAX_PW_LEN + 1];
uint8_t inputLen = 0;
unsigned long lastInputAt = 0;
Button btn1 = {PIN_BTN1, false, false, 0, 0};
Button btn2 = {PIN_BTN2, false, false, 0, 0};
bool b1LongDone = false, b2LongDone = false, b1StopDone = false;
bool comboActive = false, comboBeeped = false;
unsigned long comboStart = 0;
uint8_t failCount = 0, lockoutLevel = 0;
bool lockoutActive = false;
unsigned long lockoutUntil = 0, lockoutSirenUntil = 0;

// --- Menu ---
bool menuMode = false;
uint8_t menuIdx = 0;
unsigned long menuLastAt = 0;

// --- Đi vắng / báo trộm ---
bool awayArmed = false, armPending = false, intruderAlarm = false;
unsigned long armAt = 0, intruderUntil = 0;

// --- Còi ---
uint16_t curTone = 0;
uint8_t beepQueue = 0;
uint16_t beepFreq = 0, beepDur = 0;
bool beepActive = false;
unsigned long beepEndAt = 0, beepNextAt = 0;
bool quietMode = false;

// --- Nhạc ---
bool melodyPlaying = false;
const uint16_t *melNotes = 0;
const uint8_t *melDurs = 0;
uint8_t melLen = 0, melIdx = 0;
unsigned long melNoteStart = 0;
uint8_t currentSong = 0;
uint8_t tempoPct = 100;
bool playlistOn = false, songPending = false, wasPlaying = false;
unsigned long songGapUntil = 0;

// --- Hẹn giờ ---
bool timerActive = false;
unsigned long timerEndAt = 0;

// --- LCD ---
char lcdCache[2][17];
char msgA[17], msgB[17];
bool msgActive = false;
unsigned long msgUntil = 0, lcdNextAt = 0, pageNextAt = 0, lastActivity = 0;
uint8_t lcdPage = 0, logoFrame = 255;
bool blOn = true;

// --- Thống kê / hộp đen / telemetry ---
uint16_t gasAlarmCount = 0, doorOpenCount = 0, intruderCount = 0, pumpRuns = 0, failTotal = 0, visitorCount = 0;
LogEntry logBuf[LOG_SIZE];
uint8_t logHead = 0, logCount = 0;
bool telemetryOn = TELEMETRY_DEFAULT_ON;
unsigned long telemAt = 0;

// --- Serial parser ---
char pendingCmd = 0;
char argBuf[MAX_PW_LEN + 2];
uint8_t argLen = 0;
unsigned long argStart = 0;

// ============================ KÝ TỰ TUỲ CHỈNH (LOGO HUST) ============================
uint8_t gearA_L[8] = {3, 11, 7, 30, 30, 7, 11, 3};
uint8_t gearA_R[8] = {24, 26, 28, 15, 15, 28, 26, 24};
uint8_t gearB_L[8] = {27, 27, 7, 14, 14, 7, 27, 27};
uint8_t gearB_R[8] = {27, 27, 28, 14, 14, 28, 27, 27};
uint8_t noteGlyph[8] = {2, 3, 2, 14, 30, 12, 0, 0};   // ký tự nốt nhạc (mã 3)

void setGear(uint8_t frame) {
  if (frame == 0) {
    mylcd.createChar(1, gearA_L);
    mylcd.createChar(2, gearA_R);
  } else {
    mylcd.createChar(1, gearB_L);
    mylcd.createChar(2, gearB_R);
  }
}

// ================================= NHẠC (PROGMEM) ====================================
// Đơn vị thời lượng: 25 ms. Có thể chỉnh nhanh/chậm bằng lệnh < >
#define N_R   0
#define N_G3  196
#define N_GS3 208
#define N_A3  220
#define N_AS3 233
#define N_B3  247
#define N_C4  262
#define N_CS4 277
#define N_D4  294
#define N_DS4 311
#define N_E4  330
#define N_F4  349
#define N_FS4 370
#define N_G4  392
#define N_GS4 415
#define N_A4  440
#define N_B4  494
#define N_C5  523
#define N_D5  587
#define N_DS5 622
#define N_E5  659
#define N_F5  698
#define N_G5  784
#define N_C6  1047

#define NREST 0
#define NL5 392
#define NM1 523
#define NM2 586
#define NM3 658
#define NM4 697
#define NM5 783

// 1. Happy Birthday
const uint16_t BIRTHDAY_NOTES[] PROGMEM = {
  294, 440, 392, 532, 494, 392, 440, 392, 587, 532, 392,
  784, 659, 532, 494, 440, 698, 659, 532, 587, 532
};
const uint8_t BIRTHDAY_DURS[] PROGMEM = {
  10, 10, 10, 10, 20, 10, 10, 10, 10, 20, 10,
  10, 10, 10, 10, 10, 15, 10, 10, 10, 20
};

// 2. Ode to Joy
const uint16_t ODE_NOTES[] PROGMEM = {
  NM3, NM3, NM4, NM5, NM5, NM4, NM3, NM2, NM1, NM1, NM2, NM3, NM3, NM2, NM2,
  NM3, NM3, NM4, NM5, NM5, NM4, NM3, NM2, NM1, NM1, NM2, NM3, NM2, NM1, NM1,
  NM2, NM2, NM3, NM1, NM2, NM3, NM4, NM3, NM1, NM2, NM3, NM4, NM3, NM2,
  NM1, NM2, NL5, NREST, NM3, NM3, NM4, NM5, NM5, NM4, NM3, NM4, NM2,
  NM1, NM1, NM2, NM3, NM2, NM1, NM1
};
const uint8_t ODE_DURS[] PROGMEM = {
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 18, 6, 24,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 18, 6, 24,
  12, 12, 12, 12, 12, 6, 6, 12, 12, 12, 6, 6, 12, 12,
  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 6, 6,
  12, 12, 12, 12, 18, 6, 24
};

// 3. Twinkle Twinkle Little Star
const uint16_t TWINKLE_NOTES[] PROGMEM = {
  N_C4, N_C4, N_G4, N_G4, N_A4, N_A4, N_G4,
  N_F4, N_F4, N_E4, N_E4, N_D4, N_D4, N_C4,
  N_G4, N_G4, N_F4, N_F4, N_E4, N_E4, N_D4,
  N_G4, N_G4, N_F4, N_F4, N_E4, N_E4, N_D4,
  N_C4, N_C4, N_G4, N_G4, N_A4, N_A4, N_G4,
  N_F4, N_F4, N_E4, N_E4, N_D4, N_D4, N_C4
};
const uint8_t TWINKLE_DURS[] PROGMEM = {
  12, 12, 12, 12, 12, 12, 24,  12, 12, 12, 12, 12, 12, 24,
  12, 12, 12, 12, 12, 12, 24,  12, 12, 12, 12, 12, 12, 24,
  12, 12, 12, 12, 12, 12, 24,  12, 12, 12, 12, 12, 12, 24
};

// 4. Jingle Bells
const uint16_t JINGLE_NOTES[] PROGMEM = {
  N_E4, N_E4, N_E4, N_E4, N_E4, N_E4, N_E4, N_G4, N_C4, N_D4, N_E4,
  N_F4, N_F4, N_F4, N_F4, N_F4, N_E4, N_E4, N_E4, N_E4, N_E4, N_D4, N_D4, N_E4, N_D4, N_G4,
  N_E4, N_E4, N_E4, N_E4, N_E4, N_E4, N_E4, N_G4, N_C4, N_D4, N_E4,
  N_F4, N_F4, N_F4, N_F4, N_F4, N_E4, N_E4, N_E4, N_E4, N_G4, N_G4, N_F4, N_D4, N_C4
};
const uint8_t JINGLE_DURS[] PROGMEM = {
  12, 12, 24, 12, 12, 24, 12, 12, 18, 6, 48,
  12, 12, 18, 6, 12, 12, 12, 6, 6, 12, 12, 12, 12, 24, 24,
  12, 12, 24, 12, 12, 24, 12, 12, 18, 6, 48,
  12, 12, 18, 6, 12, 12, 12, 6, 6, 12, 12, 12, 12, 48
};

// 5. Fur Elise
const uint16_t ELISE_NOTES[] PROGMEM = {
  N_E5, N_DS5, N_E5, N_DS5, N_E5, N_B4, N_D5, N_C5, N_A4, N_R,
  N_C4, N_E4, N_A4, N_B4, N_R,
  N_E4, N_GS4, N_B4, N_C5, N_R,
  N_E4, N_E5, N_DS5, N_E5, N_DS5, N_E5, N_B4, N_D5, N_C5, N_A4, N_R,
  N_C4, N_E4, N_A4, N_B4, N_R,
  N_E4, N_C5, N_B4, N_A4
};
const uint8_t ELISE_DURS[] PROGMEM = {
  6, 6, 6, 6, 6, 6, 6, 6, 18, 6,
  6, 6, 6, 18, 6,
  6, 6, 6, 18, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 18, 6,
  6, 6, 6, 18, 6,
  6, 6, 6, 30
};

// 6. Mary Had a Little Lamb
const uint16_t MARY_NOTES[] PROGMEM = {
  N_E4, N_D4, N_C4, N_D4, N_E4, N_E4, N_E4,
  N_D4, N_D4, N_D4,
  N_E4, N_G4, N_G4,
  N_E4, N_D4, N_C4, N_D4, N_E4, N_E4, N_E4, N_E4,
  N_D4, N_D4, N_E4, N_D4, N_C4
};
const uint8_t MARY_DURS[] PROGMEM = {
  12, 12, 12, 12, 12, 12, 24,
  12, 12, 24,
  12, 12, 24,
  12, 12, 12, 12, 12, 12, 12, 12,
  12, 12, 12, 12, 48
};

// 7. Frere Jacques
const uint16_t FRERE_NOTES[] PROGMEM = {
  N_C4, N_D4, N_E4, N_C4,  N_C4, N_D4, N_E4, N_C4,
  N_E4, N_F4, N_G4,  N_E4, N_F4, N_G4,
  N_G4, N_A4, N_G4, N_F4, N_E4, N_C4,  N_G4, N_A4, N_G4, N_F4, N_E4, N_C4,
  N_C4, N_G3, N_C4,  N_C4, N_G3, N_C4
};
const uint8_t FRERE_DURS[] PROGMEM = {
  12, 12, 12, 12,  12, 12, 12, 12,
  12, 12, 24,  12, 12, 24,
  6, 6, 6, 6, 12, 12,  6, 6, 6, 6, 12, 12,
  12, 12, 24,  12, 12, 24
};

// 8. Silent Night
const uint16_t SILENT_NOTES[] PROGMEM = {
  N_G4, N_A4, N_G4, N_E4,  N_G4, N_A4, N_G4, N_E4,
  N_D5, N_D5, N_B4,  N_C5, N_C5, N_G4,
  N_A4, N_A4, N_C5, N_B4, N_A4,  N_G4, N_A4, N_G4, N_E4,
  N_A4, N_A4, N_C5, N_B4, N_A4,  N_G4, N_A4, N_G4, N_E4,
  N_D5, N_D5, N_F5, N_D5, N_B4,  N_C5, N_G4, N_E4, N_G4, N_F4, N_D4, N_C4
};
const uint8_t SILENT_DURS[] PROGMEM = {
  22, 8, 15, 45,  22, 8, 15, 45,
  30, 15, 45,  30, 15, 45,
  30, 15, 22, 8, 15,  22, 8, 15, 45,
  30, 15, 22, 8, 15,  22, 8, 15, 45,
  30, 15, 22, 8, 15,  45, 22, 8, 15, 22, 8, 60
};

// 9. We Wish You a Merry Christmas
const uint16_t XMAS_NOTES[] PROGMEM = {
  N_D4,  N_G4, N_G4, N_A4, N_G4, N_FS4,  N_E4, N_E4, N_E4,
  N_A4, N_A4, N_B4, N_A4, N_G4,  N_FS4, N_D4, N_D4,
  N_B4, N_B4, N_C5, N_B4, N_A4,  N_G4, N_E4, N_D4, N_D4,
  N_E4, N_A4, N_FS4,  N_G4
};
const uint8_t XMAS_DURS[] PROGMEM = {
  12,  12, 6, 6, 6, 6,  12, 12, 12,
  12, 6, 6, 6, 6,  12, 12, 12,
  12, 6, 6, 6, 6,  12, 12, 6, 6,
  12, 12, 12,  36
};

// Song K10 (Beethoven 5) removed to save Flash.

// 11. HUST Fanfare
const uint16_t FANFARE_NOTES[] PROGMEM = {
  N_C5, N_E5, N_G5, N_C6, N_R, N_G5, N_C6
};
const uint8_t FANFARE_DURS[] PROGMEM = {
  8, 8, 8, 20, 6, 10, 40
};

// 12. Chuông cửa (ding - dong) - cũng dùng làm chuông chào khách
const uint16_t BELL_NOTES[] PROGMEM = { N_E5, N_C5 };
const uint8_t BELL_DURS[] PROGMEM = { 24, 48 };

// Song K13 (Bach Khoa) removed to save Flash.

// Kiểm tra lúc biên dịch: số nốt phải bằng số thời lượng
#define CHECK_SONG(n) static_assert(sizeof(n##_NOTES) / sizeof(uint16_t) == sizeof(n##_DURS), #n " sai do dai")
CHECK_SONG(BIRTHDAY);
CHECK_SONG(ODE);
CHECK_SONG(TWINKLE);
CHECK_SONG(JINGLE);
CHECK_SONG(ELISE);
CHECK_SONG(MARY);
CHECK_SONG(FRERE);
CHECK_SONG(SILENT);
CHECK_SONG(XMAS);
CHECK_SONG(FANFARE);
CHECK_SONG(BELL);

// =====================================================================================
//                                   TIỆN ÍCH CHUNG
// =====================================================================================
void setOv(Override &o) {
  o.active = true;
  o.since = millis();
}

bool isOv(Override &o) {
  if (!o.active) return false;
  if (MANUAL_TIMEOUT_MS > 0 && (millis() - o.since) >= MANUAL_TIMEOUT_MS) o.active = false;
  return o.active;
}

bool anyOverride() {
  bool r = false;
  if (isOv(ovLedW)) r = true;
  if (isOv(ovLedY)) r = true;
  if (isOv(ovFan)) r = true;
  if (isOv(ovWin)) r = true;
  if (isOv(ovRelay)) r = true;
  return r;
}

void clearOverrides() {
  ovLedW.active = false;
  ovLedY.active = false;
  ovFan.active = false;
  ovWin.active = false;
  ovRelay.active = false;
}

bool sirenSilenced() {
  if (!silenceOn) return false;
  if (TIME_REACHED(millis(), silenceUntil)) {
    silenceOn = false;
    return false;
  }
  return true;
}

const __FlashStringHelper *gasStateName() {
  switch (gasState) {
    case GAS_WARMUP: return F("WARMUP");
    case GAS_OK:     return F("OK");
    case GAS_WARN:   return F("WARNING");
    default:         return F("DANGER");
  }
}

// =====================================================================================
//                                    HỘP ĐEN (LOG)
// =====================================================================================
void logEvent(uint8_t code) {
  logBuf[logHead].sec = millis() / 1000UL;
  logBuf[logHead].code = code;
  logHead = (logHead + 1) % LOG_SIZE;
  if (logCount < LOG_SIZE) logCount++;
}

const __FlashStringHelper *eventName(uint8_t c) {
  switch (c) {
    case EV_GAS_WARN:   return F("Gas warning");
    case EV_GAS_DANGER: return F("GAS DANGER");
    case EV_GAS_CLEAR:  return F("Gas clear");
    case EV_RAIN:       return F("Rain start");
    case EV_RAIN_STOP:  return F("Rain stop");
    case EV_SOIL_DRY:   return F("Soil dry");
    case EV_SOIL_OK:    return F("Soil ok");
    case EV_DOOR_OPEN:  return F("Door open");
    case EV_PW_WRONG:   return F("Wrong password");
    case EV_LOCKOUT:    return F("LOCKOUT");
    case EV_INTRUDER:   return F("INTRUDER");
    case EV_ARMED:      return F("Away armed");
    case EV_DISARMED:   return F("Away disarmed");
    case EV_RESET:      return F("System reset");
    case EV_TIMER_DONE: return F("Timer done");
    case EV_PUMP:       return F("Pump run");
    case EV_VISITOR:    return F("Visitor near");
    case EV_HUMID:      return F("High humidity");
    default:            return F("?");
  }
}

void printLog() {
  Serial.println(F("--- HOP DEN (moi nhat truoc) ---"));
  if (logCount == 0) Serial.println(F("(trong)"));
  for (uint8_t i = 0; i < logCount; i++) {
    uint8_t idx = (logHead + LOG_SIZE - 1 - i) % LOG_SIZE;
    Serial.print(F("["));
    Serial.print(logBuf[idx].sec);
    Serial.print(F("s] "));
    Serial.println(eventName(logBuf[idx].code));
  }
}

// =====================================================================================
//                                    CẢM BIẾN
// =====================================================================================
void readSensors() {
  unsigned long now = millis();
  if (!TIME_REACHED(now, sensorNextAt)) return;
  sensorNextAt = now + SENSOR_PERIOD_MS;

  int g = analogRead(PIN_GAS);
  int l = analogRead(PIN_LIGHT);
  int s = analogRead(PIN_SOIL);
  int w = analogRead(PIN_WATER);

  if (!accInit) {
    gasAcc = (long)g << 4;
    lightAcc = (long)l << 4;
    soilAcc = (long)s << 4;
    waterAcc = (long)w << 4;
    accInit = true;
  } else {  // trung bình trượt hàm mũ (alpha = 1/4)
    gasAcc += (((long)g << 4) - gasAcc) / 4;
    lightAcc += (((long)l << 4) - lightAcc) / 4;
    soilAcc += (((long)s << 4) - soilAcc) / 4;
    waterAcc += (((long)w << 4) - waterAcc) / 4;
  }
  gasVal = (int)(gasAcc >> 4);
  lightVal = (int)(lightAcc >> 4);
  soilVal = (int)(soilAcc >> 4);
  waterVal = (int)(waterAcc >> 4);
  pirVal = digitalRead(PIN_PIR);
  // PIR latch: giữ HIGH thêm PIR_LATCH_MS sau khi PIR tắt
  unsigned long _now = millis();
  if (pirVal) { pirLatched = true; pirLatchUntil = _now + PIR_LATCH_MS; }
  else if (TIME_REACHED(_now, pirLatchUntil)) { pirLatched = false; }
  // Dùng pirLatched thay pirVal trong updateWelcome và updateSecurity để bắt xung ngắn
}

// =====================================================================================
//                                   KHÍ GAS (3 MỨC)
// =====================================================================================
void updateGas() {
  unsigned long now = millis();
  if (gasState == GAS_WARMUP) {
    if (!TIME_REACHED(now, warmupEndAt)) return;
    gasState = GAS_OK;
    Serial.println(F("gas_ready"));
  }
  GasState prev = gasState;
  switch (gasState) {
    case GAS_OK:
      if (gasVal >= GAS_DANGER_ON) gasState = GAS_DANGER;
      else if (gasVal >= GAS_WARN_ON) gasState = GAS_WARN;
      break;
    case GAS_WARN:
      if (gasVal >= GAS_DANGER_ON) gasState = GAS_DANGER;
      else if (gasVal < GAS_WARN_OFF) gasState = GAS_OK;
      break;
    case GAS_DANGER:
      if (gasVal < GAS_DANGER_OFF) gasState = (gasVal >= GAS_WARN_OFF) ? GAS_WARN : GAS_OK;
      break;
    default:
      break;
  }
  if (gasState != prev) {
    lastActivity = now;
    if (gasState == GAS_DANGER) {
      gasAlarmCount++;
      silenceOn = false;
      dangerPrintAt = now;
      logEvent(EV_GAS_DANGER);
      Serial.println(F("danger"));
    } else if (gasState == GAS_WARN) {
      warnBeepAt = now;
      logEvent(EV_GAS_WARN);
      Serial.println(F("gas_warning"));
    } else if (gasState == GAS_OK) {
      fanRunOn = true;
      fanRunOnUntil = now + FAN_RUNON_MS;
      logEvent(EV_GAS_CLEAR);
      Serial.println(F("gas_clear"));
    }
  }
  if (gasState == GAS_WARN && TIME_REACHED(now, warnBeepAt)) {
    queueBeep(1, 600, 150);
    warnBeepAt = now + 10000UL;
  }
}

// =====================================================================================
//                               MƯA / HƠI ẨM / ĐẤT
// =====================================================================================
void updateRain() {
  unsigned long now = millis();
  if (!raining) {
    if (waterVal >= RAIN_ON) {
      raining = true;
      rainClearing = false;
      lastActivity = now;
      logEvent(EV_RAIN);
      Serial.println(F("rain"));
    }
  } else if (waterVal <= RAIN_OFF) {
    if (!rainClearing) {
      rainClearing = true;
      rainClearSince = now;
    } else if (now - rainClearSince >= RAIN_CLEAR_DELAY_MS) {
      raining = false;
      rainClearing = false;
      logEvent(EV_RAIN_STOP);
      Serial.println(F("rain_stop"));
    }
  } else {
    rainClearing = false;
  }

  // ---- Hơi nước nhiều (>= VAPOR_HIGH_ON, KỂ CẢ MƯA) -> ĐÓNG CỬA SỔ + quạt tự chạy ----
  // v3.3: dùng ngưỡng cố định (bỏ mức nền tự hiệu chuẩn); quạt chạy ĐẾN KHI HẾT hơi nước,
  // KHÔNG còn giới hạn 5 phút như v3.2. Có trễ hysteresis (500 bật / 450 tắt) chống nhấp nháy.
  if (gasState == GAS_WARMUP) {          // đang làm nóng: chưa đánh giá
    vaporHigh = false;
    return;
  }
  if (!vaporHigh && waterVal >= VAPOR_HIGH_ON) {
    vaporHigh = true;
    lastActivity = now;
    logEvent(EV_HUMID);
    SHOWMSG("High humidity!", "Win close + fan", 3000);
    queueBeep(1, 700, 120);
    Serial.println(F("vapor_high"));
  } else if (vaporHigh && waterVal < VAPOR_HIGH_OFF) {
    vaporHigh = false;
    Serial.println(F("vapor_normal"));
  }
}

bool soilIsDry(int v, bool wasDry) {
#if SOIL_HIGH_MEANS_DRY
  return wasDry ? (v > SOIL_DRY_EXIT) : (v > SOIL_DRY_ENTER);
#else
  return wasDry ? (v < SOIL_DRY_EXIT) : (v < SOIL_DRY_ENTER);
#endif
}

void updateSoil() {
  unsigned long now = millis();
  bool dry = soilIsDry(soilVal, soilDry);
  if (dry != soilDry) {
    soilDry = dry;
    soilRemindAt = now + SOIL_REMIND_MS;
    if (dry) {
      logEvent(EV_SOIL_DRY);
      Serial.println(F("hydropenia"));
      queueBeep(2, 500, 100);
    } else {
      logEvent(EV_SOIL_OK);
      Serial.println(F("soil_ok"));
    }
  }
  if (soilDry && TIME_REACHED(now, soilRemindAt)) {
    queueBeep(2, 500, 100);
    soilRemindAt = now + SOIL_REMIND_MS;
  }
}

// =====================================================================================
//                                  RELAY / TƯỚI CÂY
// =====================================================================================
void setRelay(bool on) {
  relayOn = on;
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
}

void updatePump() {
  unsigned long now = millis();
  if (gasState == GAS_DANGER) {  // an toàn: ngắt relay khi có gas
    if (relayOn) setRelay(false);
    pumpState = PUMP_IDLE;
    return;
  }
  bool ov = isOv(ovRelay);
  if (pumpWasOv && !ov) {
    setRelay(false);
    pumpState = PUMP_IDLE;
  }
  pumpWasOv = ov;
  if (ov) return;
#if ENABLE_AUTO_PUMP
  switch (pumpState) {
    case PUMP_IDLE:
      if (soilDry) {
        setRelay(true);
        pumpState = PUMP_RUN;
        pumpStart = now;
        pumpRuns++;
        logEvent(EV_PUMP);
        Serial.println(F("pump_on"));
      }
      break;
    case PUMP_RUN:
      if (!soilDry || (now - pumpStart) >= PUMP_RUN_MS) {
        setRelay(false);
        pumpState = PUMP_REST;
        pumpRestUntil = now + PUMP_REST_MS;
        Serial.println(F("pump_off"));
      }
      break;
    case PUMP_REST:
      if (TIME_REACHED(now, pumpRestUntil)) pumpState = PUMP_IDLE;
      break;
  }
#endif
}

// =====================================================================================
//                                      QUẠT
//   Ưu tiên: GAS nguy hiểm > thủ công > gas cảnh báo > hơi ẩm cao > chạy thêm sau gas
// =====================================================================================
void updateFan() {
  unsigned long now = millis();
  uint8_t cmd = 0;
  fanWhy = '-';
  if (gasState == GAS_DANGER) {
    cmd = 255;
    fanWhy = 'D';
  } else if (isOv(ovFan)) {
    cmd = fanManual;
    fanWhy = 'M';
  } else if (gasState == GAS_WARN) {
    cmd = 170;
    fanWhy = 'W';
  } else if (vaporHigh && humidEnabled) {
    cmd = HUMID_FAN_LEVEL;
    fanWhy = 'H';
  } else if (fanRunOn) {
    if (TIME_REACHED(now, fanRunOnUntil)) {
      fanRunOn = false;
      cmd = 0;
    } else {
      cmd = 200;
      fanWhy = 'R';
    }
  }
  fanTarget = cmd;
  if (TIME_REACHED(now, fanNextAt)) {
    fanNextAt = now + 20;
    if (fanCur < fanTarget) {
      fanCur = (fanTarget - fanCur <= FAN_RAMP_STEP) ? fanTarget : fanCur + FAN_RAMP_STEP;
    } else if (fanCur > fanTarget) {
      fanCur = (fanCur - fanTarget <= FAN_RAMP_STEP) ? fanTarget : fanCur - FAN_RAMP_STEP;
    }
    if (fanCur == 0) {
      // BRAKE: INA=HIGH + INB=HIGH -> L9110 dừng ngay lập tức
      digitalWrite(PIN_FAN_DIR, HIGH);
      digitalWrite(PIN_FAN_PWM, HIGH);
    } else {
      // FWD: INA=LOW + INB=PWM -> quay thuận với tốc độ fanCur
      digitalWrite(PIN_FAN_DIR, LOW);
      analogWrite(PIN_FAN_PWM, fanCur);
    }
  }
}

// =====================================================================================
//                         CHÀO KHÁCH (CÓ NGƯỜI ĐẾN GẦN - PIR)
// =====================================================================================
void updateWelcome() {
  unsigned long now = millis();
  if (welcomeOn && pirLatched && gasState != GAS_WARMUP && gasState != GAS_DANGER &&
      !awayArmed && !intruderAlarm && !lockoutActive) {
    // chỉ chào khi trước đó đã yên ắng đủ lâu (tránh chào liên tục)
    if (!motionSeen || (now - lastMotionAt) >= WELCOME_COOLDOWN_MS) {
      welcomeActive = true;
      welcomeUntil = now + WELCOME_LED_MS;
      visitorCount++;
      lastActivity = now;
      logEvent(EV_VISITOR);
      startChime();
      SHOWMSG("Welcome!", "Someone is near", 3000);
      Serial.println(F("visitor"));
    }
  }
  if (welcomeActive && TIME_REACHED(now, welcomeUntil)) welcomeActive = false;
}

// =====================================================================================
//                                    CHIẾU SÁNG
// =====================================================================================
void updateLighting() {
  unsigned long now = millis();
  if (pirVal) {
    lastMotionAt = now;
    motionSeen = true;
    lastActivity = now;
  }
  // Khi đèn đang bật vì trời tối, ánh sáng đèn rọi vào cảm biến -> nâng ngưỡng "hết tối"
  int offThr = LIGHT_DARK_OFF + (isDark ? LIGHT_LED_COMP : 0);
  if (!isDark && lightVal < LIGHT_DARK_ON) isDark = true;
  else if (isDark && lightVal > offThr) isDark = false;
  bool recentMotion = motionSeen && ((now - lastMotionAt) < LIGHT_HOLD_MS);

  if (gasState == GAS_DANGER) {            // GAS NGUY HIỂM: sáng TẤT CẢ LED
    bool on = true;
#if GAS_LED_BLINK
    on = ((now / 300) % 2) == 0;
#endif
    digitalWrite(PIN_LED_W, on ? HIGH : LOW);
    ledYCur = on ? 255 : 0;
    analogWrite(PIN_LED_Y, ledYCur);
    return;
  }
  if (intruderAlarm) {                     // ĐỘT NHẬP: 2 đèn nháy luân phiên
    bool ph = ((now / 250) % 2) == 0;
    digitalWrite(PIN_LED_W, ph ? HIGH : LOW);
    ledYCur = ph ? 0 : 255;
    analogWrite(PIN_LED_Y, ledYCur);
    return;
  }

  // --- Đèn trắng ---
  // FIX v3.3: BỎ recentMotion & welcomeActive -> chuyển động (PIR) KHÔNG còn làm đèn trắng sáng.
  // Đèn trắng chỉ tự bật khi: trời tối (DARK_ALWAYS_ON=1), gas mức WARNING, hoặc bật tay ('a'/menu).
  bool autoW = (isDark && DARK_ALWAYS_ON) || (gasState == GAS_WARN);
  bool wantW = isOv(ovLedW) ? ledWManual : autoW;
  digitalWrite(PIN_LED_W, wantW ? HIGH : LOW);

  // --- Đèn vàng ---
  uint8_t target;
  if (isOv(ovLedY)) target = ledYManual;
  else if (!isDark) target = 0;                                   // đủ sáng: để yên
  else if (DARK_ALWAYS_ON) target = recentMotion ? 255 : DARK_YELLOW_LEVEL;
  else if (recentMotion) target = (uint8_t)map(constrain(lightVal, 0, LIGHT_DARK_ON), 0, LIGHT_DARK_ON, 220, 80);
  else target = NIGHT_LIGHT_LEVEL;

  if (TIME_REACHED(now, fadeNextAt)) {  // làm mờ/sáng dần
    fadeNextAt = now + 6;
    if (ledYCur < target) ledYCur = (target - ledYCur <= 4) ? target : ledYCur + 4;
    else if (ledYCur > target) ledYCur = (ledYCur - target <= 4) ? target : ledYCur - 4;
  }
  analogWrite(PIN_LED_Y, ledYCur);
}

// =====================================================================================
//                                CỬA SỔ & CỬA RA VÀO
// =====================================================================================
int stepToward(int cur, int target) {
  if (abs(target - cur) <= SERVO_STEP_DEG) return target;
  return (cur < target) ? cur + SERVO_STEP_DEG : cur - SERVO_STEP_DEG;
}

void updateWindow() {
  int want;
  if (gasState == GAS_DANGER) want = WINDOW_OPEN_ANGLE;               // thoát khí là ưu tiên số 1
  else if (isOv(ovWin)) want = winManualAngle;
  else want = ((raining || vaporHigh) && gasState != GAS_WARN) ? WINDOW_CLOSED_ANGLE : WINDOW_OPEN_ANGLE;
  winTarget = want;
}

void openDoor(unsigned long holdMs) {
  doorTarget = DOOR_OPEN_ANGLE;
  doorTimerOn = true;
  doorCloseAt = millis() + holdMs;
  doorOpenCount++;
  logEvent(EV_DOOR_OPEN);
}

void closeDoor() {
  doorTarget = DOOR_CLOSED_ANGLE;
  doorTimerOn = false;
}

void updateDoor() {
  if (doorTimerOn && TIME_REACHED(millis(), doorCloseAt)) closeDoor();
}

// Servo chỉ được cấp xung khi đang chạy; đứng yên > SERVO_DETACH_IDLE_MS thì ngắt (hết nóng).
void updateServos() {
  unsigned long now = millis();
  if (!TIME_REACHED(now, servoNextAt)) return;
  servoNextAt = now + SERVO_STEP_MS;

  if (doorCur != doorTarget) {
    if (!doorAttached) { servoDoor.attach(pinServoDoor); doorAttached = true; }
    doorCur = stepToward(doorCur, doorTarget);
    servoDoor.write(doorCur);
    doorMoveAt = now;
  } else if (SERVO_DETACH_IDLE_MS > 0 && doorAttached && (now - doorMoveAt) >= SERVO_DETACH_IDLE_MS) {
    servoDoor.detach();
    doorAttached = false;
  }

  if (winCur != winTarget) {
    if (!winAttached) { servoWin.attach(pinServoWin); winAttached = true; }
    winCur = stepToward(winCur, winTarget);
    servoWin.write(winCur);
    winMoveAt = now;
  } else if (SERVO_DETACH_IDLE_MS > 0 && winAttached && (now - winMoveAt) >= SERVO_DETACH_IDLE_MS) {
    servoWin.detach();
    winAttached = false;
  }
}

// Đổi chân servo cửa chính <-> cửa sổ (lưu EEPROM địa chỉ 20-21)
void applyServoMap(bool swap, bool save) {
  servoSwap = swap;
  pinServoDoor = swap ? 10 : 9;
  pinServoWin = swap ? 9 : 10;
  if (save) {
    EEPROM.update(20, 0x5A);
    EEPROM.update(21, swap ? 1 : 0);
  }
  servoDoor.detach();
  servoWin.detach();
  servoDoor.attach(pinServoDoor);
  servoWin.attach(pinServoWin);
  servoDoor.write(doorCur);
  servoWin.write(winCur);
  doorAttached = true;
  winAttached = true;
  doorMoveAt = millis();
  winMoveAt = millis();
}

void loadServoMap() {
  if (EEPROM.read(20) == 0x5A) servoSwap = (EEPROM.read(21) == 1);
  pinServoDoor = servoSwap ? 10 : 9;
  pinServoWin = servoSwap ? 9 : 10;
}

// Kiểm tra dây servo: cửa chính mở rồi đóng, sau đó cửa sổ đổi trạng thái rồi trả lại.
void startServoTest() {
  unsigned long now = millis();
  doorTarget = DOOR_OPEN_ANGLE;
  doorTimerOn = true;
  doorCloseAt = now + 2500UL;
  testStage = 1;
  testAt = now + 4000UL;
  SHOWMSG("TEST: MAIN DOOR", "Door must move", 3500);
  Serial.print(F("test_door pin D")); Serial.print(pinServoDoor);
  Serial.print(F(" | window pin D")); Serial.println(pinServoWin);
}

void updateTest() {
  if (testStage == 0) return;
  unsigned long now = millis();
  if (!TIME_REACHED(now, testAt)) return;
  if (testStage == 1) {
    winManualAngle = (winTarget == WINDOW_OPEN_ANGLE) ? WINDOW_CLOSED_ANGLE : WINDOW_OPEN_ANGLE;
    setOv(ovWin);
    testStage = 2;
    testAt = now + 4000UL;
    SHOWMSG("TEST: WINDOW", "Window must move", 3500);
    Serial.println(F("test_window"));
  } else {
    ovWin.active = false;
    testStage = 0;
    SHOWMSG("TEST DONE", "", 2000);
    Serial.println(F("test_done"));
  }
}

// =====================================================================================
//                                 ĐI VẮNG / BÁO TRỘM
// =====================================================================================
void armAway() {
  awayArmed = true;
  armPending = true;
  armAt = millis() + ARM_DELAY_MS;
  welcomeActive = false;
  SHOWMSG("Away mode ON", "Leave now! 10s", 4000);
  queueBeep(3, 1200, 100);
  logEvent(EV_ARMED);
  Serial.println(F("away_armed"));
}

void disarmAway() {
  awayArmed = false;
  armPending = false;
  intruderAlarm = false;
  logEvent(EV_DISARMED);
  Serial.println(F("away_disarmed"));
}

void updateSecurity() {
  unsigned long now = millis();
  if (awayArmed && armPending && TIME_REACHED(now, armAt)) {
    armPending = false;
    Serial.println(F("away_active"));
  }
  if (awayArmed && !armPending && !intruderAlarm && pirLatched && gasState != GAS_WARMUP) {
    intruderAlarm = true;
    intruderUntil = now + INTRUDER_ALARM_MS;
    intruderCount++;
    silenceOn = false;
    lastActivity = now;
    logEvent(EV_INTRUDER);
    Serial.println(F("intruder"));
  }
  if (intruderAlarm && TIME_REACHED(now, intruderUntil)) intruderAlarm = false;
  if (lockoutActive && TIME_REACHED(now, lockoutUntil)) {
    lockoutActive = false;
    Serial.println(F("lockout_end"));
  }
}

// =====================================================================================
//                                   ĐIỀU KHIỂN THIẾT BỊ
//            (dùng chung cho Menu nút bấm và lệnh Serial)
// =====================================================================================
void actFanCycle() {
  uint8_t base = isOv(ovFan) ? fanManual : fanCur;
  fanManual = (base < 85) ? 85 : (base < 170) ? 170 : (base < 255) ? 255 : 0;
  setOv(ovFan);
}

void actFanAdjust(int delta) {
  int base = isOv(ovFan) ? fanManual : fanCur;
  fanManual = (uint8_t)constrain(base + delta, 0, 255);
  setOv(ovFan);
}

void actLedWToggle() {
  bool cur = (digitalRead(PIN_LED_W) == HIGH);
  ledWManual = !cur;
  setOv(ovLedW);
}

void actLedYCycle() {
  uint8_t base = isOv(ovLedY) ? ledYManual : ledYCur;
  ledYManual = (base < 64) ? 64 : (base < 128) ? 128 : (base < 255) ? 255 : 0;
  setOv(ovLedY);
}

void actRelayToggle() {
  setRelay(!relayOn);
  setOv(ovRelay);
}

void actWindowToggle() {
  winManualAngle = (winTarget == WINDOW_OPEN_ANGLE) ? WINDOW_CLOSED_ANGLE : WINDOW_OPEN_ANGLE;
  setOv(ovWin);
}

void actDoorToggle() {
  if (doorTarget != DOOR_CLOSED_ANGLE) closeDoor();
  else openDoor(DOOR_MANUAL_HOLD_MS);
}

void actTimerAdd(int minutes) {
  unsigned long now = millis();
  unsigned long add = (unsigned long)minutes * 60000UL;
  if (!timerActive) {
    timerActive = true;
    timerEndAt = now + add;
  } else {
    timerEndAt += add;
    if ((timerEndAt - now) > 99UL * 60000UL) timerEndAt = now + 99UL * 60000UL;
  }
}

void updateTimer() {
  unsigned long now = millis();
  if (timerActive && TIME_REACHED(now, timerEndAt)) {
    timerActive = false;
    logEvent(EV_TIMER_DONE);
    SHOWMSG("TIME'S UP!", "Timer finished", 6000);
    playSong(10);                 // HUST Fanfare
    queueBeep(3, 1500, 150);
    lastActivity = now;
    Serial.println(F("timer_done"));
  }
}

// =====================================================================================
//                            MENU ĐIỀU KHIỂN BẰNG NÚT BẤM
// =====================================================================================
void menuName(uint8_t i, char *out) {
  switch (i) {
    case 0: strcpy_P(out, PSTR("FAN")); break;
    case 1: strcpy_P(out, PSTR("LED WHITE")); break;
    case 2: strcpy_P(out, PSTR("LED YELLOW")); break;
    case 3: strcpy_P(out, PSTR("RELAY")); break;
    case 4: strcpy_P(out, PSTR("WINDOW")); break;
    case 5: strcpy_P(out, PSTR("DOOR")); break;
    case 6: strcpy_P(out, PSTR("MUSIC")); break;
    case 7: strcpy_P(out, PSTR("TIMER")); break;
    case 8: strcpy_P(out, PSTR("QUIET")); break;
    case 9: strcpy_P(out, PSTR("WELCOME")); break;
    case 10: strcpy_P(out, PSTR("SERVO SWAP")); break;
    case 11: strcpy_P(out, PSTR("AUTO MODE")); break;
    default: strcpy_P(out, PSTR("STOP ALL")); break;
  }
}

void enterMenu() {
  menuMode = true;
  menuIdx = 0;
  menuLastAt = millis();
  inputLen = 0;
  inputBuf[0] = 0;
  queueBeep(2, 1400, 60);
  Serial.println(F("menu_on"));
}

void exitMenu() {
  menuMode = false;
  queueBeep(1, 900, 80);
  Serial.println(F("menu_off"));
}

void menuAction(uint8_t idx, bool longPress) {
  switch (idx) {
    case 0:
      if (longPress) { fanManual = 0; setOv(ovFan); }
      else actFanCycle();
      break;
    case 1:
      if (longPress) { ledWManual = false; setOv(ovLedW); }
      else actLedWToggle();
      break;
    case 2:
      if (longPress) { ledYManual = 0; setOv(ovLedY); }
      else actLedYCycle();
      break;
    case 3:
      if (longPress) { setRelay(false); setOv(ovRelay); }
      else actRelayToggle();
      break;
    case 4:
      actWindowToggle();
      break;
    case 5:                                   // CỬA CHÍNH: bấm = mở/đóng, giữ = đóng
      if (longPress) closeDoor();
      else actDoorToggle();
      break;
    case 6:
      if (longPress) stopMusic();
      else playSong(melodyPlaying ? (currentSong + 1) % NUM_SONGS : currentSong);
      break;
    case 7:
      if (longPress) timerActive = false;
      else actTimerAdd(1);
      break;
    case 8:
      quietMode = !quietMode;
      break;
    case 9:
      welcomeOn = !welcomeOn;
      if (!welcomeOn) welcomeActive = false;
      break;
    case 10:
      applyServoMap(!servoSwap, true);
      SHOWMSG("Servo swapped", "Saved", 1500);
      break;
    case 11:
      clearOverrides();
      break;
    default:
      exitMenu();
      factoryReset();
      return;
  }
  queueBeep(1, 1400, 40);
}

// =====================================================================================
//                            KHOÁ CỬA MẬT MÃ MORSE (KHÔNG CHẶN)
// =====================================================================================
bool pollButton(Button &b, unsigned long &heldMs) {
  unsigned long now = millis();
  bool raw = (digitalRead(b.pin) == LOW);
  if (raw != b.rawLast) {
    b.rawLast = raw;
    b.changedAt = now;
  }
  if ((now - b.changedAt) >= DEBOUNCE_MS && raw != b.stable) {
    b.stable = raw;
    if (raw) {
      b.pressedAt = now;
    } else {
      heldMs = now - b.pressedAt;
      return true;   // vừa nhả nút
    }
  }
  return false;
}

void clearInput() {
  inputLen = 0;
  inputBuf[0] = 0;
}

void startLockout() {
  unsigned long now = millis();
  uint8_t lv = (lockoutLevel > 3) ? 3 : lockoutLevel;
  lockoutUntil = now + (LOCKOUT_BASE_MS << lv);
  lockoutSirenUntil = now + LOCKOUT_SIREN_MS;
  lockoutActive = true;
  if (lockoutLevel < 250) lockoutLevel++;
  failCount = 0;
  silenceOn = false;
  logEvent(EV_LOCKOUT);
  Serial.println(F("lockout"));
}

void submitPassword() {
  if (inputLen == 0) return;
  if (strcmp(inputBuf, password) == 0) {
    openDoor(DOOR_HOLD_MS);
    if (awayArmed) disarmAway();
    intruderAlarm = false;
    failCount = 0;
    lockoutLevel = 0;
    SHOWMSG("Door OPEN!", "Welcome home", DOOR_HOLD_MS);
    queueBeep(2, 1500, 80);
    Serial.println(F("door_open"));
  } else {
    failCount++;
    failTotal++;
    logEvent(EV_PW_WRONG);
    SHOWMSG("Wrong password!", "Try again", 2000);
    queueBeep(1, 300, 400);
    Serial.println(F("password_wrong"));
    if (failCount >= MAX_FAILS) startLockout();
  }
  clearInput();
}

void handleButtons() {
  unsigned long now = millis();
  unsigned long h1 = 0, h2 = 0;
  bool e1 = pollButton(btn1, h1);   // e1/e2 = vừa nhả nút, h = thời gian giữ
  bool e2 = pollButton(btn2, h2);

  // ---------- Tổ hợp 2 nút: menu / dừng tất cả ----------
  if (!comboActive && btn1.stable && btn2.stable) {
    comboActive = true;
    comboStart = now;
    comboBeeped = false;
  }
  if (comboActive) {
    lastActivity = now;
    if (!comboBeeped && (now - comboStart) >= RESET_HOLD_MS) {
      comboBeeped = true;
      queueBeep(2, 1800, 80);        // báo đã giữ đủ lâu để reset
    }
    if (!btn1.stable && !btn2.stable) {   // cả hai đã nhả
      unsigned long held = now - comboStart;
      comboActive = false;
      comboBeeped = false;
      b1LongDone = false;
      b2LongDone = false;
      b1StopDone = false;
      if (awayArmed || lockoutActive || intruderAlarm) {
        queueBeep(1, 300, 300);      // đang khoá an ninh: từ chối
      } else if (held >= RESET_HOLD_MS) {
        factoryReset();
      } else if (held >= COMBO_MIN_MS) {
        if (menuMode) exitMenu(); else enterMenu();
      }
    }
    return;
  }

  // ---------- Bíp báo đã giữ đủ lâu ----------
  if (btn2.stable && !btn1.stable && !b2LongDone && (now - btn2.pressedAt) >= CLEAR_HOLD_MS) {
    b2LongDone = true;
    queueBeep(1, 1300, 100);
  }
  if (menuMode && btn1.stable && !btn2.stable) {
    unsigned long held = now - btn1.pressedAt;
    menuLastAt = now;                                    // đang giữ nút thì không tự thoát menu
    if (!b1LongDone && held >= CLEAR_HOLD_MS) {          // 1.5s: nhả ra = thoát menu
      b1LongDone = true;
      queueBeep(1, 1300, 100);
    }
    if (!b1StopDone && held >= MUSIC_STOP_HOLD_MS) {     // 4s: nhả ra = tắt nhạc
      b1StopDone = true;
      queueBeep(2, 1800, 80);
    }
  }

  if (menuMode) {
    // ---------- Chế độ MENU ----------
    if (e1) {
      lastActivity = now;
      menuLastAt = now;
      if (h1 >= MUSIC_STOP_HOLD_MS) {
        stopMusic();                                     // giữ >= 4s ở bất kỳ mục nào: tắt nhạc
        SHOWMSG("Music OFF", "Stopped", 1500);
      } else if (h1 >= CLEAR_HOLD_MS) {
        exitMenu();
      } else {
        menuIdx = (menuIdx + 1) % MENU_ITEMS;
        queueBeep(1, 1000, 30);
      }
    }
    if (e2 && menuMode) {
      lastActivity = now;
      menuLastAt = now;
      menuAction(menuIdx, h2 >= CLEAR_HOLD_MS);
    }
    if (menuMode && (now - menuLastAt) >= MENU_TIMEOUT_MS) exitMenu();
  } else {
    // ---------- Chế độ KHOÁ CỬA MORSE ----------
    if (e1) {
      lastActivity = now;
      if (lockoutActive) {
        queueBeep(1, 300, 100);
      } else if (inputLen < MAX_PW_LEN) {
        char c = (h1 >= DASH_MIN_MS) ? '-' : '.';
        inputBuf[inputLen++] = c;
        inputBuf[inputLen] = 0;
        lastInputAt = now;
        if (c == '.') queueBeep(1, 1800, 40);
        else queueBeep(1, 1000, 120);
        Serial.print(c);
      } else {
        queueBeep(1, 300, 100);
      }
    }
    if (e2) {
      lastActivity = now;
      if (lockoutActive) {
        queueBeep(1, 300, 100);
      } else if (h2 >= CLEAR_HOLD_MS) {
        if (inputLen > 0) {
          clearInput();
          SHOWMSG("Input cleared", "", 1200);
          Serial.println(F("input_cleared"));
        } else if (!awayArmed) {
          armAway();
        }
      } else {
        Serial.println();
        submitPassword();
      }
    }
    if (inputLen > 0 && (now - lastInputAt) >= INPUT_TIMEOUT_MS) clearInput();
  }

  if (e1) { b1LongDone = false; b1StopDone = false; }
  if (e2) b2LongDone = false;
}

// =====================================================================================
//                                     LƯU MẬT MÃ
// =====================================================================================
void loadPassword() {
  if (EEPROM.read(0) == 0xA5) {
    uint8_t len = EEPROM.read(1);
    if (len >= 3 && len <= MAX_PW_LEN) {
      bool ok = true;
      for (uint8_t i = 0; i < len; i++) {
        char c = (char)EEPROM.read(2 + i);
        if (c != '.' && c != '-') { ok = false; break; }
        password[i] = c;
      }
      if (ok) {
        password[len] = 0;
        return;
      }
    }
  }
  strcpy(password, PASSWORD_DEFAULT);
}

void savePassword() {
  uint8_t len = strlen(password);
  EEPROM.update(0, 0xA5);
  EEPROM.update(1, len);
  for (uint8_t i = 0; i < len; i++) EEPROM.update(2 + i, password[i]);
}

// =====================================================================================
//                                   CÒI & NHẠC
// =====================================================================================
void setTone(uint16_t f) {
  if (f == curTone) return;
  curTone = f;
  if (f == 0) noTone(PIN_BUZZER);
  else tone(PIN_BUZZER, f);
}

void queueBeep(uint8_t n, uint16_t freq, uint16_t dur) {
  if (quietMode) return;            // chế độ yên lặng: bỏ qua bíp thông báo
  beepQueue = n;
  beepFreq = freq;
  beepDur = dur;
  beepActive = false;
  beepNextAt = millis();
}

void startMelody(const uint16_t *notes, const uint8_t *durs, uint8_t len) {
  melNotes = notes;
  melDurs = durs;
  melLen = len;
  melIdx = 0;
  melNoteStart = millis();
  melodyPlaying = true;
  wasPlaying = true;
}

// Chuông ding-dong chào khách (~1.8s). Không đổi bài đang chọn, không chen vào nhạc đang phát.
void startChime() {
  if (quietMode || melodyPlaying || playlistOn) return;
  startMelody(BELL_NOTES, BELL_DURS, sizeof(BELL_DURS));
}

void stopMusic() {
  melodyPlaying = false;
  wasPlaying = false;
  playlistOn = false;
  songPending = false;
  beepQueue = 0;
  setTone(0);
}

void songName(uint8_t id, char *out) {
  switch (id) {
    case 0:  strcpy_P(out, PSTR("Happy Birthday")); break;
    case 1:  strcpy_P(out, PSTR("Ode to Joy")); break;
    case 2:  strcpy_P(out, PSTR("Twinkle Star")); break;
    case 3:  strcpy_P(out, PSTR("Jingle Bells")); break;
    case 4:  strcpy_P(out, PSTR("Fur Elise")); break;
    case 5:  strcpy_P(out, PSTR("Mary's Lamb")); break;
    case 6:  strcpy_P(out, PSTR("Frere Jacques")); break;
    case 7:  strcpy_P(out, PSTR("Silent Night")); break;
    case 8:  strcpy_P(out, PSTR("Merry Christmas")); break;
    case 9:  strcpy_P(out, PSTR("HUST Fanfare")); break;
    case 10: strcpy_P(out, PSTR("Door Bell")); break;
    default: strcpy_P(out, PSTR("Happy Birthday")); break;
  }
}

void playSong(uint8_t id) {
  id = id % NUM_SONGS;
  currentSong = id;
  songPending = false;
  switch (id) {
    case 0:  startMelody(BIRTHDAY_NOTES, BIRTHDAY_DURS, sizeof(BIRTHDAY_DURS)); break;
    case 1:  startMelody(ODE_NOTES, ODE_DURS, sizeof(ODE_DURS)); break;
    case 2:  startMelody(TWINKLE_NOTES, TWINKLE_DURS, sizeof(TWINKLE_DURS)); break;
    case 3:  startMelody(JINGLE_NOTES, JINGLE_DURS, sizeof(JINGLE_DURS)); break;
    case 4:  startMelody(ELISE_NOTES, ELISE_DURS, sizeof(ELISE_DURS)); break;
    case 5:  startMelody(MARY_NOTES, MARY_DURS, sizeof(MARY_DURS)); break;
    case 6:  startMelody(FRERE_NOTES, FRERE_DURS, sizeof(FRERE_DURS)); break;
    case 7:  startMelody(SILENT_NOTES, SILENT_DURS, sizeof(SILENT_DURS)); break;
    case 8:  startMelody(XMAS_NOTES, XMAS_DURS, sizeof(XMAS_DURS)); break;
    case 9:  startMelody(FANFARE_NOTES, FANFARE_DURS, sizeof(FANFARE_DURS)); break;
    case 10: startMelody(BELL_NOTES, BELL_DURS, sizeof(BELL_DURS)); break;
    default: startMelody(BIRTHDAY_NOTES, BIRTHDAY_DURS, sizeof(BIRTHDAY_DURS)); break;
  }
  Serial.print(F("song:"));
  Serial.println(id + 1);
}

uint16_t melodyFreq(unsigned long now) {
  unsigned long dur = (unsigned long)pgm_read_byte(&melDurs[melIdx]) * 25UL * 100UL / tempoPct;
  if ((now - melNoteStart) >= dur) {
    melIdx++;
    melNoteStart = now;
    if (melIdx >= melLen) {
      melodyPlaying = false;
      return 0;
    }
    dur = (unsigned long)pgm_read_byte(&melDurs[melIdx]) * 25UL * 100UL / tempoPct;
  }
  if ((now - melNoteStart) > (dur / 10UL * 9UL)) return 0;   // khoảng nghỉ giữa các nốt
  return pgm_read_word(&melNotes[melIdx]);
}

void updateMusic() {                 // xử lý phát liên tục (playlist)
  unsigned long now = millis();
  if (wasPlaying && !melodyPlaying) {
    wasPlaying = false;
    if (playlistOn) {
      songPending = true;
      songGapUntil = now + 800UL;
    }
  }
  if (songPending && TIME_REACHED(now, songGapUntil)) {
    songPending = false;
    if (playlistOn) playSong((currentSong + 1) % NUM_SONGS);
  }
}

void updateBuzzer() {
  unsigned long now = millis();

  if (beepActive && TIME_REACHED(now, beepEndAt)) beepActive = false;
  if (!beepActive && beepQueue > 0 && TIME_REACHED(now, beepNextAt)) {
    beepActive = true;
    beepEndAt = now + beepDur;
    beepNextAt = now + beepDur + 80;
    beepQueue--;
  }

  uint16_t f = 0;
  bool siren = !sirenSilenced();
  if (gasState == GAS_DANGER && siren) {                      // ưu tiên 1: gas nguy hiểm
    unsigned long p = now % 700;
    f = (p < 125 || (p >= 225 && p < 350)) ? 880 : 0;
  } else if (intruderAlarm && siren) {                        // ưu tiên 2: đột nhập
    f = ((now / 150) % 2) ? 900 : 600;
  } else if (lockoutActive && !TIME_REACHED(now, lockoutSirenUntil) && siren) {
    f = ((now / 150) % 2) ? 900 : 600;                        // ưu tiên 3: bị khoá do sai mã
  } else if (beepActive) {                                    // ưu tiên 4: bíp thông báo
    f = beepFreq;
  } else if (melodyPlaying) {                                 // ưu tiên 5: nhạc
    f = melodyFreq(now);
  }
  setTone(f);
}

// =====================================================================================
//                                      LCD
// =====================================================================================
void lcdLine(uint8_t row, const char *txt) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16s", txt);
  if (strcmp(buf, lcdCache[row]) != 0) {
    strcpy(lcdCache[row], buf);
    mylcd.setCursor(0, row);
    mylcd.print(buf);
  }
}

void showMsgP(const char *a, const char *b, unsigned long ms) {
  strncpy_P(msgA, a, 16); msgA[16] = 0;
  strncpy_P(msgB, b, 16); msgB[16] = 0;
  msgActive = true;
  msgUntil = millis() + ms;
}

uint8_t nextPage(uint8_t p) {
  for (uint8_t i = 0; i < PAGE_COUNT; i++) {
    p = (p + 1) % PAGE_COUNT;
    if (p == 4 && !timerActive) continue;
    if (p == 5 && !melodyPlaying) continue;
    return p;
  }
  return 0;
}

void updateLCD() {
  unsigned long now = millis();
  if (!TIME_REACHED(now, lcdNextAt)) return;
  lcdNextAt = now + LCD_REFRESH_MS;

  bool alarmNow = (gasState == GAS_DANGER) || intruderAlarm || lockoutActive;
  bool wantBl = true;
#if LCD_BACKLIGHT_TIMEOUT_MS > 0
  wantBl = alarmNow || ((now - lastActivity) < LCD_BACKLIGHT_TIMEOUT_MS);
#endif
  if (wantBl != blOn) {
    blOn = wantBl;
    if (blOn) mylcd.backlight();
    else mylcd.noBacklight();
  }

  if (TIME_REACHED(now, pageNextAt)) {
    pageNextAt = now + LCD_PAGE_MS;
    lcdPage = nextPage(lcdPage);
  }
  if ((lcdPage == 4 && !timerActive) || (lcdPage == 5 && !melodyPlaying)) lcdPage = nextPage(lcdPage);

  char a[24] = "";
  char b[24] = "";
  char nm[17];

  if (gasState == GAS_DANGER) {
    if ((now / 500) % 2) SNP(a, "!! GAS DANGER !!");
    SNP(b, "Gas:%4d Vent.", gasVal);
  } else if (intruderAlarm) {
    if ((now / 400) % 2) SNP(a, "!! INTRUDER !!");
    SNP(b, "Enter password");
  } else if (lockoutActive) {
    SNP(a, "LOCKED!");
    SNP(b, "Wait %lu s", (lockoutUntil - now) / 1000UL + 1);
  } else if (msgActive && !TIME_REACHED(now, msgUntil)) {
    snprintf(a, sizeof(a), "%s", msgA);
    snprintf(b, sizeof(b), "%s", msgB);
  } else if (menuMode) {
    menuName(menuIdx, nm);
    SNP(a, "%d/%d %s", menuIdx + 1, MENU_ITEMS, nm);
    switch (menuIdx) {
      case 0: SNP(b, "Speed:%3d%% %S", (int)((long)fanCur * 100 / 255), isOv(ovFan) ? PSTR("MAN") : PSTR("AUTO")); break;
      case 1: SNP(b, "State: %S", ONOFF(digitalRead(PIN_LED_W) == HIGH)); break;
      case 2: SNP(b, "Level: %3d%%", (int)((long)ledYCur * 100 / 255)); break;
      case 3: SNP(b, "State: %S", ONOFF(relayOn)); break;
      case 4: SNP(b, "%S B2=toggle", (winTarget == WINDOW_OPEN_ANGLE) ? PSTR("OPEN") : PSTR("CLOSED")); break;
      case 5: SNP(b, "%S B2=toggle", (doorTarget != DOOR_CLOSED_ANGLE) ? PSTR("OPEN") : PSTR("CLOSED")); break;
      case 6:
        if (melodyPlaying) {
          if ((now / 2000UL) % 2) {
            songName(currentSong, nm);
            SNP(b, "\003 %s", nm);
          } else {
            SNP(b, "B1 hold 4s=OFF");
          }
        } else {
          SNP(b, "Stopped B2=play");
        }
        break;
      case 7:
        if (timerActive) {
          unsigned long rem = (timerEndAt - now) / 1000UL;
          SNP(b, "%02lu:%02lu B2=+1m", rem / 60UL, rem % 60UL);
        } else {
          SNP(b, "Off  B2=+1min");
        }
        break;
      case 8: SNP(b, "Quiet: %S", ONOFF(quietMode)); break;
      case 9: SNP(b, "Visitor: %S", ONOFF(welcomeOn)); break;
      case 10: SNP(b, "Door=D%d Win=D%d", pinServoDoor, pinServoWin); break;
      case 11: SNP(b, "B2 = to AUTO"); break;
      default: SNP(b, "B2 = RESET ALL"); break;
    }
  } else if (inputLen > 0) {
    SNP(a, "Password:");
    snprintf(b, sizeof(b), "%s", inputBuf);
  } else if (gasState == GAS_WARN) {
    SNP(a, "GAS WARNING");
    SNP(b, "Level:%4d", gasVal);
  } else if (gasState == GAS_WARMUP) {
    unsigned long rem = TIME_REACHED(now, warmupEndAt) ? 0UL : (warmupEndAt - now) / 1000UL;
    SNP(a, "Smart Home v%s", FW_VERSION);
    SNP(b, "Warm-up: %2lus", rem);
  } else {
    msgActive = false;
    switch (lcdPage) {
      case 0:
        SNP(a, "G:%4d L:%4d", gasVal, lightVal);
        SNP(b, "R:%4d S:%4d", waterVal, soilVal);
        break;
      case 1:
        SNP(a, "D:%S W:%S", (doorTarget != DOOR_CLOSED_ANGLE) ? PSTR("OPEN") : PSTR("LOCK"),
            (winTarget == WINDOW_OPEN_ANGLE) ? PSTR("OPEN") : PSTR("SHUT"));
        SNP(b, "Fan:%3d%%%c P:%S", (int)((long)fanCur * 100 / 255), fanWhy, ONOFF(relayOn));
        break;
      case 2:
        SNP(a, "Mode:%S", anyOverride() ? PSTR("MANUAL") : PSTR("AUTO"));
        SNP(b, "Sec:%S Rn:%S", awayArmed ? PSTR("AWAY") : PSTR("HOME"),
            raining ? PSTR("YES") : vaporHigh ? PSTR("VAP") : PSTR("NO"));
        break;
      case 3: {
        unsigned long s = now / 1000UL;
        SNP(a, "Up %02lu:%02lu:%02lu", s / 3600UL, (s / 60UL) % 60UL, s % 60UL);
        SNP(b, "Gas:%u Door:%u", gasAlarmCount, doorOpenCount);
        break;
      }
      case 4: {
        unsigned long rem = TIME_REACHED(now, timerEndAt) ? 0UL : (timerEndAt - now) / 1000UL;
        SNP(a, "Timer running");
        SNP(b, "%02lu:%02lu left", rem / 60UL, rem % 60UL);
        break;
      }
      case 5:
        songName(currentSong, nm);
        SNP(a, "\003 Now playing");
        snprintf(b, sizeof(b), "%s", nm);
        break;
      default: {                      // trang LOGO HUST (bánh răng quay)
        uint8_t fr = (now / 600UL) & 1;
        if (fr != logoFrame) {
          logoFrame = fr;
          setGear(fr);
        }
        SNP(a, "\001\002 HUST 1956");
        SNP(b, "BACH KHOA HA NOI");
        break;
      }
    }
  }
  lcdLine(0, a);
  lcdLine(1, b);
}

// =====================================================================================
//                                DỪNG TẤT CẢ (RESET)
// =====================================================================================
void factoryReset() {
  unsigned long now = millis();

  // Âm thanh
  melodyPlaying = false;
  wasPlaying = false;
  playlistOn = false;
  songPending = false;
  tempoPct = 100;
  beepQueue = 0;
  beepActive = false;
  silenceOn = false;
  quietMode = false;
  setTone(0);

  // Thiết bị & lệnh thủ công
  clearOverrides();
  ledWManual = false;
  ledYManual = 0;
  fanManual = 0;
  fanRunOn = false;
  fanCur = 0;
  fanTarget = 0;
  ledYCur = 0;
  // BRAKE mode: INA=HIGH + INB=HIGH = dừng hẳn (không coast)
  digitalWrite(PIN_FAN_DIR, HIGH);
  digitalWrite(PIN_FAN_PWM, HIGH);
  digitalWrite(PIN_LED_W, LOW);
  analogWrite(PIN_LED_Y, 0);
  setRelay(false);
  pumpState = PUMP_IDLE;
  pumpWasOv = false;
  doorTimerOn = false;
  doorTarget = DOOR_CLOSED_ANGLE;
  winManualAngle = WINDOW_OPEN_ANGLE;
  winTarget = WINDOW_OPEN_ANGLE;
  testStage = 0;
  welcomeActive = false;
  welcomeOn = WELCOME_DEFAULT_ON;
  vaporHigh = false;
  humidEnabled = HUMID_ENABLE_DEFAULT;
  fanWhy = '-';

  // An ninh
  awayArmed = false;
  armPending = false;
  intruderAlarm = false;
  lockoutActive = false;
  failCount = 0;
  lockoutLevel = 0;
  clearInput();
  comboActive = false;
  b1LongDone = false;
  b2LongDone = false;
  b1StopDone = false;

  // Menu, hẹn giờ, telemetry, thống kê
  menuMode = false;
  menuIdx = 0;
  timerActive = false;
  telemetryOn = TELEMETRY_DEFAULT_ON;
  gasAlarmCount = 0;
  doorOpenCount = 0;
  intruderCount = 0;
  pumpRuns = 0;
  failTotal = 0;
  visitorCount = 0;
  logHead = 0;
  logCount = 0;
  pendingCmd = 0;
  argLen = 0;

  // Cảm biến gas: ân hạn như lúc khởi động
  gasState = GAS_WARMUP;
  warmupEndAt = now + RESET_GRACE_MS;

  lastActivity = now;
  lcdPage = 0;
  logEvent(EV_RESET);
  SHOWMSG("SYSTEM RESET", "Back to default", 2500);
  queueBeep(2, 1200, 100);
  Serial.println(F("system_reset"));
}

// =====================================================================================
//                              LỆNH SERIAL / BLUETOOTH
// =====================================================================================
void printBanner() {
  Serial.println(F("=== BACH KHOA HA NOI - HUST ==="));
  Serial.println(F("SMART HOME PRO v" FW_VERSION));
}

void printHelp() {
  printBanner();
  Serial.println(F("a/b den W | p/q den Y | v<n># PWM Y | c/d relay"));
  Serial.println(F("r/s quat max/tat | w<n># | +/- quat | F dao quat"));
  Serial.println(F("l/m cua | t<n># | n/o cua so | u<n># | D dao cua"));
  Serial.println(F("e=K1 f=K2 | K<1-11># bai | N/B ke/truoc | g dung"));
  Serial.println(F("G playlist | <> tempo | Q yen lang | A<phut># hen"));
  Serial.println(F("h=anh sang i=gas j=dat k=nuoc | y=status z=json"));
  Serial.println(F("S=tat coi | R=reset khoa | H=di vang | P<ma>#=doi"));
  Serial.println(F("V=chao khach | C=test servo | M=doi servo+EEPROM"));
  Serial.println(F("U=quat-am | Z=xem nguong am | X=auto | x=RESET TAT CA"));
  Serial.println(F("Giu 2 nut: 0.8s=MENU | >=3s=RESET TAT CA | ?=help"));
}

void printStatus() {
  char nm[17];
  Serial.println(F("===== SMART HOME STATUS ====="));
  Serial.print(F("Gas   : ")); Serial.print(gasVal); Serial.print(F("  [")); Serial.print(gasStateName()); Serial.println(F("]"));
  Serial.print(F("Light : ")); Serial.print(lightVal); Serial.println(isDark ? F("  (dark)") : F("  (bright)"));
  Serial.print(F("Soil  : ")); Serial.print(soilVal); Serial.println(soilDry ? F("  (DRY)") : F("  (ok)"));
  Serial.print(F("Water : ")); Serial.print(waterVal);
  Serial.println(raining ? F("  (RAIN)") : vaporHigh ? F("  (VAPOR)") : F("  (normal)"));
  Serial.print(F("PIR   : ")); Serial.println(pirVal);
  Serial.print(F("Door  : ")); Serial.print(doorCur); Serial.print(F(" deg | Window: ")); Serial.print(winCur); Serial.println(F(" deg"));
  Serial.print(F("Fan   : ")); Serial.print(fanCur);
  Serial.print(F("/255 why=")); Serial.print(fanWhy);
  Serial.print(F(" [D=DGR W=WARN H=HUMID M=MAN R=RunOn] Relay:"));
  Serial.println(relayOn ? F("ON") : F("OFF"));
  Serial.print(F("Vapor : now=")); Serial.print(waterVal);
  Serial.print(F(" on>=")); Serial.print(VAPOR_HIGH_ON);
  Serial.print(F(" off<")); Serial.print(VAPOR_HIGH_OFF);
  Serial.print(F(" | vaporHigh=")); Serial.println(vaporHigh ? F("YES") : F("NO"));
  Serial.print(F("Auto-fan: ")); Serial.println(humidEnabled ? F("ON") : F("OFF"));
  Serial.print(F("Servo : door D")); Serial.print(pinServoDoor); Serial.print(F(" | window D")); Serial.println(pinServoWin);
  Serial.print(F("LED   : white ")); Serial.print(digitalRead(PIN_LED_W) == HIGH ? F("ON") : F("OFF"));
  Serial.print(F(" | yellow ")); Serial.println(ledYCur);
  Serial.print(F("Mode  : ")); Serial.print(anyOverride() ? F("MANUAL") : F("AUTO"));
  Serial.print(F(" | Away: ")); Serial.print(awayArmed ? F("ARMED") : F("OFF"));
  Serial.print(F(" | Lockout: ")); Serial.print(lockoutActive ? F("YES") : F("NO"));
  Serial.print(F(" | Quiet: ")); Serial.print(quietMode ? F("ON") : F("OFF"));
  Serial.print(F(" | Welcome: ")); Serial.println(welcomeOn ? F("ON") : F("OFF"));
  songName(currentSong, nm);
  Serial.print(F("Music : ")); Serial.print(melodyPlaying ? F("playing ") : F("stopped "));
  Serial.print(nm); Serial.print(F(" | tempo ")); Serial.print(tempoPct); Serial.print(F("% | playlist "));
  Serial.println(playlistOn ? F("ON") : F("OFF"));
  Serial.print(F("Timer : "));
  if (timerActive) { Serial.print((timerEndAt - millis()) / 1000UL); Serial.println(F(" s left")); }
  else Serial.println(F("off"));
  Serial.print(F("Stats : gasAlarm=")); Serial.print(gasAlarmCount);
  Serial.print(F(" doorOpen=")); Serial.print(doorOpenCount);
  Serial.print(F(" intruder=")); Serial.print(intruderCount);
  Serial.print(F(" pumpRuns=")); Serial.print(pumpRuns);
  Serial.print(F(" visitors=")); Serial.print(visitorCount);
  Serial.print(F(" wrongPw=")); Serial.println(failTotal);
}

void printJson() {
  Serial.print(F("{\"gas\":")); Serial.print(gasVal);
  Serial.print(F(",\"gas_state\":\"")); Serial.print(gasStateName()); Serial.print(F("\""));
  Serial.print(F(",\"light\":")); Serial.print(lightVal);
  Serial.print(F(",\"soil\":")); Serial.print(soilVal);
  Serial.print(F(",\"water\":")); Serial.print(waterVal);
  Serial.print(F(",\"pir\":")); Serial.print(pirVal);
  Serial.print(F(",\"rain\":")); Serial.print(raining ? 1 : 0);
  Serial.print(F(",\"humid\":")); Serial.print(vaporHigh ? 1 : 0);
  Serial.print(F(",\"fan_why\":\"")); Serial.print(fanWhy); Serial.print(F("\""));
  Serial.print(F(",\"soil_dry\":")); Serial.print(soilDry ? 1 : 0);
  Serial.print(F(",\"door\":")); Serial.print(doorCur);
  Serial.print(F(",\"window\":")); Serial.print(winCur);
  Serial.print(F(",\"fan\":")); Serial.print(fanCur);
  Serial.print(F(",\"relay\":")); Serial.print(relayOn ? 1 : 0);
  Serial.print(F(",\"away\":")); Serial.print(awayArmed ? 1 : 0);
  Serial.print(F(",\"lockout\":")); Serial.print(lockoutActive ? 1 : 0);
  Serial.print(F(",\"visitors\":")); Serial.print(visitorCount);
  Serial.print(F(",\"music\":")); Serial.print(melodyPlaying ? currentSong + 1 : 0);
  Serial.println(F("}"));
}

void execCommand(char c) {
  unsigned long now = millis();
  lastActivity = now;
  switch (c) {
    case 'a': ledWManual = true;  setOv(ovLedW); break;
    case 'b': ledWManual = false; setOv(ovLedW); break;
    case 'c': setRelay(true);  setOv(ovRelay); break;
    case 'd': setRelay(false); setOv(ovRelay); break;
    case 'e': playSong(0); break;
    case 'f': playSong(1); break;
    case 'g': stopMusic(); Serial.println(F("music_stop")); break;
    case 'h': Serial.println(lightVal); break;
    case 'i': Serial.println(gasVal); break;
    case 'j': Serial.println(soilVal); break;
    case 'k': Serial.println(waterVal); break;
    case 'l': openDoor(DOOR_HOLD_MS); break;
    case 'm': closeDoor(); break;
    case 'n': winManualAngle = WINDOW_CLOSED_ANGLE; setOv(ovWin); break;
    case 'o': winManualAngle = WINDOW_OPEN_ANGLE;   setOv(ovWin); break;
    case 'p': ledYManual = 255; setOv(ovLedY); break;
    case 'q': ledYManual = 0;   setOv(ovLedY); break;
    case 'r': fanManual = 255;  setOv(ovFan); break;
    case 's': fanManual = 0;    setOv(ovFan); break;
    case 'x': factoryReset(); break;
    case 'X':
      clearOverrides();
      queueBeep(1, 1500, 80);
      Serial.println(F("mode_auto"));
      break;
    case 'y': printStatus(); break;
    case 'z': printJson(); break;
    case 'E': printLog(); break;
    case 'T': telemetryOn = !telemetryOn; Serial.println(telemetryOn ? F("telemetry_on") : F("telemetry_off")); break;
    case 'S': silenceOn = true; silenceUntil = now + SILENCE_MS; Serial.println(F("siren_silenced")); break;
    case 'R':
      lockoutActive = false; failCount = 0; lockoutLevel = 0;
      intruderAlarm = false; silenceOn = false;
      Serial.println(F("reset_done"));
      break;
    case 'H': if (awayArmed) disarmAway(); else armAway(); break;
    case 'V':
      welcomeOn = !welcomeOn;
      if (!welcomeOn) welcomeActive = false;
      Serial.println(welcomeOn ? F("welcome_on") : F("welcome_off"));
      break;
    case 'C': startServoTest(); break;
    case 'M':
      applyServoMap(!servoSwap, true);
      Serial.print(F("servo_swapped: door D")); Serial.print(pinServoDoor);
      Serial.print(F(" window D")); Serial.println(pinServoWin);
      break;
    case 'U':
      humidEnabled = !humidEnabled;
      Serial.println(humidEnabled ? F("humid_fan_on") : F("humid_fan_off"));
      break;
    case 'Z':  // v3.3: không còn mức nền; in ngưỡng hơi nước hiện tại
      Serial.print(F("vapor_on>=")); Serial.print(VAPOR_HIGH_ON);
      Serial.print(F(" off<")); Serial.print(VAPOR_HIGH_OFF);
      Serial.print(F(" now=")); Serial.println(waterVal);
      break;
    case '+': actFanAdjust(30); break;
    case '-': actFanAdjust(-30); break;
    case 'F': fanManual = (fanCur > 0) ? 0 : 255; setOv(ovFan); break;
    case 'L': actLedWToggle(); break;
    case 'Y': actLedYCycle(); break;
    case 'W': actWindowToggle(); break;
    case 'D': actDoorToggle(); break;
    case 'N': playSong(currentSong + 1); break;
    case 'B': playSong(currentSong == 0 ? NUM_SONGS - 1 : currentSong - 1); break;
    case 'G':
      playlistOn = !playlistOn;
      if (playlistOn && !melodyPlaying) playSong(currentSong);
      Serial.println(playlistOn ? F("playlist_on") : F("playlist_off"));
      break;
    case '<': tempoPct = (tempoPct > 60) ? tempoPct - 10 : 50; Serial.print(F("tempo:")); Serial.println(tempoPct); break;
    case '>': tempoPct = (tempoPct < 190) ? tempoPct + 10 : 200; Serial.print(F("tempo:")); Serial.println(tempoPct); break;
    case 'Q': quietMode = !quietMode; Serial.println(quietMode ? F("quiet_on") : F("quiet_off")); break;
    case '?': printHelp(); break;
    default: break;
  }
}

void execArgCommand(char cmd, char *arg) {
  unsigned long now = millis();
  lastActivity = now;
  if (cmd == 'P') {
    uint8_t len = strlen(arg);
    bool ok = (len >= 3 && len <= MAX_PW_LEN);
    for (uint8_t i = 0; ok && i < len; i++) {
      if (arg[i] != '.' && arg[i] != '-') ok = false;
    }
    if (ok) {
      strcpy(password, arg);
      savePassword();
      Serial.println(F("password_changed"));
    } else {
      Serial.println(F("password_invalid (3-12 ky tu . va -)"));
    }
    return;
  }
  int v = atoi(arg);
  switch (cmd) {
    case 't':
      doorTarget = constrain(v, 0, 180);
      if (doorTarget != DOOR_CLOSED_ANGLE) { doorTimerOn = true; doorCloseAt = now + DOOR_HOLD_MS; }
      else doorTimerOn = false;
      break;
    case 'u': winManualAngle = constrain(v, 0, 180); setOv(ovWin); break;
    case 'v': ledYManual = constrain(v, 0, 255); setOv(ovLedY); break;
    case 'w': fanManual = constrain(v, 0, 255); setOv(ovFan); break;
    case 'K':
      if (v >= 1 && v <= NUM_SONGS) playSong(v - 1);
      break;
    case 'A':
      v = constrain(v, 0, 99);
      if (v == 0) {
        timerActive = false;
        Serial.println(F("timer_off"));
      } else {
        timerActive = true;
        timerEndAt = now + (unsigned long)v * 60000UL;
        Serial.println(F("timer_on"));
      }
      break;
    default: break;
  }
}

void handleSerial() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (pendingCmd) {
      if (c == '#') {
        argBuf[argLen] = 0;
        execArgCommand(pendingCmd, argBuf);
        pendingCmd = 0;
        argLen = 0;
      } else if (argLen < sizeof(argBuf) - 1) {
        argBuf[argLen++] = c;
      } else {
        pendingCmd = 0;
        argLen = 0;
      }
      continue;
    }
    if (c == '\r' || c == '\n' || c == ' ') continue;
    if (c == 't' || c == 'u' || c == 'v' || c == 'w' || c == 'P' || c == 'K' || c == 'A') {
      pendingCmd = c;
      argLen = 0;
      argStart = millis();
    } else {
      execCommand(c);
    }
  }
  if (pendingCmd && (millis() - argStart) > 1000UL) {  // hết thời gian chờ tham số
    pendingCmd = 0;
    argLen = 0;
  }
}

void updateTelemetry() {
  unsigned long now = millis();
  if (gasState == GAS_DANGER && TIME_REACHED(now, dangerPrintAt)) {
    Serial.println(F("danger"));
    dangerPrintAt = now + 2000UL;
  }
  if (telemetryOn && TIME_REACHED(now, telemAt)) {
    telemAt = now + TELEMETRY_PERIOD_MS;
    printJson();
  }
}

// =====================================================================================
//                            MÀN HÌNH CHÀO: LOGO HUST + NHẠC KHAI MẠC
// =====================================================================================
void splashLogo() {
  mylcd.clear();
  setGear(0);
  mylcd.setCursor(0, 0);
  mylcd.write((uint8_t)1);
  mylcd.write((uint8_t)2);
  mylcd.print(F(" HUST 1956"));

  const uint16_t fan[4] = {523, 659, 784, 1047};   // fanfare khai mạc
  for (uint8_t i = 0; i < 4; i++) {
    setGear(i & 1);
    tone(PIN_BUZZER, fan[i]);
    delay(150);
  }
  noTone(PIN_BUZZER);

  const char *t = PSTR("BACH KHOA HA NOI");       // hiệu ứng gõ chữ
  for (uint8_t i = 0; i < 16; i++) {
    mylcd.setCursor(i, 1);
    mylcd.print((char)pgm_read_byte(t + i));
    if (i % 3 == 0) setGear((i / 3) & 1);
    delay(45);
  }
  setGear(0);
  delay(700);

  mylcd.clear();
  mylcd.setCursor(0, 0);
  mylcd.print(F("Smart Home PRO"));
  mylcd.setCursor(0, 1);
  mylcd.print(F("v" FW_VERSION " booting..."));
  delay(600);
}

// =====================================================================================
//                                       SETUP
// =====================================================================================
void setup() {
  Serial.begin(SERIAL_BAUD);

  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);
  pinMode(PIN_LED_Y, OUTPUT);
  pinMode(PIN_LED_W, OUTPUT);
  pinMode(PIN_FAN_PWM, OUTPUT);
  pinMode(PIN_FAN_DIR, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_GAS, INPUT);
  pinMode(PIN_LIGHT, INPUT);
  pinMode(PIN_SOIL, INPUT);
  pinMode(PIN_WATER, INPUT);

  // BRAKE mode: INA=HIGH + INB=HIGH = dừng hẳn (không coast)
  digitalWrite(PIN_FAN_DIR, HIGH);
  digitalWrite(PIN_FAN_PWM, HIGH);
  digitalWrite(PIN_RELAY, LOW);
  digitalWrite(PIN_LED_W, LOW);
  analogWrite(PIN_LED_Y, 0);

  loadServoMap();
  servoDoor.attach(pinServoDoor);
  servoWin.attach(pinServoWin);
  servoDoor.write(DOOR_CLOSED_ANGLE);
  servoWin.write(WINDOW_OPEN_ANGLE);

  memset(lcdCache, 0, sizeof(lcdCache));
  mylcd.init();
  mylcd.backlight();
  mylcd.createChar(3, noteGlyph);

  digitalWrite(PIN_LED_W, HIGH);        // tự kiểm tra LED trong lúc chiếu logo
  analogWrite(PIN_LED_Y, 255);
  splashLogo();
  digitalWrite(PIN_LED_W, LOW);
  analogWrite(PIN_LED_Y, 0);

  loadPassword();
  clearInput();

  unsigned long now = millis();
  lastActivity = now;
  lcdNextAt = now;
  warmupEndAt = now + WARMUP_MS;
  doorMoveAt = now;                     // servo đã có thời gian về vị trí đầu; sau 0.7s sẽ tự ngắt xung
  winMoveAt = now;

  printBanner();
  Serial.println(F("San sang. Gui '?' de xem danh sach lenh."));

#if USE_WATCHDOG
  wdt_enable(WDTO_4S);
#endif
}

// =====================================================================================
//                                       LOOP
// =====================================================================================
void loop() {
#if USE_WATCHDOG
  wdt_reset();
#endif
  handleSerial();
  readSensors();

  updateGas();
  updateRain();
  updateSoil();
  updateSecurity();
  updateWelcome();

  handleButtons();
  updateTimer();
  updateDoor();
  updateWindow();
  updateTest();
  updateServos();

  updateLighting();
  updateFan();
  updatePump();

  updateMusic();
  updateBuzzer();
  updateLCD();
  updateTelemetry();
}
