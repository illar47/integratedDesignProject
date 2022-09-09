int analogPin = A2; //analog pin that will be connected to voltage divider with Vin
bool pwmOn = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  TCCR2B = TCCR2B & B11111000 | B00000001; //for PWM frequency of 31372.55 Hz
  pinMode(3, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int volt = analogRead(analogPin);
  float charge= volt * (4.5 / 1023.0);
  if (charge >= 4.5) {
    if (pwmOn) {
      analogWrite(3, 0);
      pwmOn = false;
    }
    Serial.print(charge);
    Serial.println(" is >= 9V");
    //start clock
    //send to sleep every thirty seconds
    //read in temp, change display graphic settings, send updated settings to bluetooth
  }
  else {
    //start clock
    if (!pwmOn) {
      analogWrite(3, 94); //duty cycle * 255 = .37 * 255
      pwmOn = true;
    }
    Serial.print(charge);
    Serial.println(" is < 9V");
   
    //while clock has not run out of time don't read voltage
  }
}
