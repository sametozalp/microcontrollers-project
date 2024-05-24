int person_count = 0;
int old_person_count = 0;
//*************************************************************************************************************
void setup() {

  Serial.begin(9600);
  delay(500);

  
  for (int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
  }
}
//*************************************************************************************************************
void loop() {  
 
  if (Serial.available() > 0) {
    person_count = Serial.parseInt();
  }

  if(person_count != old_person_count) {
    turn_off_leds();
  }

  if (person_count != 0) {
    turn_on_leds(person_count);
    old_person_count = person_count;
  }
  
  delay(500);
}
//*************************************************************************************************************
void turn_on_leds(int &person_count) {
  
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
