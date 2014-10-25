#include <avr/io.h>

// use in vibrateMotors()
//#define DELAY_TIME_VERYHIGH_INTENSITY 1000 
//#define DELAY_TIME_HIGH_INTENSITY 750 
//#define DELAY_TIME_MEDIUM_INTENSITY 500 
//#define DELAY_TIME_LOW_INTENSITY 250 2
#define sensorPin0 A0
#define sensorPin1 A1
#define sensorPin2 A2
#define sensorPin3 A3

int head_ultrasound_State = LOW;   
int head_IR_State = LOW; 
int waist_centre_IR_State = LOW;
int chest_left_State = LOW;
int chest_right_State = LOW;  
 
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
long interval_very_high_intensity = 1000; 
long interval_high_intensity = 750;
long interval_medium_intensity = 500;
long interval_low_intensity = 250;
  
int motor_head_ultrasound[] = {8};

int motor_head_IR[] = {9};
int motor_chestLeft[] = {10};
int motor_chestRight[] = {11};
int motor_waistCentre_IR[] = {12};


//int shoulderLeft, shoulderRight, chestLeft, chestRight, head_ultraound;
//int waistLeft, waistRight, waistCenter, head_IR;
int head_IR, head_ultrasound;
int waistCentre_IR, chestLeft, chestRight;
long duration;

//for ultrasound
//int ultrasoundTrigPin[5]={10,12,14,16,18}; //Put pins to be triggered here

//int ultrasoundTrigPin[1] = {2};
int ultrasoundTrigPin = 2;
//int ultrasoundEchoPin[5]={11,13,15,17,19}; //Put pins to receive trigger here
//int ultrasoundEchoPin[1] = {3}; //Put pins to receive trigger here
int ultrasoundEchoPin = 3;

//for short range IR
//int shortRangeInfraredPin[3]={5,6,7};

//for long range IR
//int longRangeInfraredPin=8;
//int longRangeInfraredPin[4] = {4,5,6,7};


void ultrasoundSensorPoll() //Note that ultrasound sensors read after each other
{
  int ultrasoundPinIndex = 0; //Scoped variable which resets to 0 at every beginning
//  int trigPin = 0;
//  int echoPin = 0;
  //while(ultrasoundPinIndex < 5)
 // trigPin = ultrasoundTrigPin[ultrasoundPinIndex];
 // echoPin = ultrasoundEchoPin[ultrasoundPinIndex];
//  pinMode(trigPin, OUTPUT);
//  pinMode(echoPin, INPUT);
  
  //put into setup
//  pinMode(ultrasoundTrigPin, OUTPUT); 
//  pinMode(ultrasoundEchoPin, INPUT);
  
  
//  analogWrite(trigPin, LOW);
  digitalWrite(ultrasoundTrigPin, LOW);
  delayMicroseconds(2);
  
  //analogWrite(trigPin, HIGH);
  digitalWrite(ultrasoundTrigPin, HIGH);
  delayMicroseconds(5);
  
//  analogWrite(trigPin, LOW);
//  duration = pulseIn(echoPin,HIGH);
  digitalWrite(ultrasoundTrigPin, LOW);
  duration = pulseIn(ultrasoundEchoPin,HIGH);
  
  duration = duration / 29 / 2;
  head_ultrasound = duration;
//  Serial.print("head_ultrasound: ");
//  Serial.print(duration);
//  Serial.print("\n");
  ultrasoundPinIndex++;
//     switch(ultrasoundPinIndex)
//    {
//      case 0:
//        shoulderLeft = duration;
//      case 1:
//        shoulderRight = duration;
//      case 2:
//        chestLeft = duration;
//      case 3:
//        chestRight = duration;
//      case 4:
//        head = duration;
//      default:
//        break;
//    }
    //Need to delay for a while to let echoes die off 
    //Poll ultrasound sensor using the value of ultrasoundPins[ultrasoundPinIndex]
    //Store to shoulderleft/shoulderright etc. depending on value of ultrasoundPinIndex
      //ultrasoundPinIndex++;
   // }
    
 // if(ultrasoundPinIndex = 5)
  if(ultrasoundPinIndex == 1)
    {
      ultrasoundPinIndex = 0;
    }
}

//not used now
//void shortRangeInfraredSensorPoll()
//{
//  int infraredPinIndex = 0;
//  float v = 0;
//  unsigned long currentTotalReading = 0; //This thing is not just currentTotalReading, it's also used to hold the average and the return value.
//  while(infraredPinIndex<3)
//  {
//    for(int i=0;i<10;i++) //Poll ten times
//    {  
//    currentTotalReading+=analogRead(shortRangeInfraredPin[infraredPinIndex]);
//    }
//    currentTotalReading/=(float)10;
//    v=(float)currentTotalReading*5.0/1024.0;
//    currentTotalReading = -34.159*v*v*v*v*v+ 298.19*v*v*v*v -1004.3*v*v*v + 1638.3*v*v -1333.7*v + 497.11;
//    switch(infraredPinIndex)
//    {
//      case 0:
//        waistleft = currentTotalReading;
//      case 1:
//        waistRight = currentTotalReading;
//      case 2:
//        head_ultrasound = currentTotalReading;
//    }
//   //Poll infrared sensor using the values of infraredPins[infraredPinIndex]
//   infraredPinIndex++; 
//  }
//  if(infraredPinIndex=3)
//  {
//    //Reset pins
//    infraredPinIndex = 0;
//  }
//}

// not used now
//void longRangeInfraredSensorPoll()
//{
//  //This thing could be combined, but I'm not doing it because I'm dumb.
//  int infraredPinIndex = 0;
//  float v = 0;
//  unsigned long currentTotalReading = 0;
//  for(int i=0;i<10;i++)
//  {
//    currentTotalReading+=analogRead(longRangeInfraredPin[infraredPinIndex]);
//  }
//  currentTotalReading/=(float)10;
//  v=(float)currentTotalReading*5.0/1024.0;
//  currentTotalReading = -270.79*v*v*v*v*v + 3289.7*v*v*v*v - 15920*v*v*v + 38434*v*v - 46483*v + 22797;
//  waistcenter = currentTotalReading;
//}

void longRangeInfraredSensorPoll()
{
//  pinMode(sensorPin0, INPUT); 
//  pinMode(sensorPin1, INPUT);
//  pinMode(sensorPin2, INPUT);
//  pinMode(sensorPin3, INPUT);
  int infraredPinIndex = 0;
  float v = 0;
  float currentTotalReading = 0; //This thing is not just currentTotalReading, it's also used to hold the average and the return value.
  while (infraredPinIndex < 4)
  {  
    currentTotalReading = 0;
//    for(int i=0;i<10;i++) //Poll ten times
//    {  
    //currentTotalReading+=digitalRead(longRangeInfraredPin[infraredPinIndex]);
    if (infraredPinIndex == 0){
    currentTotalReading=analogRead(sensorPin0);
    }
    
    if (infraredPinIndex == 1){
    currentTotalReading=analogRead(sensorPin1);
    }
    
    if (infraredPinIndex == 2){
    currentTotalReading=analogRead(sensorPin2);
    }
    
    if (infraredPinIndex == 3){
    currentTotalReading=analogRead(sensorPin3);
    }
    
//    }
//    currentTotalReading/=(float)10;
//    v=(float)currentTotalReading*5.0/1024.0;
    v=(float)currentTotalReading*5.0/1024.0;
    //currentTotalReading = -270.79*v*v*v*v*v + 3289.7*v*v*v*v - 15920*v*v*v + 38434*v*v - 46483*v + 22797;
   currentTotalReading = -270.79*pow(v, 5) + 3289.7*pow(v, 4) - 15920*pow(v, 3) + 38434*pow(v, 2) - 46483*v + 22797;
    switch(infraredPinIndex)
    {
      case 0:
        head_IR = currentTotalReading;
        break;
      case 1:
        chestLeft = currentTotalReading;
        break;
      case 2:
        chestRight = currentTotalReading;
        break;
      case 3:
        waistCentre_IR = currentTotalReading;
        break;
    }
    
//  if (infraredPinIndex == 0){
   // Serial.print("head_IR: ");
  Serial.println(currentTotalReading);
    Serial.print("infraredPinIndex: ");
  Serial.print(infraredPinIndex);
  Serial.print("\n");
//  } 
    
    
   //Poll infrared sensor using the values of infraredPins[infraredPinIndex]
   currentTotalReading = 0;
   infraredPinIndex++; 
  }
  if(infraredPinIndex==4)
  {
    //Reset pins
    infraredPinIndex = 0;
  }   
}

// --------------------------------------------------------------------------- Drive


void ultrasound_motordriver(){

   
 if(head_ultrasound < 200){

  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  //digitalWrite(motor_head_ultrasound[0], HIGH);
  digitalWrite(motor_waistCentre_IR[0],LOW);
  
  unsigned long currentMillis = millis();

  if(head_ultrasound < 80){
   // delay(DELAY_TIME_HIGH_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_high_intensity) {
      previousMillis = currentMillis;   

      if (head_ultrasound_State == LOW)
        head_ultrasound_State = HIGH;
      else
        head_ultrasound_State = LOW; 
        
      digitalWrite( motor_head_ultrasound[0], head_ultrasound_State);  
    } 
        
  }
  
  else if(head_ultrasound >= 80 && head_ultrasound <140){
    //delay(DELAY_TIME_MEDIUM_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_medium_intensity) {
      previousMillis = currentMillis;   

      if (head_ultrasound_State == LOW)
        head_ultrasound_State = HIGH;
      else
        head_ultrasound_State = LOW; 
      
      digitalWrite( motor_head_ultrasound[0], head_ultrasound_State);  
    } 
    
  }
  else{ 
    //delay(DELAY_TIME_LOW_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_low_intensity) {
      previousMillis = currentMillis;   

      if (head_ultrasound_State == LOW)
        head_ultrasound_State = HIGH;
      else
        head_ultrasound_State = LOW; 
        
      digitalWrite( motor_head_ultrasound[0], head_ultrasound_State);
    } 
  
  }
 // digitalWrite(motor_head_ultrasound[0], LOW);
 } 

}

//void shortrangeIR_motordriver(){  
// if(head_IR < 140){
//  digitalWrite(motor_shoulderLeft[0], LOW);
//  digitalWrite(motor_shoulderRight[0], LOW); 
//  digitalWrite(motor_chestLeft[0], LOW);
//  digitalWrite(motor_chestRight[0], LOW);
//  digitalWrite(motor_head_IR[0], HIGH);
//  digitalWrite(motor_head_ultrasound[0], LOW);
//  digitalWrite(motor_waistLeft[0],LOW);
//  digitalWrite(motor_waistCentre[0],LOW);
//  digitalWrite(motor_waistRight[0],LOW); 
//    //low intensity
//  if(head_IR < 80)
//    delay(DELAY_TIME_HIGH_INTENSITY);
//  else if(head_IR >= 80 && head_IR <140)
//    delay(DELAY_TIME_MEDIUM_INTENSITY);
//  else 
//    delay(DELAY_TIME_LOW_INTENSITY);
// 
//  digitalWrite(motor_head_IR[0], LOW);
// }
//  
// if(waistLeft < 140){
//  digitalWrite(motor_shoulderLeft[0], LOW);
//  digitalWrite(motor_shoulderRight[0], LOW); 
//  digitalWrite(motor_chestLeft[0], LOW);
//  digitalWrite(motor_chestRight[0], LOW);
//  digitalWrite(motor_head_IR[0], LOW);
//  digitalWrite(motor_head_ultrasound[0], LOW);
//  digitalWrite(motor_waistLeft[0],HIGH);
//  digitalWrite(motor_waistCentre[0],LOW);
//  digitalWrite(motor_waistRight[0],LOW); 
//    //low intensity
//  if(waistLeft < 80)
//    delay(DELAY_TIME_HIGH_INTENSITY);
//  else if(waistLeft >= 80 && waistLeft <140)
//    delay(DELAY_TIME_MEDIUM_INTENSITY);
//  else 
//    delay(DELAY_TIME_LOW_INTENSITY);
// 
//  digitalWrite(motor_waistLeft[0], LOW);
// }
// 
// if(waistRight < 140){
//  digitalWrite(motor_shoulderLeft[0], LOW);
//  digitalWrite(motor_shoulderRight[0], LOW); 
//  digitalWrite(motor_chestLeft[0], LOW);
//  digitalWrite(motor_chestRight[0], LOW);
//  digitalWrite(motor_head_IR[0], LOW);
//  digitalWrite(motor_head_ultrasound[0], LOW);
//  digitalWrite(motor_waistLeft[0],LOW);
//  digitalWrite(motor_waistCentre[0],LOW);
//  digitalWrite(motor_waistRight[0],HIGH); 
//    //low intensity
//  if(waistRight < 80)
//    delay(DELAY_TIME_HIGH_INTENSITY);
//  else if(waistRight >= 80 && waistRight <140)
//    delay(DELAY_TIME_MEDIUM_INTENSITY);
//  else 
//    delay(DELAY_TIME_LOW_INTENSITY);
// 
//  digitalWrite(motor_waistRight[0], LOW);
// }
// 
//}

void longrangeIR_motordriver(){
  
 if(waistCentre_IR < 240){

  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  //digitalWrite(motor_waistCentre_IR[0],HIGH);
    
  unsigned long currentMillis = millis();
 
  if(waistCentre_IR < 80){
    //delay(DELAY_TIME_HIGH_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_high_intensity) {
      previousMillis = currentMillis;   
      
      if (waist_centre_IR_State == LOW)
        waist_centre_IR_State = HIGH;
      else
        waist_centre_IR_State = LOW; 
        
      digitalWrite( motor_waistCentre_IR[0], waist_centre_IR_State);  
    }   
    
  }
  else if(waistCentre_IR >= 80 && waistCentre_IR <160){
   // delay(DELAY_TIME_MEDIUM_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_medium_intensity) {
      previousMillis = currentMillis;   
      
      if (waist_centre_IR_State == LOW)
        waist_centre_IR_State = HIGH;
      else
        waist_centre_IR_State = LOW; 
        
      digitalWrite( motor_waistCentre_IR[0], waist_centre_IR_State);  
    }    
   
   
  }
  else{ 
    //delay(DELAY_TIME_LOW_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_low_intensity) {
      previousMillis = currentMillis;   
      
      if (waist_centre_IR_State == LOW)
        waist_centre_IR_State = HIGH;
      else
        waist_centre_IR_State = LOW; 
        
      digitalWrite( motor_waistCentre_IR[0], waist_centre_IR_State);  
    }   
  }
  //digitalWrite(motor_waistCentre_IR[0], LOW);
 }
 
 
 if(head_IR < 240){
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  //digitalWrite(motor_head_IR[0], HIGH);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistCentre_IR[0],LOW);

  if(head_IR < 80){
    //delay(DELAY_TIME_HIGH_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_high_intensity) {
      previousMillis = currentMillis;   
      
      if (head_IR_State == LOW)
        head_IR_State = HIGH;
      else
        head_IR_State = LOW; 
        
      digitalWrite(motor_head_IR[0], head_IR_State);  
    }   
  }
  else if(head_IR >= 80 && head_IR <160){
    //delay(DELAY_TIME_MEDIUM_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_medium_intensity) {
      previousMillis = currentMillis;   
      
      if (head_IR_State == LOW)
        head_IR_State = HIGH;
      else
        head_IR_State = LOW; 
        
      digitalWrite(motor_head_IR[0], head_IR_State);  
    }   
  }
  else{
   //delay(DELAY_TIME_LOW_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_low_intensity) {
      previousMillis = currentMillis;   
      
      if (head_IR_State == LOW)
        head_IR_State = HIGH;
      else
        head_IR_State = LOW; 
        
      digitalWrite(motor_head_IR[0], head_IR_State);  
    }   
  }
 // digitalWrite(motor_head_IR[0], LOW);
 }
 
 
 if(chestLeft < 240){
  //digitalWrite(motor_chestLeft[0], HIGH);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistCentre_IR[0],LOW);

    //low intensity
  if(chestLeft < 80){
    //delay(DELAY_TIME_HIGH_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_high_intensity) {
      previousMillis = currentMillis;   
      
      if (chest_left_State == LOW)
        chest_left_State = HIGH;
      else
        chest_left_State = LOW; 
        
      digitalWrite(motor_chestLeft[0], chest_left_State);  
    }   
  
  }
  else if(chestLeft >= 80 && chestLeft <160){
    //delay(DELAY_TIME_MEDIUM_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_medium_intensity) {
      previousMillis = currentMillis;   
      
      if (chest_left_State == LOW)
        chest_left_State = HIGH;
      else
        chest_left_State = LOW; 
        
      digitalWrite(motor_chestLeft[0], chest_left_State);  
    }     

}
  else{ 
    //delay(DELAY_TIME_LOW_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_low_intensity) {
      previousMillis = currentMillis;   
      
      if (chest_left_State == LOW)
        chest_left_State = HIGH;
      else
        chest_left_State = LOW; 
        
      digitalWrite(motor_chestLeft[0], chest_left_State);  
    }   
  }
 // digitalWrite(motor_chestLeft[0], LOW);
 } 
 
 

 if(chestRight < 240){
  digitalWrite(motor_chestLeft[0], LOW);
 // digitalWrite(motor_chestRight[0], HIGH);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistCentre_IR[0],LOW);


  if(chestRight < 80){
   // delay(DELAY_TIME_HIGH_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_high_intensity) {
      previousMillis = currentMillis;   
      
      if (chest_right_State == LOW)
        chest_right_State = HIGH;
      else
        chest_right_State = LOW; 
        
      digitalWrite(motor_chestRight[0], chest_right_State);  
    }  
  }
  else if(chestRight >= 80 && chestRight <160){
   // delay(DELAY_TIME_MEDIUM_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_medium_intensity) {
      previousMillis = currentMillis;   
      
      if (chest_left_State == LOW)
        chest_left_State = HIGH;
      else
        chest_left_State = LOW; 
        
      digitalWrite(motor_chestLeft[0], chest_left_State);  
    }  
  }
  else {
    //delay(DELAY_TIME_LOW_INTENSITY);
    currentMillis = millis();
    previousMillis = 0;
    if(currentMillis - previousMillis > interval_low_intensity) {
      previousMillis = currentMillis;   
      
      if (chest_left_State == LOW)
        chest_left_State = HIGH;
      else
        chest_left_State = LOW; 
        
      digitalWrite(motor_chestLeft[0], chest_left_State);  
    }  
  }
  //digitalWrite(motor_chestRight[0], LOW);
 } 
 
}


void vibrateMotors()
{
  //Insert Angela code here.
  ultrasound_motordriver();
//  shortrangeIR_motordriver();
  longrangeIR_motordriver();

}
  

void setup()
{
  //Do setup stuff here.
  Serial.begin(9600);

//setup ultrasound 
  pinMode(ultrasoundTrigPin, OUTPUT); 
  pinMode(ultrasoundEchoPin, INPUT);

//setup long range IR
  pinMode(sensorPin0, INPUT); 
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);

// Setup motors
  int i;
  for(i = 0; i < 1; i++){
    pinMode(motor_head_IR[i], OUTPUT);
    pinMode(motor_head_ultrasound[i], OUTPUT);
    pinMode(motor_chestLeft[i], OUTPUT);
    pinMode(motor_chestRight[i], OUTPUT);  
    pinMode(motor_waistCentre_IR[i], OUTPUT);
  }  
  
}

void loop() //Main time loop runs here
{
  ultrasoundSensorPoll();
//  shortRangeInfraredSensorPoll();
  longRangeInfraredSensorPoll();
  vibrateMotors();
}
