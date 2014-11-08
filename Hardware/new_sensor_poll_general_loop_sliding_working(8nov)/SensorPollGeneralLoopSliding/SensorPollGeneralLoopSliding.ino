#include <avr/io.h>
#include <elapsedMillis.h>

#define MAX_INTERVAL 1000 //Milliseconds
#define MIN_INTERVAL 100 //Milliseconds

// use in vibrateMotors()
//#define DELAY_TIME_VERYHIGH_INTENSITY 1000 
//#define DELAY_TIME_HIGH_INTENSITY 750 
//#define DELAY_TIME_MEDIUM_INTENSITY 500 
//#define DELAY_TIME_LOW_INTENSITY 250 2
#define sensorPin0 A0 //headIR
#define sensorPin1 A1 //belt IR
#define sensorPin2 A2 //left IR
#define sensorPin3 A3 //right IR
//Ultrasound pins are 2 and 3
int head_ultrasound_State = LOW;   
int head_IR_State = LOW; 
int waist_centre_IR_State = LOW;
int chest_left_State = LOW;
int chest_right_State = LOW;  
 
elapsedMillis timeElapsed;
unsigned long previousHeadIRMillis = 0;
unsigned long previousHeadUSMillis = 0;
unsigned long previousChestLeftMillis = 0;
unsigned long previousChestRightMillis = 0;
unsigned long previousBeltMillis = 0;
unsigned long currentMillis = 0; 
int motor_head_ultrasound[] = {28};
byte shortRangeInfraredPin[] = {A2, A3};
int motor_head_IR[] = {29};
int motor_chestLeft[] = {30};
int motor_chestRight[] = {31};
int motor_waistCentre_IR[] = {32};


//int shoulderLeft, shoulderRight, chestLeft, chestRight, head_ultraound;
//int waistLeft, waistRight, waistCenter, head_IR;
int head_IR, head_ultrasound, waistCentre_IR, chestLeft, chestRight;
long duration_ultrasound;
int ultrasoundTrigPin = 2;
int ultrasoundEchoPin = 3;
//Ultrasound pins are 2 and 3
unsigned int interval_ultrasound = 0.01;
//for ultrasound

//for short range IR
//int shortRangeInfraredPin[3]={5,6,7};

//for long range IR
//int longRangeInfraredPin=8;
//int longRangeInfraredPin[4] = {4,5,6,7};


void ultrasoundSensorPoll() //Note that ultrasound sensors read after each other
{

  int ultrasoundPinIndex = 0; //Scoped variable which resets to 0 at every beginning
  
  digitalWrite(ultrasoundTrigPin, LOW);
 
  timeElapsed = 0;
  while (timeElapsed < interval_ultrasound );
  timeElapsed = 0;
  
  //analogWrite(trigPin, HIGH);
  digitalWrite(ultrasoundTrigPin, HIGH);

  timeElapsed = 0;
  while (timeElapsed < interval_ultrasound );
  timeElapsed = 0;
  
  digitalWrite(ultrasoundTrigPin, LOW);
  duration_ultrasound = pulseIn(ultrasoundEchoPin,HIGH);
  
  duration_ultrasound = duration_ultrasound/ 29 / 2;
  if(duration_ultrasound < 10)
    duration_ultrasound = 10;
  if (duration_ultrasound > 200)
    duration_ultrasound = 200;
    
  head_ultrasound = duration_ultrasound;
  Serial.print("head_ultrasound: ");
  Serial.println(head_ultrasound);
  ultrasoundPinIndex++;

  if(ultrasoundPinIndex == 1)
    {
      ultrasoundPinIndex = 0;
    }
}


void shortRangeInfraredSensorPoll()
{
  int infraredPinIndex = 0;
  float v = 0;
  unsigned long currentTotalReading = 0; //This thing is not just currentTotalReading, it's also used to hold the average and the return value.
  while(infraredPinIndex<2)
  {  
    currentTotalReading=analogRead(shortRangeInfraredPin[infraredPinIndex]);
    v=(float)currentTotalReading*5.0/1024.0;
    currentTotalReading = -34.159*v*v*v*v*v+ 298.19*v*v*v*v -1004.3*v*v*v + 1638.3*v*v -1333.7*v + 497.11;
    if(currentTotalReading > 150)
    {
      currentTotalReading = 150;
    }
    else if(currentTotalReading < 20)
    {
      currentTotalReading = 20;
    }
    switch(infraredPinIndex)
    {
      case 0:
        chestLeft = currentTotalReading;
//        Serial.print("chestLeft: ");
//        Serial.print(currentTotalReading);
        break;
      case 1:
        chestRight = currentTotalReading;
//        Serial.print("chestright");
//        Serial.println(currentTotalReading);
        break;
    }
   //Poll infrared sensor using the values of infraredPins[infraredPinIndex]
   infraredPinIndex++; 
  }
  if(infraredPinIndex=2)
  {
    //Reset pins
    infraredPinIndex = 0;
  }
}

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

int return_motor_strength(int distance, int type)
{
  float unit;
  int approxfreq = MIN_INTERVAL;
  int cmrange = 0;
  //Based on the distance detected by the sensor and the type of sensor, return the interval for the motor to vibrate at.
  //Sensors are expected to ONLY return values withi their ranges.
  if(type == 1) //Short-range IR, distance returned from 20-150 cm
  {
   cmrange = 130;
  }
  else if (type == 2) //Ultrasonic, 10-200cm
  {
    cmrange = 190;
  }
  else if (type == 3) //Long-range IR, distance returned from 100 to 320 cm
  {
    cmrange = 220; //This may require an inverse of approxfreq - longer distances indicate obstacles or drops. Or I just use my brain
  }
  unit = (float)(MAX_INTERVAL - MIN_INTERVAL)/(float)cmrange;
  if((type == 1) || (type == 2)) {
  approxfreq = MIN_INTERVAL + (distance*unit);
  }
 else if (type == 3) {
  approxfreq = MAX_INTERVAL - (distance*unit);
  Serial.println("Approx frequency: ");
  Serial.println(approxfreq);
   }  
  return approxfreq;
}

void longRangeInfraredSensorPoll()
{
//  pinMode(sensorPin0, INPUT); 
//  pinMode(sensorPin1, INPUT);
//  pinMode(sensorPin2, INPUT);
//  pinMode(sensorPin3, INPUT);
  int infraredPinIndex = 0;
  float v = 0;
  float currentTotalReading = 0; //This thing is not just currentTotalReading, it's also used to hold the average and the return value.
  while (infraredPinIndex < 2)
  {  
//    for(int i=0;i<10;i++) //Poll ten times
//    {  
    //currentTotalReading+=digitalRead(longRangeInfraredPin[infraredPinIndex]);
    if (infraredPinIndex == 0){
    currentTotalReading=analogRead(sensorPin0);
  }
    
    if (infraredPinIndex == 1){
    currentTotalReading=analogRead(sensorPin1); 
  }
//    }
//    currentTotalReading/=(float)10;
//    v=(float)currentTotalReading*5.0/1024.0;
    v=(float)currentTotalReading*5.0/1024.0;
    //currentTotalReading = -270.79*v*v*v*v*v + 3289.7*v*v*v*v - 15920*v*v*v + 38434*v*v - 46483*v + 22797;
   currentTotalReading = -270.79*pow(v, 5) + 3289.7*pow(v, 4) - 15920*pow(v, 3) + 38434*pow(v, 2) - 46483*v + 22797;
    if (currentTotalReading > 320)  //Limit the return value of the sensor.
   {
     currentTotalReading = 320;
   } 
   else if (currentTotalReading < 80)
   {
     currentTotalReading = 80;
   }
    switch(infraredPinIndex)
    {
      case 0:
        head_IR = currentTotalReading;
//        Serial.print("head");
//        Serial.println(currentTotalReading);
//        Serial.print("\t");
        break;
      case 1:
        waistCentre_IR = currentTotalReading;
//        Serial.print("waist: ");
//        Serial.print(currentTotalReading);
//        Serial.print("\t");
//        Serial.print("\n");
        break;
    }
    
   //Poll infrared sensor using the values of infraredPins[infraredPinIndex]
   currentTotalReading = 0;
   infraredPinIndex++; 
  }
  if(infraredPinIndex==2)
  {
    //Reset pins
    infraredPinIndex = 0;
  }   
}

// --------------------------------------------------------------------------- Drive


void  ultrasound_motordriver(){
  int ultrasound_delay = return_motor_strength(head_ultrasound, 2);
  unsigned long currentMillis = millis();

    if(currentMillis - previousHeadUSMillis > ultrasound_delay) {
      previousHeadUSMillis = currentMillis;   

      if (head_ultrasound_State == LOW)
        head_ultrasound_State = HIGH;
      else
        head_ultrasound_State = LOW; 
        
      digitalWrite( motor_head_ultrasound[0], head_ultrasound_State);  
    }  

}
  
  

void shortrangeIR_motordriver(){  

  int shortrangeIRLeft_delay = return_motor_strength(chestLeft, 1);
  int shortrangeIRRight_delay = return_motor_strength(chestRight, 1);
  unsigned long currentMillis = millis();

    if(currentMillis - previousChestLeftMillis > shortrangeIRLeft_delay) {
      previousChestLeftMillis = currentMillis;   

      if (chest_left_State == LOW)
        chest_left_State = HIGH;
      else
        chest_left_State = LOW; 
        
      digitalWrite( motor_chestLeft[0], chest_left_State);  
    }
 currentMillis = millis();
  
  if(currentMillis - previousChestRightMillis > shortrangeIRRight_delay) {
     previousChestRightMillis = currentMillis;
      
      if (chest_right_State == LOW)
         chest_right_State = HIGH;
      else
         chest_right_State = LOW;
        
      digitalWrite( motor_chestRight[0], chest_right_State); 
  }
}

void longrangeIR_motordriver(){
 int longrangeHeadIR_delay = return_motor_strength(head_IR, 3);
 int longrangewaistcentreIR_delay = return_motor_strength(waistCentre_IR, 3);
  unsigned long currentMillis = millis();

    if(currentMillis - previousHeadIRMillis > longrangeHeadIR_delay) {
      previousHeadIRMillis = currentMillis;   

      if (head_IR_State == LOW)
        head_IR_State = HIGH;
      else
        head_IR_State = LOW; 
        
      digitalWrite( motor_head_IR[0], head_IR_State);  
    }
    
 currentMillis = millis();
  if(currentMillis - previousBeltMillis > longrangewaistcentreIR_delay) {
     previousBeltMillis = currentMillis;
      
      if (waist_centre_IR_State == LOW)
         waist_centre_IR_State = HIGH;
      else
         waist_centre_IR_State = LOW;
        
      digitalWrite( motor_waistCentre_IR[0], waist_centre_IR_State); 
  }  
 
}


void vibrateMotors()
{
  ultrasound_motordriver();
  shortrangeIR_motordriver();
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
  //Serial.println("Slow debug");
  ultrasoundSensorPoll();
  shortRangeInfraredSensorPoll();
  longRangeInfraredSensorPoll();
  vibrateMotors();
}
