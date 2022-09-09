
//figuring out interupts and timers 
#include <avr/sleep.h> //do I need this? 
#include <SoftwareSerial.h>


int LED = 13; //pin of LED being used 
int timerDone = 0; 
const uint16_t t1_load = 0; //counter value
const uint16_t t1_compareVal = 31250; //comparison val got from calulating prescalar of 1024 for timer of 2 sec


void setup() {
  Serial.begin(9600); //send and recive at 9600 BaudRate
  
  pinMode(LED, OUTPUT);//Set the pin to be OUTPUT for LED
  
  TCCR1A = 0; //resetting Timer1 register A to zero

  //setting prescalar to 1024
  TCCR1B |= (1 << CS12); 
  TCCR1B &= ~(1<< CS11); 
  TCCR1B |= (1 << CS10); 

 //Reset Timer1 and set the compare value 
  TCNT1 = t1_load; 
  OCR1A = t1_compareVal; 

  TIMSK1 = (1 << OCIE1A); //enable Timer1 compare interrupt 

  sei(); //enable global interupts

}

//a method to put the arduino to sleep for 8 seconds
void SleepyTimeMode(){
  set_sleep_mode(SLEEP_MODE_IDLE); //set mode to full sleep mode
  sleep_enable(); //enabling sleep mode

  digitalWrite(LED, LOW); 
  delay(1000); //short delay to turn off LED. 

  sleep_mode(); //activate sleep mode
  sleep_disable(); 
  Serial.print("timer Expired \n"); //wake up here
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // Serial.print("test\n"); 
  if (timerDone == 1){
    timerDone = 0; 
    digitalWrite(LED, HIGH); 
    delay (1000); 
    for (int x = 0; x < 30; x++){
      SleepyTimeMode(); 
    }
    
  }
   

   //digitalWrite(LED,HIGH);  //Write new state to LED
   //delay(500); 
   
 
}

//Interupt service for timer1
ISR(TIMER1_COMPA_vect) {
  
  TCNT1 = t1_load; //reset pin to zero
  //PORTB ^= ~(1 << LED); 
 // digitalWrite(LED, HIGH); 

 if(timerDone == 0){
    timerDone = 1; 
 }

}
