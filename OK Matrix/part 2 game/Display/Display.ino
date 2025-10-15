// #define F_CPU 16000000UL
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
// #include <Arduino.h>
// #include <string.h>
// #include <ctype.h>

// #define ROWS 8
// #define COLS 8
// #define BTN_X A4   // Replay current level
// #define SIZE 8
// #define MAX_PATH 32

// volatile uint8_t fb[ROWS];   // LED framebuffer

// // ===================================================
// // ----- Patterns -----
// // ===================================================
// const uint8_t BOX[ROWS][COLS] = {
//   {1,1,1,1,1,1,1,1},
//   {1,0,0,0,0,0,0,1},
//   {1,0,0,0,0,0,0,1},
//   {1,0,0,0,0,0,0,1},
//   {1,0,0,0,0,0,0,1},
//   {1,0,0,0,0,0,0,1},
//   {1,0,0,0,0,0,0,1},
//   {1,1,1,1,1,1,1,1}
// };

// const uint8_t X_PATTERN[ROWS][COLS] = {
//   {1,0,0,0,0,0,0,1},
//   {0,1,0,0,0,0,1,0},
//   {0,0,1,0,0,1,0,0},
//   {0,0,0,1,1,0,0,0},
//   {0,0,0,1,1,0,0,0},
//   {0,0,1,0,0,1,0,0},
//   {0,1,0,0,0,0,1,0},
//   {1,0,0,0,0,0,0,1}
// };

// // ===================================================
// // ----- Font (A–Z, 0–9, space) -----
// // ===================================================
// const uint8_t FONT[37][ROWS] = {
//   // A–Z (26)
//   {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00}, // A
//   {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}, // B
//   {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00}, // C
//   {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00}, // D
//   {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00}, // E
//   {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00}, // F
//   {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00}, // G
//   {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, // H
//   {0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00}, // I
//   {0x1E,0x0C,0x0C,0x0C,0x6C,0x6C,0x38,0x00}, // J
//   {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00}, // K
//   {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00}, // L
//   {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00}, // M
//   {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00}, // N
//   {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // O
//   {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00}, // P
//   {0x3C,0x66,0x66,0x66,0x66,0x6C,0x36,0x00}, // Q
//   {0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00}, // R
//   {0x3C,0x66,0x30,0x18,0x0C,0x66,0x3C,0x00}, // S
//   {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // T
//   {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // U
//   {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, // V
//   {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // W
//   {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00}, // X
//   {0x66,0x66,0x3C,0x18,0x18,0x18,0x18,0x00}, // Y
//   {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00}, // Z
//   // Digits 0–9
//   {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00}, // 0
//   {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, // 1
//   {0x3C,0x66,0x06,0x0C,0x30,0x60,0x7E,0x00}, // 2
//   {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}, // 3
//   {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00}, // 4
//   {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}, // 5
//   {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00}, // 6
//   {0x7E,0x06,0x0C,0x18,0x18,0x18,0x18,0x00}, // 7
//   {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, // 8
//   {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00}, // 9
//   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  // SPACE
// };

// uint8_t char_index(char c) {
//   if (c >= 'A' && c <= 'Z') return c - 'A';      // A–Z
//   if (c >= '0' && c <= '9') return 26 + (c - '0'); // 0–9
//   return 36; // space
// }

// // ===================================================
// // ----- LED Matrix driver -----
// // ===================================================
// static inline void rows_all_inactive() {
//   PORTD |= 0b11111100;
//   PORTB |= 0b00000011;
// }
// static inline void cols_write_mask(uint8_t m) {
//   PORTB &= ~((1<<PB2)|(1<<PB3)|(1<<PB4));
//   PORTC &= ~((1<<PC5)|(1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3));
//   if (m & (1<<0)) PORTB |= (1<<PB2);
//   if (m & (1<<1)) PORTB |= (1<<PB3);
//   if (m & (1<<2)) PORTB |= (1<<PB4);
//   if (m & (1<<3)) PORTC |= (1<<PC5);
//   if (m & (1<<4)) PORTC |= (1<<PC0);
//   if (m & (1<<5)) PORTC |= (1<<PC1);
//   if (m & (1<<6)) PORTC |= (1<<PC2);
//   if (m & (1<<7)) PORTC |= (1<<PC3);
// }
// ISR(TIMER2_COMPA_vect) {
//   static uint8_t row = 0;
//   if (row < 6) PORTD |= (1 << (row + 2));
//   else         PORTB |= (1 << (row - 6));
//   cols_write_mask(0);
//   row++;
//   if (row >= ROWS) row = 0;
//   cols_write_mask(fb[row]);
//   if (row < 6) PORTD &= ~(1 << (row + 2));
//   else         PORTB &= ~(1 << (row - 6));
// }
// void setup_pins() {
//   DDRD |= 0b11111100;
//   DDRB |= 0b00000011;
//   DDRB |= (1<<PB2)|(1<<PB3)|(1<<PB4);
//   DDRC |= (1<<PC5)|(1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3);
//   rows_all_inactive();
//   cols_write_mask(0);
// }
// void setup_timer2_scan() {
//   TCCR2A = (1 << WGM21);
//   TCCR2B = (1 << CS22);
//   OCR2A  = 31;
//   TIMSK2 = (1 << OCIE2A);
//   sei();
// }
// void write_pattern_to_fb(const uint8_t pat[ROWS][COLS]) {
//   uint8_t sreg = SREG; cli();
//   for (uint8_t r = 0; r < ROWS; r++) {
//     uint8_t m = 0;
//     for (uint8_t c = 0; c < COLS; c++) if (pat[r][c]) m |= (1 << c);
//     fb[r] = m;
//   }
//   SREG = sreg;
// }

// // ===================================================
// // ----- Scroll text -----
// // ===================================================
// void scroll_text(const char *msg, uint16_t speed) {
//   uint8_t len = strlen(msg);
//   uint16_t total_cols = len * 9;
//   for (uint16_t offset = 0; offset < total_cols; offset++) {
//     uint8_t window[ROWS][COLS] = {0};
//     for (uint8_t r = 0; r < ROWS; r++) {
//       for (uint8_t c = 0; c < COLS; c++) {
//         int col_index = offset + c;
//         int char_i = col_index / 9;
//         int within = col_index % 9;
//         if (char_i < len && within < 8) {
//           uint8_t bits = FONT[char_index(toupper((unsigned char)msg[char_i]))][r];
//           window[r][c] = (bits >> (7 - within)) & 1;
//         }
//       }
//     }
//     write_pattern_to_fb(window);
//     _delay_ms(speed);
//   }
//   uint8_t blank[ROWS][COLS] = {0};
//   write_pattern_to_fb(blank);
// }

// // ===================================================
// // ----- Flash helper -----
// // ===================================================
// void flash_pattern(const uint8_t pat[ROWS][COLS], uint8_t times, uint16_t delay_ms) {
//   for (uint8_t i = 0; i < times; i++) {
//     write_pattern_to_fb(pat);
//     _delay_ms(delay_ms);
//     uint8_t blank[ROWS][COLS] = {0};
//     write_pattern_to_fb(blank);
//     _delay_ms(delay_ms);
//   }
// }

// // ===================================================
// // ----- Game Logic -----
// // ===================================================
// uint8_t grid[SIZE][SIZE];
// uint8_t path_r[MAX_PATH];
// uint8_t path_c[MAX_PATH];
// uint8_t path_len = 0;
// uint8_t level = 1;

// void clear_grid() {
//   for (uint8_t r = 0; r < SIZE; r++)
//     for (uint8_t c = 0; c < SIZE; c++)
//       grid[r][c] = 0;
// }

// void generate_path(uint8_t lvl) {
//   clear_grid();
//   const uint8_t easy[][2] = {
//     {7,0},{7,1},{6,1},{5,1},{5,2}
//   };
//   const uint8_t normal[][2] = {
//     {7,0},{7,1},{7,2},{6,2},{6,3},{5,3},{4,3},{4,4},{4,5},{5,5}
//   };
//   const uint8_t hard[][2] = {
//     {7,0},{7,1},{6,1},{6,2},{5,2},{5,3},{4,3},{3,3},{3,4},
//     {3,5},{4,5},{5,5},{6,5},{6,6},{7,6}
//   };

//   const uint8_t (*selected)[2];
//   uint8_t len;

//   if (lvl == 1) { selected = easy; len = sizeof(easy)/2; }
//   else if (lvl == 2) { selected = normal; len = sizeof(normal)/2; }
//   else { selected = hard; len = sizeof(hard)/2; }

//   path_len = len;
//   for (uint8_t i = 0; i < len; i++) {
//     path_r[i] = selected[i][0];
//     path_c[i] = selected[i][1];
//     grid[path_r[i]][path_c[i]] = i + 1;
//   }
// }

// void show_path_sequence() {
//   for (uint8_t i = 0; i < path_len; i++) {
//     uint8_t frame[8] = {0};
//     frame[path_r[i]] = (1 << path_c[i]);
//     uint8_t sreg = SREG; cli();
//     memcpy(fb, frame, 8);
//     SREG = sreg;
//     _delay_ms(500);
//     sreg = SREG; cli();
//     memset(fb, 0, 8);
//     SREG = sreg;
//     _delay_ms(100);
//   }
// }

// bool player_replay() {
//   uint8_t r = SIZE - 1, c = 0;
//   uint8_t expectStep = 2;
//   unsigned long lastInput = millis();

//   while (expectStep <= path_len) {
//     if (digitalRead(BTN_X) == LOW) return false; // restart level

//     if (Serial.available()) {
//       String cmd = Serial.readStringUntil('\n');
//       cmd.trim();
//       if (cmd.equalsIgnoreCase("UP")) r = max(0, (int)r - 1);
//       else if (cmd.equalsIgnoreCase("DOWN")) r = min(SIZE - 1, (int)r + 1);
//       else if (cmd.equalsIgnoreCase("LEFT")) c = max(0, (int)c - 1);
//       else if (cmd.equalsIgnoreCase("RIGHT")) c = min(SIZE - 1, (int)c + 1);

//       uint8_t frame[8] = {0};
//       frame[r] = (1 << c);
//       uint8_t sreg = SREG; cli();
//       memcpy(fb, frame, 8);
//       SREG = sreg;
//       _delay_ms(250);
//       sreg = SREG; cli();
//       memset(fb, 0, 8);
//       SREG = sreg;

//       if (grid[r][c] != expectStep) {
//         flash_pattern(X_PATTERN, 3, 150);
//         return false;
//       }
//       expectStep++;
//       lastInput = millis();
//     }
//     if (millis() - lastInput > 15000) return false;
//   }
//   return true;
// }

// // ===================================================
// // ----- Setup & Loop -----
// // ===================================================
// void setup() {
//   Serial.begin(9600);
//   setup_pins();
//   pinMode(BTN_X, INPUT_PULLUP);
//   setup_timer2_scan();
//   scroll_text("LINE TRACE", 60);
//   delay(1000);
// }

// void loop() {
//   char lvl_msg[12];
//   sprintf(lvl_msg, " LEVEL%d", level);
//   scroll_text(lvl_msg, 60);
//   generate_path(level);
//   show_path_sequence();

//   bool success = player_replay();

//   if (success) {
//     flash_pattern(BOX, 3, 150);
//     if (level < 3) {
//       scroll_text("GOOD", 60);
//       level++;
//       delay(1000);
//     } else {
//       scroll_text("CLEAR", 60);
//       delay(2000);
//       level = 1;
//     }
//   } else {
//     scroll_text("RETRY", 60);
//     delay(1000);
//   }
// }
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Arduino.h>
#include <string.h>
#include <ctype.h>

#define ROWS 8
#define COLS 8
#define BTN_X A4   // Replay current level
#define SIZE 8
#define MAX_PATH 32

volatile uint8_t fb[ROWS];   // LED framebuffer

// ===================================================
// ----- Patterns -----
// ===================================================
const uint8_t BOX[ROWS][COLS] = {
  {1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1}
};

const uint8_t X_PATTERN[ROWS][COLS] = {
  {1,0,0,0,0,0,0,1},
  {0,1,0,0,0,0,1,0},
  {0,0,1,0,0,1,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,1,0,0,1,0,0},
  {0,1,0,0,0,0,1,0},
  {1,0,0,0,0,0,0,1}
};

// ===================================================
// ----- Font (A–Z, 0–9, space) -----
// ===================================================
const uint8_t FONT[37][ROWS] = {
  // A–Z (26)
  {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00}, // A
  {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}, // B
  {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00}, // C
  {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00}, // D
  {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00}, // E
  {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00}, // F
  {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00}, // G
  {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, // H
  {0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00}, // I
  {0x1E,0x0C,0x0C,0x0C,0x6C,0x6C,0x38,0x00}, // J
  {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00}, // K
  {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00}, // L
  {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00}, // M
  {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00}, // N
  {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // O
  {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00}, // P
  {0x3C,0x66,0x66,0x66,0x66,0x6C,0x36,0x00}, // Q
  {0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00}, // R
  {0x3C,0x66,0x30,0x18,0x0C,0x66,0x3C,0x00}, // S
  {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // T
  {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // U
  {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, // V
  {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // W
  {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00}, // X
  {0x66,0x66,0x3C,0x18,0x18,0x18,0x18,0x00}, // Y
  {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00}, // Z
  // Digits 0–9
  {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00}, // 0
  {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, // 1
  {0x3C,0x66,0x06,0x0C,0x30,0x60,0x7E,0x00}, // 2
  {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}, // 3
  {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00}, // 4
  {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}, // 5
  {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00}, // 6
  {0x7E,0x06,0x0C,0x18,0x18,0x18,0x18,0x00}, // 7
  {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, // 8
  {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00}, // 9
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  // SPACE
};

uint8_t char_index(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= '0' && c <= '9') return 26 + (c - '0');
  return 36;
}

// ===================================================
// ----- LED Matrix Driver -----
// ===================================================
static inline void rows_all_inactive() {
  PORTD |= 0b11111100;
  PORTB |= 0b00000011;
}
static inline void cols_write_mask(uint8_t m) {
  PORTB &= ~((1<<PB2)|(1<<PB3)|(1<<PB4));
  PORTC &= ~((1<<PC5)|(1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3));
  if (m & (1<<0)) PORTB |= (1<<PB2);
  if (m & (1<<1)) PORTB |= (1<<PB3);
  if (m & (1<<2)) PORTB |= (1<<PB4);
  if (m & (1<<3)) PORTC |= (1<<PC5);
  if (m & (1<<4)) PORTC |= (1<<PC0);
  if (m & (1<<5)) PORTC |= (1<<PC1);
  if (m & (1<<6)) PORTC |= (1<<PC2);
  if (m & (1<<7)) PORTC |= (1<<PC3);
}
ISR(TIMER2_COMPA_vect) {
  static uint8_t row = 0;
  if (row < 6) PORTD |= (1 << (row + 2));
  else         PORTB |= (1 << (row - 6));
  cols_write_mask(0);
  row++;
  if (row >= ROWS) row = 0;
  cols_write_mask(fb[row]);
  if (row < 6) PORTD &= ~(1 << (row + 2));
  else         PORTB &= ~(1 << (row - 6));
}
void setup_pins() {
  DDRD |= 0b11111100;
  DDRB |= 0b00000011;
  DDRB |= (1<<PB2)|(1<<PB3)|(1<<PB4);
  DDRC |= (1<<PC5)|(1<<PC0)|(1<<PC1)|(1<<PC2)|(1<<PC3);
  rows_all_inactive();
  cols_write_mask(0);
}
void setup_timer2_scan() {
  TCCR2A = (1 << WGM21);
  TCCR2B = (1 << CS22);
  OCR2A  = 31;
  TIMSK2 = (1 << OCIE2A);
  sei();
}
void write_pattern_to_fb(const uint8_t pat[ROWS][COLS]) {
  uint8_t sreg = SREG; cli();
  for (uint8_t r = 0; r < ROWS; r++) {
    uint8_t m = 0;
    for (uint8_t c = 0; c < COLS; c++) if (pat[r][c]) m |= (1 << c);
    fb[r] = m;
  }
  SREG = sreg;
}

// ===================================================
// ----- Scroll Text -----
// ===================================================
void scroll_text(const char *msg, uint16_t speed) {
  uint8_t len = strlen(msg);
  uint16_t total_cols = len * 9;
  for (uint16_t offset = 0; offset < total_cols; offset++) {
    uint8_t window[ROWS][COLS] = {0};
    for (uint8_t r = 0; r < ROWS; r++) {
      for (uint8_t c = 0; c < COLS; c++) {
        int col_index = offset + c;
        int char_i = col_index / 9;
        int within = col_index % 9;
        if (char_i < len && within < 8) {
          uint8_t bits = FONT[char_index(toupper((unsigned char)msg[char_i]))][r];
          window[r][c] = (bits >> (7 - within)) & 1;
        }
      }
    }
    write_pattern_to_fb(window);
    _delay_ms(speed);
  }
  uint8_t blank[ROWS][COLS] = {0};
  write_pattern_to_fb(blank);
}

// ===================================================
// ----- Flash Helper -----
// ===================================================
void flash_pattern(const uint8_t pat[ROWS][COLS], uint8_t times, uint16_t delay_ms) {
  for (uint8_t i = 0; i < times; i++) {
    write_pattern_to_fb(pat);
    _delay_ms(delay_ms);
    uint8_t blank[ROWS][COLS] = {0};
    write_pattern_to_fb(blank);
    _delay_ms(delay_ms);
  }
}

// ===================================================
// ----- Game Logic -----
// ===================================================
uint8_t grid[SIZE][SIZE];
uint8_t path_r[MAX_PATH];
uint8_t path_c[MAX_PATH];
uint8_t path_len = 0;
uint8_t level = 1;
bool firstAttempt = true;

void clear_grid() {
  for (uint8_t r = 0; r < SIZE; r++)
    for (uint8_t c = 0; c < SIZE; c++)
      grid[r][c] = 0;
}

void generate_path(uint8_t lvl) {
  clear_grid();
  const uint8_t easy[][2] = {
    {7,0},{7,1},{6,1},{5,1},{5,2}
  };
  const uint8_t normal[][2] = {
    {7,0},{7,1},{7,2},{6,2},{6,3},{5,3},{4,3},{4,4},{4,5},{5,5}
  };
  const uint8_t hard[][2] = {
    {7,0},{7,1},{6,1},{6,2},{5,2},{5,3},{4,3},{3,3},{3,4},
    {3,5},{4,5},{5,5},{6,5},{6,6},{7,6}
  };

  const uint8_t (*selected)[2];
  uint8_t len;

  if (lvl == 1) { selected = easy; len = sizeof(easy)/2; }
  else if (lvl == 2) { selected = normal; len = sizeof(normal)/2; }
  else { selected = hard; len = sizeof(hard)/2; }

  path_len = len;
  for (uint8_t i = 0; i < len; i++) {
    path_r[i] = selected[i][0];
    path_c[i] = selected[i][1];
    grid[path_r[i]][path_c[i]] = i + 1;
  }
}

void show_path_sequence() {
  for (uint8_t i = 0; i < path_len; i++) {
    uint8_t frame[8] = {0};
    frame[path_r[i]] = (1 << path_c[i]);
    uint8_t sreg = SREG; cli();
    memcpy(fb, frame, 8);
    SREG = sreg;
    _delay_ms(500);
    sreg = SREG; cli();
    memset(fb, 0, 8);
    SREG = sreg;
    _delay_ms(100);
  }
}

bool player_replay() {
  uint8_t r = SIZE - 1, c = 0;
  uint8_t expectStep = 2;
  unsigned long lastInput = millis();

  // 🔹 clear leftover serial input before play
  while (Serial.available()) Serial.read();

  while (expectStep <= path_len) {
    if (digitalRead(BTN_X) == LOW) return false; // restart

    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();
      if (cmd.equalsIgnoreCase("UP")) r = max(0, (int)r - 1);
      else if (cmd.equalsIgnoreCase("DOWN")) r = min(SIZE - 1, (int)r + 1);
      else if (cmd.equalsIgnoreCase("LEFT")) c = max(0, (int)c - 1);
      else if (cmd.equalsIgnoreCase("RIGHT")) c = min(SIZE - 1, (int)c + 1);

      uint8_t frame[8] = {0};
      frame[r] = (1 << c);
      uint8_t sreg = SREG; cli();
      memcpy(fb, frame, 8);
      SREG = sreg;
      _delay_ms(250);
      sreg = SREG; cli();
      memset(fb, 0, 8);
      SREG = sreg;

      if (grid[r][c] != expectStep) {
        flash_pattern(X_PATTERN, 3, 150);
        return false;
      }
      expectStep++;
      lastInput = millis();
    }
    if (millis() - lastInput > 15000) return false;
  }
  return true;
}

// ===================================================
// ----- Setup & Loop -----
// ===================================================
void setup() {
  Serial.begin(9600);
  setup_pins();
  pinMode(BTN_X, INPUT_PULLUP);
  setup_timer2_scan();
  scroll_text("LINE TRACE", 60);
  delay(1000);
}

void loop() {
  // Show level only once per level start
  if (firstAttempt) {
    char lvl_msg[12];
    sprintf(lvl_msg, " LEVEL%d", level);
    scroll_text(lvl_msg, 60);
    firstAttempt = false;
  }

  generate_path(level);
  show_path_sequence();

  bool success = player_replay();

  // clear buffer before next round to avoid input carryover
  while (Serial.available()) Serial.read();

  if (success) {
    flash_pattern(BOX, 3, 150);
    if (level < 3) {
      scroll_text("GOOD", 60);
      level++;
      firstAttempt = true;
      delay(1000);
    } else {
      scroll_text("CLEAR", 60);
      delay(2000);
      level = 1;
      firstAttempt = true;
    }
  } else {
    scroll_text("RETRY", 60);
    delay(1000);
    // keep firstAttempt = false → same level replay, no LEVEL text
  }
}
