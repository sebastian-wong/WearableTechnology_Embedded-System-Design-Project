#include <avr/io.h>

int shoulderLeft, shoulderRight, chestLeft, chestRight, head;
int waistLeft, waistRight, waistCenter;
int ultrasoundTrigPin[5]={10,12,14,16,18}; //Put pins to be triggered here
int ultrasoundEchoPin[5]={11,13,15,17,19}; //Put pins to receive trigger here
int shortRangeInfraredPin[3]={5,6};
int longRangeInfraredPin=7;
int shortRangeInfraredPolled = 0;
int longRangeInfraredPolled = 0;
int ultrasoundPolled = 0;

void ultrasoundSensorPoll() //Note that ultrasound sensors read after each other
{
  int ultrasoundPinIndex = 0; //Scoped variable which resets to 0 at every beginning
  int trigPin = 0;
  int echoPin = 0;
  while(ultrasoundPinIndex < 5)
    {
     trigPin = ultrasoundTrigPin[ultrasoundPinIndex];
     echoPin = ultrasoundEchoPin[ultrasoundPinIndex];
     pinMode(trigPin, OUTPUT);
     pinMode(echoPin, INPUT);
     digitalWrite(trigPin, LOW);
     delayMicroseconds(2);
     digitalWrite(trigPin, HIGH);
     delayMicroseconds(5);
     digitalWrite(trigPin, LOW);
     duration = pulseIn(echoPin,HIGH);
     duration = duration / 29 / 2;
     switch(ultrasoundPinIndex)
    {
      case 0:
        shoulderLeft = duration;
      case 1:
        shoulderRight = duration;
      case 2:
        chestLeft = duration;
      case 3:
        chestRight = duration;
      case 4:
        head = duration;
      default:
        break;
    }
    //Need to delay for a while to let echoes die off 
    //Poll ultrasound sensor using the value of ultrasoundPins[ultrasoundPinIndex]
    //Store to shoulderleft/shoulderright etc. depending on value of ultrasoundPinIndex
      ultrasoundPinIndex++;
    }
    
  if(ultrasoundPinIndex = 5)
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
    for(int i=0;i<10;i++) //Poll ten times
    {  
    currentTotalReading+=analogRead(shortRangeInfraredPin[infraredPinIndex]);
    }
    currentTotalReading/=(float)10;
    v=(float)currentTotalReading*5.0/1024.0;
    currentTotalReading = -34.159*v*v*v*v*v+ 298.19*v*v*v*v -1004.3*v*v*v + 1638.3*v*v -1333.7*v + 497.11;
    switch(infraredPinIndex)
    {
      case 0:
        waistleft = currentTotalReading;
      case 1:
        waistRight = currentTotalReading;
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

void longRangeInfraredSensorPoll()
{
  //This thing could be combined, but I'm not doing it because I'm dumb.
  int infraredPinIndex = 0;
  float v = 0;
  unsigned long currentTotalReading = 0;
  for(int i=0;i<10;i++)
  {
    currentTotalReading+=analogRead(longRangeInfraredPin[infraredPinIndex]);
  }
  currentTotalReading/=(float)10;
  v=(float)currentTotalReading*5.0/1024.0;
  currentTotalReading = -270.79*v*v*v*v*v + 3289.7*v*v*v*v - 15920*v*v*v + 38434*v*v - 46483*v + 22797;
  waistcenter = currentTotalReading;
}

void vibrateMotors()
{
  //Insert Angela code here.
}
  

void UARTKeypadInterrupt()
{

}

void setup()
{
  //Do setup stuff here.
}

void loop() //Main time loop runs here
{
  
}
