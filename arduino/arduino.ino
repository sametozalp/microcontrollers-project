volatile int person_count = 0;
volatile int old_person_count = 0;
bool durum = false;
//*************************************************************************************************************
void setup() {

  Serial.begin(9600);
  delay(500);
  cli(); // tüm interrupları durdur

  for (int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
  }

  TCCR1A = 0; // NORMAL MOD
  TCCR1B = B00000100; // 4 DE YAZILABİLİR // PRESCALER 256 OLDU
  OCR1A = 31250; // SABİT
  TIMSK1 = B00000010; // COMPARE A MI B Mİ KULLANICAZ SEÇ


  sei(); // tüm interruptları çalıştır
}
//*************************************************************************************************************
void loop() {

  if (Serial.available() > 0) {
    person_count = Serial.parseInt();
  }
  //person_count= random(0,3);
  //Serial.print(person_count);

  if (person_count != old_person_count) {
    turn_off_leds();
  }

  if (person_count != 0) {
    turn_on_leds(person_count);
    old_person_count = person_count;
  }

  digitalWrite(4, durum);
  delay(500);
}
//*************************************************************************************************************
void turn_on_leds(int person_count) {

  for (int i = 2; i < person_count + 2; i++) {
    digitalWrite(i, HIGH);
  }

}
//*************************************************************************************************************
void turn_off_leds() {
  for (int i = 2; i < 14; i++) {
    digitalWrite(i, LOW);
  }
}
//*************************************************************************************************************
ISR(TIMER1_COMPA_vect) {
  // kameradaki insan sayısı çift ise ledi söndür değilse ledi yak (4. pindeki)
  if(person_count % 2 == 0) {
    durum = false;
  } else {
    durum = true;
  }
  TCNT1 = 0; // 0.5 SANİYEDE BİR YAKAR, SAYAÇ SIFIRLAMA
}
