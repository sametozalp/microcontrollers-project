int person = 0;

void setup() {
  Serial.begin(9600);

  // LED PIN
  for (int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
  }

}

void loop() {
  person = random(14);
  Serial.println(person);

  if (person != 0) {
    for (int i = 2; i < person + 2; i++) {
      digitalWrite(i, HIGH);
    }
  }

  delay(1000);

  close_leds();
}

void close_leds() {
  for (int i = 2; i < 14; i++) {
    digitalWrite(i, LOW);
  }
}
