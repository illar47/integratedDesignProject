#include <SoftwareSerial.h>
#include <avr/sleep.h>
int analogPin = A2; //analog pin that will be connected to voltage divider with Vin
int ThermistorPin = 0;
SoftwareSerial BSerial(4, 5); //used for bluetooth pin 4 is TXD pin 5 is RXD

int Vo; //output voltage

float R1 = 10000;

float logR2, R2, T;

float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int dc = 0; //duty cycle intial value

//timer values
int timerDone = 0; //determines if timer is done.
const uint16_t t1_load = 0; //counter value
const uint16_t t1_compareVal = 31250; //comparison val got from calulating prescalar of 1024 for timer of 2 sec

void setup() {

  BSerial.begin(9600); //start the transmission of data from bluetooth.

  TCCR2B = TCCR2B & B11111000 | B00000001; //for PWM frequency of 31372.55 Hz
  pinMode(3, OUTPUT); //output pin for PWM

  //timer set up
  TCCR1A = 0; //resetting Timer1 register A to zero

  //setting prescalar to 1024
  TCCR1B |= (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B |= (1 << CS10);

  //Reset Timer1 and set the compare value
  TCNT1 = t1_load;
  OCR1A = t1_compareVal;

  TIMSK1 = (1 << OCIE1A); //enable Timer1 compare interrupt

  sei(); //enable global interupts

  //testing sequences
  Serial.begin(9600);
  Serial.print("testing");
  BSerial.print("testing");
}

//a method to put the arduino to sleep for 2 seconds
void SleepyTimeMode() {
  set_sleep_mode(SLEEP_MODE_IDLE); //set mode to full sleep mode
  sleep_enable(); //enabling sleep mode

  //digitalWrite(LED, LOW);
  //delay(1000); //short delay to turn off LED.

  sleep_mode(); //activate sleep mode
  sleep_disable();


}
void loop() {
  // put your main code here, to run repeatedly:

  //if the timer has expired
  if (timerDone == 1) { //change to 1 to start

    timerDone = 0; //reset the timer

    int volt = analogRead(analogPin); //read in the output voltage
    float charge = volt * (5 / 1023.0); //calculate the current charge

    //the voltage is above the desired threshold
    if (charge >= 4.5) {
      analogWrite(3, 0);
      //read in temp, change display graphic settings, send updated settings to bluetooth
      Vo = analogRead(ThermistorPin);
      R2 = R1 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
      T = T - 273.15;
      T = (T * 9.0) / 5.0 + 32.0;
      BSerial.print("Temperature: ");
      BSerial.print(T);
      BSerial.println(" F");
      delay(500);
    }

    //the voltage is below the desired threshold
    else {
      //until the charge is at the needed threshold, modify the PWM
      while (charge < 4.5) {

        //when the charge is greater than threshold
        // if (charge > 5) {
        //  dc -= 0.01 * 255;
        // analogWrite(3, dc);
        //}

        //when the charge is less than threshold
        //if (dc < 255) {
        //dc = dc + 5; //increase duty cycle by 1%
        // }

        analogWrite(3, 255 * 0.75); //duty cycle * 255

        volt = analogRead(analogPin);
        charge = volt * (5 / 1023.0);
      }
    }


    //start timer and sleep mode for 30 seconds
    for (int x = 0; x < 60; x++) {
      SleepyTimeMode();
    }
    Serial.print("timer Expired \n"); //wake up here
  }
}

//Interupt service for timer1
ISR(TIMER1_COMPA_vect) {

  TCNT1 = t1_load; //reset pin to zero
  //    PORTB ^= ~(1 << LED);
  //digitalWrite(LED, HIGH);

  if (timerDone == 0) {
    timerDone = 1;
  }

}
