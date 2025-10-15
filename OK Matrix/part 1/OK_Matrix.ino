#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Arduino.h>

#define ROWS 8
#define COLS 8

volatile uint8_t fb[ROWS];

// ===================================================
// 8×8 patterns for O and K
// ===================================================
const uint8_t LETTER_O_8x8[ROWS][COLS] = {
  {0,1,1,1,1,1,1,0},
  {1,1,1,0,0,1,1,1},
  {1,1,0,0,0,0,1,1},
  {1,1,0,0,0,0,1,1},
  {1,1,0,0,0,0,1,1},
  {1,1,0,0,0,0,1,1},
  {1,1,1,0,0,1,1,1},
  {0,1,1,1,1,1,1,0}
};

const uint8_t LETTER_K_8x8[ROWS][COLS] = {
  {1,1,0,0,0,1,1,0},
  {1,1,0,0,1,1,0,0},
  {1,1,0,1,1,0,0,0},
  {1,1,1,1,0,0,0,0},
  {1,1,1,1,0,0,0,0},
  {1,1,0,1,1,0,0,0},
  {1,1,0,0,1,1,0,0},
  {1,1,0,0,0,1,1,0}
};

// Invert helper
void invert_pattern(const uint8_t src[ROWS][COLS], uint8_t dest[ROWS][COLS]) {
  for (uint8_t r = 0; r < ROWS; r++)
    for (uint8_t c = 0; c < COLS; c++)
      dest[r][c] = src[r][c] ? 0 : 1;
}

// ===================================================
// 8×8 font for scrolling (A–Z + space)
// ===================================================
const uint8_t FONT[27][ROWS] = {
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
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}  // SPACE
};

uint8_t char_index(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  return 26;
}

// ===================================================
// Matrix control and ISR
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
  row++;
  if (row >= ROWS) row = 0;
  uint8_t mask = fb[row];
  cols_write_mask(mask);
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
    for (uint8_t c = 0; c < COLS; c++) {
      if (pat[r][c]) m |= (1 << c);
    }
    fb[r] = m;
  }
  SREG = sreg;
}

// ===================================================
// Scroll using the 8×8 FONT
// ===================================================
void scroll_text(const char *msg, uint16_t speed) {
  uint8_t len = strlen(msg);
  uint16_t total_cols = len * 9; // 8 per letter + 1 space
  for (uint16_t offset = 0; offset < total_cols; offset++) {
    uint8_t window[ROWS][COLS] = {0};
    for (uint8_t r = 0; r < ROWS; r++) {
      for (uint8_t c = 0; c < COLS; c++) {
        int col_index = offset + c;
        int char_i = col_index / 9;
        int within = col_index % 9;
        if (char_i < len && within < 8) {
          uint8_t bits = FONT[char_index(toupper(msg[char_i]))][r];
          window[r][c] = (bits >> (7 - within)) & 1;
        }
      }
    }
    write_pattern_to_fb(window);
    _delay_ms(speed);
  }
}

// ===================================================
// Main sequence
// ===================================================
void setup() {
  setup_pins();
  setup_timer2_scan();
}

void loop() {
  uint8_t invO[ROWS][COLS];
  uint8_t invK[ROWS][COLS];
  invert_pattern(LETTER_O_8x8, invO);
  invert_pattern(LETTER_K_8x8, invK);

  // Step 1: Show O, K, and inverses
  write_pattern_to_fb(LETTER_O_8x8); _delay_ms(1000);
  write_pattern_to_fb(LETTER_K_8x8); _delay_ms(1000);
  write_pattern_to_fb(invO); _delay_ms(1000);
  write_pattern_to_fb(invK); _delay_ms(1000);

  // Step 2: Scroll text using bold 8×8 font
  scroll_text(" THROUGH THE WIRES", 80);
}
