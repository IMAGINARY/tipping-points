#include <Arduino.h>

// arduino cnc shield

int EN_PIN = 8; // für alle motoren  //8 cnc

// für die x achse:
int STEP_PIN_1 = 2; // 2 cnc
int DIR_PIN_1 = 5;  // 5 cnc
int BUTTON_PIN_1 = 9;
int BUTTON_PIN_2 = 10;
int BUTTON_PIN_3 = A1;
int buttonState_1;
int buttonState_2;
int buttonState_3;
int analogInPin = A0;
int schritt = 0;
int fahren;
int zaehler = 0;
int motor_speed;
int genullt = 0;

int motordelay;
int sensorValue;
int outputValue;
int rampe = 10000;

void nullen() {
  Serial.println("motor nullfahren");
  motordelay = 5000;
  digitalWrite(
      DIR_PIN_1,
      HIGH); // set the direction: links, erstmal gucken, ob magnet erreicht
  buttonState_1 = digitalRead(BUTTON_PIN_1);
  if (buttonState_1 == LOW) {
    Serial.print("rausfahren");
    while (buttonState_1 == LOW) {
      buttonState_1 = digitalRead(BUTTON_PIN_1);
      digitalWrite(STEP_PIN_1, LOW);
      delayMicroseconds(motordelay);
      digitalWrite(STEP_PIN_1, HIGH);
      delayMicroseconds(motordelay);
    }
    Serial.println(" aus magnet");
    for (int i = 0; i <= 80; i++) {
      digitalWrite(STEP_PIN_1, LOW);
      delayMicroseconds(motordelay);
      digitalWrite(STEP_PIN_1, HIGH);
      delayMicroseconds(motordelay);
    }
    delay(500);
  }
  digitalWrite(DIR_PIN_1, LOW); // set the direction: rechts bis magnet
  buttonState_1 = digitalRead(BUTTON_PIN_1);
  while (buttonState_1 == HIGH) {
    buttonState_1 = digitalRead(BUTTON_PIN_1);
    digitalWrite(STEP_PIN_1, LOW);
    delayMicroseconds(motordelay);
    digitalWrite(STEP_PIN_1, HIGH);
    delayMicroseconds(motordelay);
  }

  digitalWrite(DIR_PIN_1, HIGH);  // set the direction: rechts bis magnet
  Serial.println("mitte magnet"); // und noch ein bisschen zurück
  for (int i = 0; i <= 200; i++) {
    digitalWrite(STEP_PIN_1, LOW);
    delayMicroseconds(motordelay);
    digitalWrite(STEP_PIN_1, HIGH);
    delayMicroseconds(motordelay);
  }
  Serial.println("nullposition erreicht");
  delay(500);
  genullt = 1;

  digitalWrite(DIR_PIN_1, LOW); // set the direction: rechts
}

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(analogInPin, INPUT);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(EN_PIN, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);

  digitalWrite(EN_PIN, LOW);    // activate driver LOW ist an
  digitalWrite(DIR_PIN_1, LOW); // set the direction: low

  delay(100);

  Serial.println(
      "start ------------------------------------------------------------");

  nullen();

  sensorValue = analogRead(analogInPin);
  outputValue = map(sensorValue, 0, 1023, 15000, 1000);
  motordelay = outputValue;
  Serial.println("                    start");
  Serial.println(motordelay);

  motor_speed = rampe;
  Serial.println(rampe);
}

void loop() {

  fahren = 0;
  buttonState_2 = digitalRead(BUTTON_PIN_2);
  if (buttonState_2 == LOW) {
    zaehler = 0;
    digitalWrite(DIR_PIN_1, LOW);
    fahren = 1;
    Serial.println("2");
  }
  buttonState_3 = digitalRead(BUTTON_PIN_3);
  if (buttonState_3 == LOW) {
    zaehler = 0;
    digitalWrite(DIR_PIN_1, HIGH);
    fahren = 1;
    Serial.println("3");
  }

  if (fahren == 0) {
    motor_speed = rampe;
    zaehler++;
    delay(10);
  }

  if (zaehler > 100 * 60 * 5) { // *10 = 10sekunden, 100*60*5
    if (genullt == 0) {
      Serial.println("nullen");
      nullen();
      zaehler = 0;
    }
  }

  if (fahren) {
    genullt = 0;
    if (motor_speed > motordelay) {
      motor_speed = motor_speed - 50;
    }

    // Serial.println(motor_speed);
    schritt = !schritt;
    digitalWrite(STEP_PIN_1, schritt);
    delayMicroseconds(motor_speed);
  }
}
