# Smart Home Pro v6 — Nhóm 14

Hệ thống **nhà thông minh chạy trên Arduino UNO + Kit Keyestudio Smart Home (KS0085)**, được xây dựng bởi **Nguyễn Đoàn Nhất Phong** và **Nguyễn Duy Quang** — **HUST, Trường Công nghệ Thông tin và Truyền thông (SoICT)**.

Dự án tập trung vào tự động hóa, an ninh, giám sát khí gas, chiếu sáng, thông gió, cửa tự động, tưới cây, chào khách, phát nhạc và hẹn giờ. Điểm nổi bật về kỹ thuật là kiến trúc **non-blocking dựa trên `millis()`**, máy trạng thái (state machine), cơ chế **override**, nhật ký sự kiện dạng **black box** và lưu cấu hình bằng **EEPROM**.

---

## Mục lục

- [1. Tổng quan hệ thống](#1-tổng-quan-hệ-thống)
- [2. Phần cứng](#2-phần-cứng)
- [3. An ninh — khóa cửa bằng mật mã Morse](#3-an-ninh--khóa-cửa-bằng-mật-mã-morse)
- [4. Giám sát khí gas MQ-2](#4-giám-sát-khí-gas-mq-2)
- [5. Chiếu sáng thông minh](#5-chiếu-sáng-thông-minh)
- [6. Hệ thống quạt](#6-hệ-thống-quạt)
- [7. Cửa sổ tự động](#7-cửa-sổ-tự-động)
- [8. Cửa chính](#8-cửa-chính)
- [9. Hơi nước, mưa và tưới cây](#9-hơi-nước-mưa-và-tưới-cây)
- [10. Chào khách tự động](#10-chào-khách-tự-động)
- [11. Máy phát nhạc](#11-máy-phát-nhạc)
- [12. Hẹn giờ](#12-hẹn-giờ)
- [13. Giao diện người dùng](#13-giao-diện-người-dùng)
- [14. Giao thức Serial](#14-giao-thức-serial)
- [15. Override và chế độ tự động](#15-override-và-chế-độ-tự-động)
- [16. Black Box — nhật ký sự kiện](#16-black-box--nhật-ký-sự-kiện)
- [17. Thống kê và telemetry](#17-thống-kê-và-telemetry)
- [18. Kỹ thuật xử lý tín hiệu và an toàn](#18-kỹ-thuật-xử-lý-tín-hiệu-và-an-toàn)
- [19. Tóm tắt khả năng hệ thống](#19-tóm-tắt-khả-năng-hệ-thống)

---

## 1. Tổng quan hệ thống

### Thông tin dự án

| Hạng mục | Chi tiết |
|---|---|
| Tên | **Smart Home Pro v6 — Nhóm 14** |
| Nhóm | Nguyễn Đoàn Nhất Phong, Nguyễn Duy Quang |
| Trường | HUST — Trường CNTT&TT (SoICT) |
| Nền tảng | Arduino UNO |
| Kit | Keyestudio Smart Home (KS0085) |
| LCD | 16x2 I2C, địa chỉ `0x27` |

### Kiến trúc phần mềm

Hệ thống được thiết kế theo hướng **không chặn hoàn toàn (non-blocking)**:

- Dựa trên `millis()` thay cho các `delay()` dài.
- Dùng **state machine** cho các chức năng cần nhiều trạng thái như khí gas, bơm tưới và servo.
- Có **hàng đợi beep** để phát âm báo mà không dừng các tác vụ khác.
- Mỗi thiết bị quan trọng có cơ chế **override thủ công**.
- Có **black box log** lưu các sự kiện gần nhất.
- Cấu hình quan trọng được lưu bằng **EEPROM**, có thể tồn tại sau khi reset/mất điện.

---

## 2. Phần cứng

Hệ thống sử dụng các thành phần chính:

- Cảm biến khí gas **MQ-2**
- Cảm biến ánh sáng
- Cảm biến độ ẩm đất
- Cảm biến hơi nước/mưa
- Cảm biến chuyển động **PIR**
- Còi **buzzer**
- 2 nút nhấn
- 2 LED: **trắng + vàng PWM**
- Quạt điều khiển qua driver **L9110** với chân PWM + chân hướng
- Relay điều khiển bơm tưới
- 2 servo: **cửa chính + cửa sổ**
- LCD 16x2 I2C (`0x27`)

> **Ghi chú phần cứng:** servo cửa sổ của nhóm bị hỏng phần cứng nên trong thực tế chỉ có servo cửa chính hoạt động.

---

## 3. An ninh — khóa cửa bằng mật mã Morse

Đây là một trong những tính năng đặc sắc nhất của hệ thống.

### 3.1. Nhập mật mã bằng Morse

**Nút 1** dùng để nhập từng ký tự Morse:

- Nhấn ngắn `< 500 ms` → dấu chấm `.`
- Giữ `≥ 500 ms` → dấu gạch `-`

Âm thanh phản hồi:

- Dấu chấm: `1800 Hz / 40 ms`
- Dấu gạch: `1000 Hz / 120 ms`

Mật mã mặc định:

```text
.--.-.
```

- Tối đa 12 ký tự.
- **Nút 2** dùng để xác nhận/gửi mật mã.
- Timeout nhập: **10 giây**, sau đó buffer tự xóa.

### 3.2. Xử lý mật mã đúng/sai

**Đúng:**

- Mở cửa chính tự động.
- Tự đóng sau **5 giây** (`DOOR_HOLD_MS`).
- LCD: `Door OPEN! Welcome home`
- Phát 2 tiếng bíp xác nhận.
- Nếu đang ở Away Mode → tự tắt Away Mode và hủy báo động trộm.
- Đưa bộ đếm nhập sai về `0`.

**Sai:**

- Bíp trầm `300 Hz / 400 ms`.
- LCD: `Wrong password!`
- Ghi sự kiện `Wrong password` vào black box.

### 3.3. Chống dò mật mã — khóa theo cấp số nhân

Sau **3 lần sai liên tiếp** (`MAX_FAILS`) → kích hoạt `LOCKOUT`.

Thời gian khóa tăng gấp đôi:

```text
30s → 60s → 120s → 240s
```

Theo công thức:

```text
LOCKOUT_BASE_MS << lockoutLevel
```

Trong trạng thái lockout:

- Các lần bấm nút chỉ tạo bíp từ chối `300 Hz`.
- Không thể mở cửa.
- Kích hoạt còi báo động trong **5 giây** (`LOCKOUT_SIREN_MS`).

### 3.4. Đổi mật mã

Serial:

```text
P<mã>#
```

Mật mã mới được lưu bền vào EEPROM.

### 3.5. Away Mode — báo trộm

Bật bằng lệnh `H` hoặc qua menu.

Quy trình:

1. Hoãn **10 giây** (`ARM_DELAY_MS`) để chủ nhà rời đi.
2. Hiển thị `Leave now! 10s`.
3. Phát 3 bíp cảnh báo.
4. Hết thời gian → Away Mode hoạt động.
5. PIR phát hiện chuyển động → **INTRUDER ALARM**.

Báo động:

- Kéo dài **30 giây** (`INTRUDER_ALARM_MS`).
- Còi hú.
- Hai LED nháy luân phiên mỗi `250 ms`:
  - trắng sáng / vàng tắt
  - trắng tắt / vàng sáng

Hủy báo động bằng:

- Nhập đúng mật mã.
- Lệnh `R`.

Các sự kiện được ghi log:

```text
Away armed
Away disarmed
INTRUDER
```

---

## 4. Giám sát khí gas MQ-2

Hệ thống xử lý khí gas theo **máy trạng thái 4 mức**, có hysteresis để tránh dao động.

### 4.1. WARMUP

Trong **20 giây đầu** sau khi khởi động (`WARMUP_MS`):

- MQ-2 chưa được đánh giá.
- LCD hiển thị `WARMUP`.
- Các logic phụ thuộc gas tạm dừng:
  - hơi nước
  - chào khách
  - báo trộm

### 4.2. Các mức khí gas

| Trạng thái | Ngưỡng bật | Ngưỡng hạ |
|---|---:|---:|
| OK | — | — |
| WARNING | `gasVal ≥ 450` | `< 400` |
| DANGER | `gasVal ≥ 700` | `< 600` |

### 4.3. WARNING

Khi gas ở mức cảnh báo:

- Bíp nhắc **600 Hz** mỗi **10 giây**.
- Đèn trắng tự bật.
- Quạt chạy mức **170/255**.
- Cửa sổ **không tự đóng** để ưu tiên thông khí.

### 4.4. DANGER

Khi khí gas ở mức nguy hiểm:

- Quạt tăng lên tối đa **255/255**.
- Cửa sổ **mở hoàn toàn** — thoát khí là ưu tiên số 1.
- Hai LED sáng; có thể nhấp nháy đồng bộ nếu `GAS_LED_BLINK=1`.
- Relay/bơm bị ngắt bắt buộc để giảm nguy cơ phát tia lửa.
- Còi báo động liên tục.
- Tăng `gasAlarmCount`.
- Ghi sự kiện vào black box.
- Vô hiệu hóa tính năng chào khách.

### 4.5. Run-on sau khi hết gas

Khi gas trở lại mức OK:

- Quạt vẫn chạy thêm **30 giây** (`FAN_RUNON_MS`).
- Mức quạt: **200**.

Mục đích là xả phần khí còn tồn dư.

### 4.6. Lệnh im lặng

```text
S
```

Tắt tiếng còi gas trong **60 giây** khi đang ở mức cảnh báo.

> Lệnh này **không áp dụng cho mức nguy hiểm**.

### 4.7. Hiệu chuẩn

Hệ thống có hướng dẫn hiệu chuẩn lại ngưỡng bằng lệnh:

```text
i
```

Thực hiện trong không khí sạch.

---

## 5. Chiếu sáng thông minh

### 5.1. Phát hiện trời tối

Cảm biến ánh sáng sử dụng hysteresis:

- Tối khi `< 300`
- Hết tối khi `> 380`

Khi đèn đang bật, ánh sáng từ LED có thể phản hồi ngược vào cảm biến. Vì vậy ngưỡng hết tối được cộng thêm:

```text
LIGHT_LED_COMP = 100
```

để tránh đèn bật/tắt liên tục.

### 5.2. Đèn trắng — D13

Tự bật khi:

- Trời tối và `DARK_ALWAYS_ON=1`, hoặc
- Gas ở mức WARNING.

Điều khiển thủ công:

```text
a / b
L
```

Ngoài ra có thể điều khiển từ menu.

> Từ phiên bản `v3.3`, PIR **không còn trực tiếp bật đèn trắng**.

### 5.3. Đèn vàng — D5 PWM

Khi trời tối:

| Điều kiện | PWM |
|---|---:|
| Có người trong 15 giây gần đây | `255` |
| Tối nhưng không có người | `180` |
| Chưa đủ tối | `0` |
| Chế độ đèn ngủ | `6` |

Nếu `DARK_ALWAYS_ON=0`, độ sáng khi có người được nội suy tuyến tính theo độ tối môi trường (`220 → 80`).

Hiệu ứng fade:

- Thay đổi PWM **4 đơn vị mỗi 6 ms**.
- Đèn tăng/giảm mượt, không bật/tắt đột ngột.

### 5.4. Điều khiển thủ công

```text
p / q     # bật/tắt
Y         # đảo trạng thái
v128#     # đặt PWM cụ thể
```

Giá trị PWM:

```text
0 → 255
```

Qua menu:

```text
64 → 128 → 255 → 0
```

Giữ lâu → tắt.

---

## 6. Hệ thống quạt

### 6.1. Hệ thống ưu tiên 5 cấp

Từ ưu tiên thấp → cao:

| Ưu tiên | Nguyên nhân | Mức quạt | `fanWhy` |
|---:|---|---:|---|
| 1 | Gas run-on | `200` | `R` |
| 2 | Hơi nước nhiều | `150` | `H` |
| 3 | Gas WARNING | `170` | `W` |
| 4 | Thủ công | `0–255` | `M` |
| 5 | Gas DANGER | `255` | `D` |

Quạt luôn phản ánh **tình huống nghiêm trọng nhất**.

### 6.2. Soft ramp

Tốc độ thay đổi tối đa:

```text
12 PWM / 20 ms
```

Nhờ đó quạt tăng/giảm tốc từ từ và giảm giật dòng điện.

### 6.3. Phanh chủ động

Khi quạt về `0`:

```text
INA = HIGH
INB = HIGH
```

Driver L9110 dừng quạt ngay lập tức thay vì để quạt quay theo quán tính.

### 6.4. Điều khiển thủ công

```text
r / s       # bật max / tắt
w<0-255>#   # đặt tốc độ
+ / -       # tăng / giảm
F           # đảo chiều
```

Qua menu:

```text
85 → 170 → 255 → 0
```

---

## 7. Cửa sổ tự động

### 7.1. Logic ưu tiên

Cửa sổ tự đóng khi:

- Có mưa, hoặc
- Hơi nước/ẩm cao.

Điều kiện: không có gas WARNING/DANGER.

Các tình huống chính:

- **Mưa** → đóng cửa sổ.
- **Hơi nước/ẩm cao `≥ 500`** → đóng cửa + chạy quạt, kể cả khi đang mưa.
- **Gas DANGER** → mở hoàn toàn cửa sổ, ưu tiên tuyệt đối.
- **Hết hơi nước `< 450`** → mở lại cửa sổ và tắt quạt.

### 7.2. Kỹ thuật servo

Servo di chuyển:

```text
2° mỗi 20 ms
```

Lợi ích:

- Quay mượt.
- Giảm dòng đỉnh.
- Giảm nguy cơ sụt áp làm LCD tối.

Nếu nguồn vẫn sụt khi servo quay, tài liệu gốc khuyến cáo cấp nguồn ngoài:

```text
5V / ≥ 2A
```

Servo tự detach sau:

```text
700 ms
```

khi đứng yên (`SERVO_DETACH_IDLE_MS`) để giảm nóng.

Góc đóng cửa sổ:

```text
175°
```

thay vì `180°`, tránh ép servo vào điểm chết cơ khí.

### 7.3. Hoán đổi chân servo

Lệnh:

```text
M
```

Hoán đổi chân servo cửa chính ↔ cửa sổ.

Cấu hình được lưu EEPROM.

### 7.4. Kiểm tra dây servo

Lệnh:

```text
C
```

Tự động:

1. Mở/đóng cửa chính.
2. Đổi trạng thái cửa sổ.
3. Trả hệ thống về trạng thái phù hợp.

Mục đích: xác nhận dây servo được cắm đúng chân.

---

## 8. Cửa chính

Các chức năng:

- Mở bằng mật mã đúng.
- Mở bằng lệnh `l`.
- Điều khiển qua menu.
- Mật mã đúng → tự đóng sau **5 giây**.
- Mở thủ công → giữ **60 giây** rồi tự đóng (`DOOR_MANUAL_HOLD_MS`).
- Điều khiển góc trực tiếp:

```text
t<góc>#
```

- Đảo trạng thái:

```text
D
```

- Đếm số lần mở cửa.
- Ghi log mỗi lần mở.

---

## 9. Hơi nước, mưa và tưới cây

### 9.1. Phát hiện mưa

Cảm biến mưa ở **A3**:

- Ngưỡng bật: `800`
- Ngưỡng tắt: `400`

Có hysteresis và yêu cầu giá trị duy trì trạng thái sạch trong:

```text
8 giây
```

(`RAIN_CLEAR_DELAY_MS`) trước khi công nhận mưa đã dứt.

### 9.2. Hơi nước nhiều — VAPOR HIGH

Ngưỡng:

```text
≥ 500  → HIGH
< 450  → hết HIGH
```

Khi HIGH:

- Đóng cửa sổ.
- Quạt chạy mức `150`.
- Hiển thị:

```text
High humidity! Win close + fan
```

- Phát bíp.
- Quạt tiếp tục chạy cho tới khi hết hơi nước.

Điều khiển:

```text
U   # bật/tắt tính năng
Z   # xem ngưỡng
```

### 9.3. Tưới cây tự động

Đất khô:

- Ngưỡng vào: `> 50`
- Ngưỡng ra: `> 10`

Chu kỳ tưới:

```text
Đất khô
   ↓
Bơm 5 giây
   ↓
Nghỉ bắt buộc 60 giây
   ↓
Kiểm tra lại
   ↓
Nếu vẫn khô → tưới tiếp
```

Mục tiêu:

- Tránh úng cây.
- Tránh cháy bơm.

Nếu đất khô mà chưa được tưới:

- Bíp nhắc **2 tiếng mỗi 60 giây**.

An toàn gas:

> Gas DANGER → relay/bơm **bắt buộc ngắt**, bất kể đất đang khô.

Điều khiển relay:

```text
c / d
```

hoặc menu.

Hệ thống cũng thống kê tổng số lần bơm chạy.

---

## 10. Chào khách tự động

PIR phát hiện người đến gần cửa →:

- Chuông ding-dong gồm 2 nốt `E5-C5`.
- LCD:

```text
Welcome! Someone is near
```

- Hiển thị trong **3 giây**.

### Chống chào liên tục

Phải yên ắng đủ **30 giây** (`WELCOME_COODOWN_MS`) mới chào lại.

### Không chào trong các trường hợp

- Gas đang WARMUP.
- Gas DANGER.
- Away Mode đang bật.
- Đang báo động.
- Hệ thống đang LOCKOUT.

Số lượt khách được lưu ở:

```text
visitorCount
```

Bật/tắt:

```text
V
```

hoặc qua menu.

### PIR latch

Tín hiệu PIR được giữ HIGH thêm **1,5 giây** sau khi cảm biến về LOW để tránh bỏ sót người đi nhanh.

---

## 11. Máy phát nhạc

Có **11 bài/âm thanh** được lưu trong Flash bằng `PROGMEM` để tiết kiệm RAM của Arduino UNO:

1. Happy Birthday
2. Ode to Joy — Beethoven
3. Twinkle Twinkle Little Star
4. Jingle Bells
5. Für Elise — Beethoven
6. Mary Had a Little Lamb
7. Frère Jacques
8. Silent Night
9. We Wish You a Merry Christmas
10. HUST Fanfare
11. Chuông cửa ding-dong

### Kiểm tra khi biên dịch

Dự án dùng `static_assert` để kiểm tra:

> Số nốt phải bằng số thời lượng tương ứng; sai lệch sẽ khiến chương trình không build được.

### Điều khiển

```text
e        # Happy Birthday
f        # Ode to Joy
K<1-11># # phát bài cụ thể
N        # bài kế
B        # bài trước
g        # dừng
G        # playlist liên tục
<        # chậm hơn
>        # nhanh hơn
```

Tốc độ mặc định:

```text
100%
```

Phát nhạc là **non-blocking**, nên khi nhạc chạy:

- cảm biến vẫn hoạt động;
- báo động vẫn chạy;
- các chức năng nền vẫn tiếp tục.

---

## 12. Hẹn giờ

Đặt hẹn giờ:

```text
A<phút>#
```

Ví dụ:

```text
A10#
```

→ hẹn 10 phút.

Hủy:

```text
A0#
```

### Cộng dồn thời gian

Nếu đang có 10 phút và gửi:

```text
A5#
```

→ tổng thời gian trở thành **15 phút**.

Giới hạn:

```text
Tối đa 99 phút
```

Khi hết giờ:

- LCD: `TIME'S UP!` trong 6 giây.
- Phát `HUST Fanfare`.
- 3 bíp lớn.
- Ghi log.

Qua menu:

- Mỗi lần chọn mục `TIMER` → cộng 1 phút.
- Giữ lâu → hủy.

---

## 13. Giao diện người dùng

### 13.1. LCD 16x2

Màn hình có:

- Logo **HUST** bằng 2 bánh răng tự tạo character.
- Hoạt ảnh 2 khung hình bánh răng quay.
- Ký tự nốt nhạc.
- **7 trang thông tin**, tự chuyển mỗi 3 giây.

Các trang bao phủ:

- trạng thái thiết bị;
- cảm biến;
- an ninh;
- nhạc;
- timer;
- thống kê;
- đèn/quạt.

### Overlay thông báo

Các cảnh báo quan trọng như:

```text
Gas DANGER
INTRUDER
```

có thể đè lên trang hiện tại trong một khoảng thời gian, sau đó LCD trở về bình thường.

### LCD cache

Biến:

```text
lcdCache
```

giúp chỉ vẽ lại dòng thay đổi thực sự, từ đó giảm:

- flicker;
- số lần ghi LCD;
- nhiễu hình ảnh.

### Tiết kiệm điện

Đèn nền LCD tự tắt sau:

```text
60 giây
```

không có thao tác.

---

### 13.2. Menu 2 nút — không cần máy tính

#### Vào menu

Giữ đồng thời 2 nút:

```text
0,8 → 3 giây
```

sau đó nhả.

#### Nút 1

- Nhấn ngắn → mục kế tiếp.
- Giữ `1,5 s` → thoát menu.
- Giữ `4 s` → tắt nhạc.

#### Nút 2

- Nhấn ngắn → hành động chính.
- Giữ lâu → hành động phụ, thường là tắt/xóa.

### 13 mục menu

```text
FAN
LED WHITE
LED YELLOW
RELAY
WINDOW
DOOR
MUSIC
TIMER
QUIET
WELCOME
SERVO SWAP
AUTO MODE
STOP ALL
```

Menu tự thoát sau:

```text
20 giây
```

không thao tác.

Mỗi thao tác có bíp xác nhận với tần số riêng.

### Chống reset nhầm / chống phá hoại

- Giữ 2 nút đủ 3 giây → phát bíp cảnh báo trước.
- Nếu đang:
  - Away Mode
  - LOCKOUT
  - INTRUDER ALARM

  thì combo nút sẽ bị từ chối.

Mục tiêu là ngăn việc phá hệ thống chỉ bằng thao tác giữ hai nút vật lý.

### Debounce

Nút bấm được lọc bằng debounce phần mềm:

```text
30 ms
```

Hệ thống phân biệt:

- nhấn;
- nhả;
- thời gian giữ.

---

## 14. Giao thức Serial

Tốc độ:

```text
9600 baud
```

Hệ thống hỗ trợ **hơn 40 lệnh** để điều khiển và theo dõi ngôi nhà từ máy tính.

### 14.1. Thiết bị

| Chức năng | Lệnh |
|---|---|
| LED trắng | `a / b` |
| LED vàng | `p / q` |
| LED vàng PWM | `v<0-255>#` |
| Relay | `c / d` |
| Quạt | `r / s` |
| Quạt PWM | `w<0-255>#` |
| Tăng/giảm quạt | `+ / -` |
| Đảo chiều quạt | `F` |

### 14.2. Cửa

| Chức năng | Lệnh |
|---|---|
| Cửa chính | `l / m` |
| Đảo cửa chính | `D` |
| Góc cửa chính | `t<góc>#` |
| Cửa sổ | `n / o` |
| Chế độ cửa sổ | `W` |
| Góc cửa sổ | `u<góc>#` |

### 14.3. Nhạc

```text
e
f
K<1-11>#
N
B
g
G
<
>
```

### 14.4. Hẹn giờ

```text
A<phút>#
```

### 14.5. Cảm biến

```text
h   # ánh sáng
i   # gas
j   # đất
k   # hơi nước
```

### 14.6. Báo cáo

```text
y   # trạng thái đầy đủ
z   # JSON
T   # telemetry mỗi 2 giây
E   # black box
```

### 14.7. An ninh

```text
S       # quiet gas
R       # reset khóa/báo động
H       # Away Mode
P<mã>#  # đổi mật mã
V       # bật/tắt welcome
```

### 14.8. Hệ thống

```text
X       # về AUTO, xóa override
x       # STOP ALL
C       # test servo
M       # đổi chân servo
U       # quạt theo hơi nước
Z       # xem ngưỡng
?       # trợ giúp
```

### Phân tích lệnh

Các lệnh có tham số dùng ký tự kết thúc:

```text
#
```

Ví dụ:

```text
v128#
w200#
t90#
A10#
P.--.-.#
```

Bộ phân tích lệnh hoạt động **non-blocking**, không khóa vòng lặp chính.

---

## 15. Override và chế độ tự động

Mọi thiết bị chính đều có thể bị ghi đè thủ công:

- LED trắng
- LED vàng
- quạt
- cửa sổ
- relay

Khi override tồn tại:

> Chế độ tự động nhường quyền điều khiển.

Để quay lại tự động hoàn toàn:

```text
X
```

hoặc vào menu:

```text
AUTO MODE
```

Lệnh này xóa toàn bộ override.

### Luật an toàn

**An toàn luôn có ưu tiên cao nhất.**

Ví dụ:

- Gas DANGER vẫn buộc quạt lên tối đa.
- Cửa sổ vẫn phải mở.
- Đèn vẫn phải sáng.
- Relay vẫn phải ngắt.

Không được phép để override thủ công làm mất các hành vi an toàn.

Hệ thống cũng có cơ chế hết hạn override (mặc định tắt) và cửa sổ được trả về tự động sau test.

---

## 16. Black Box — nhật ký sự kiện

Black box là bộ đệm vòng lưu **10 sự kiện gần nhất**, kèm timestamp theo giây.

Các sự kiện có thể được ghi:

- Gas WARNING
- Gas DANGER
- Gas hết
- Mưa bắt đầu/dứt
- Đất khô/ổn
- Mở cửa
- Wrong password
- LOCKOUT
- INTRUDER
- Away armed
- Away disarmed
- Reset hệ thống
- Hết giờ timer
- Bơm chạy
- Khách đến
- Hơi nước cao

Xem log bằng:

```text
E
```

Ví dụ định dạng:

```text
[123s] Gas DANGER
```

---

## 17. Thống kê và telemetry

Hệ thống theo dõi các bộ đếm:

- số lần báo động gas;
- số lần mở cửa;
- số lần phát hiện trộm;
- số lần bơm chạy;
- tổng số lần nhập sai mật mã;
- số lượt khách.

Các dữ liệu này có thể được sử dụng trong:

```text
y   # báo cáo trạng thái
z   # JSON
T   # telemetry
```

Telemetry tự động có chu kỳ:

```text
2 giây
```

---

## 18. Kỹ thuật xử lý tín hiệu và an toàn

### 18.1. Lọc cảm biến

Sử dụng **EMA (Exponential Moving Average)** với:

```text
alpha = 1/4
```

và số học fixed-point `<< 4`.

Mục tiêu:

- giảm nhiễu ADC;
- không dùng `float`;
- không tốn thêm nhiều RAM.

Chu kỳ đọc:

```text
40 ms
```

### 18.2. Hysteresis

Các hệ thống dùng hysteresis:

| Hệ thống | Ngưỡng |
|---|---|
| Gas WARNING | `450 / 400` |
| Gas DANGER | `700 / 600` |
| Ánh sáng | `300 / 380` + bù LED |
| Mưa | `800 / 400` + trễ 8s |
| Hơi nước | `500 / 450` |
| Đất khô | `50 / 10` |

Mục tiêu là ngăn hiện tượng chớp tắt liên tục khi tín hiệu dao động quanh ngưỡng.

### 18.3. Servo chống sụt áp

- Di chuyển `2° / 20 ms`.
- Tự ngắt xung khi đứng yên.
- Giảm dòng đỉnh.
- Giảm lỗi LCD tối khi servo quay.

### 18.4. PIR latch

Giữ tín hiệu PIR thêm:

```text
1,5 giây
```

để không bỏ sót người di chuyển nhanh.

### 18.5. Debounce nút

```text
30 ms
```

kết hợp phát hiện thời gian giữ.

### 18.6. Non-blocking

Toàn bộ hệ thống chạy nền:

- nhạc;
- beep;
- servo;
- quạt;
- cảm biến;
- cảnh báo.

Không dùng `delay()` dài để khóa toàn bộ chương trình.

### 18.7. Watchdog

Watchdog timer:

```text
Tùy chọn — mặc định tắt
```

### 18.8. An toàn điện

Khi gas DANGER:

```text
Relay/bơm → OFF
```

Mục tiêu là giảm nguy cơ cháy nổ do thiết bị điện.

### 18.9. Chống phá hoại

Menu/reset vật lý bị chặn khi:

- LOCKOUT;
- Away Mode;
- Intruder Alarm.

### 18.10. EEPROM

Lưu bền:

- mật mã;
- cấu hình hoán đổi chân servo.

Các dữ liệu này sống sót qua reset và mất điện.

### 18.11. Tối ưu bộ nhớ

- Nhạc lưu trong `PROGMEM`.
- Chuỗi hiển thị cũng được tối ưu bộ nhớ.
- `static_assert` kiểm tra dữ liệu bài hát khi biên dịch.

---

## 19. Tóm tắt khả năng hệ thống

### Tự động 24/7

Smart Home Pro v6 có thể:

- Phát hiện rò rỉ gas theo 3 mức và phản ứng khẩn cấp:
  - quạt tối đa;
  - mở cửa sổ;
  - bật đèn;
  - còi;
  - ngắt bơm.
- Tự đóng cửa sổ khi mưa hoặc có nhiều hơi nước.
- Tự tưới cây theo chu kỳ khô/ướt an toàn.
- Tự bật/tắt và điều chỉnh độ sáng theo ánh sáng môi trường + sự hiện diện của người.
- Tự chào khách bằng chuông.
- Phát hiện trộm khi chủ nhà vắng.
- Tự đóng cửa sau khi mở.

### Thủ công

Người dùng có thể:

- Khóa/mở cửa bằng mật mã Morse có chống dò vét.
- Điều khiển hệ thống bằng menu **13 chức năng** chỉ với 2 nút.
- Điều khiển từ máy tính bằng **40+ lệnh Serial**.
- Nhận dữ liệu JSON và telemetry.
- Phát **11 bài/âm thanh** với điều khiển tempo và playlist.
- Đặt timer.
- Xem báo cáo, thống kê và black box.
- Hiệu chuẩn và kiểm tra phần cứng.

---

## Ghi chú

README này được biên soạn trực tiếp từ tài liệu mô tả **Smart Home Pro v6 — Nhóm 14**. Các ngưỡng, thời gian, tên trạng thái, tên lệnh và mô tả hành vi được giữ theo nội dung nguồn.
