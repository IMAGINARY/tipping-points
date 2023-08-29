#include <Arduino.h>
#include <AccelStepper.h>

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
long ziel;

int motordelay;
int sensorValue;
int outputValue;
int rampe = 10000;

AccelStepper stepper(1, STEP_PIN_1, DIR_PIN_1);

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
  stepper.setCurrentPosition(0);
  delay(50);
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

  delay(500);

  Serial.println(">>>>>>>>>>>>>>>>>>> start");
  sensorValue = analogRead(analogInPin);
  outputValue = map(sensorValue, 0, 1023, 30, 500);
  motor_speed = outputValue;
  Serial.print("motor_speed: ");
  Serial.println(motor_speed);

  nullen();

  stepper.setMaxSpeed(
      motor_speed); // für 1/4 step, höher = schneller, 2000 mx ?
  stepper.setSpeed(motor_speed);
  stepper.setAcceleration(motor_speed); // höher = steiler
}

void loop() {

  fahren = 0;
  buttonState_2 = digitalRead(BUTTON_PIN_2);
  if (buttonState_2 == LOW) {
    zaehler = 0;
    ziel = -64000;
    stepper.moveTo(ziel);
    fahren = 1;
  }
  buttonState_3 = digitalRead(BUTTON_PIN_3);
  if (buttonState_3 == LOW) {
    zaehler = 0;
    ziel = 64000;
    stepper.moveTo(ziel);
    fahren = 1;
  }

  if (fahren == 0) {
    stepper.stop();          // Stop as fast as possible: sets new target
    stepper.runToPosition(); // Now stopped after quickstop
    zaehler++;
    delay(10);
  }

  if (fahren) {
    stepper.run();
    genullt = 0;
  }

  if (zaehler > 100 * 60 * 5) { // *10 = 10 minuten, 100*60*5
    if (genullt == 0) {
      Serial.println("nullen");
      nullen();
      zaehler = 0;
    }
  }
}
