#define BTN_W 2
#define BTN_A 3
#define BTN_S 4
#define BTN_D 5

bool lastW = HIGH, lastA = HIGH, lastS = HIGH, lastD = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(BTN_W, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_S, INPUT_PULLUP);
  pinMode(BTN_D, INPUT_PULLUP);
}

void loop() {
  bool nowW = digitalRead(BTN_W);
  bool nowA = digitalRead(BTN_A);
  bool nowS = digitalRead(BTN_S);
  bool nowD = digitalRead(BTN_D);

  if (nowW != lastW) {
    if (nowW == LOW) Serial.println("UP");
    lastW = nowW;
  }
  if (nowA != lastA) {
    if (nowA == LOW) Serial.println("LEFT");
    lastA = nowA;
  }
  if (nowS != lastS) {
    if (nowS == LOW) Serial.println("DOWN");
    lastS = nowS;
  }
  if (nowD != lastD) {
    if (nowD == LOW) Serial.println("RIGHT");
    lastD = nowD;
  }
  delay(5);  // debounce
}
