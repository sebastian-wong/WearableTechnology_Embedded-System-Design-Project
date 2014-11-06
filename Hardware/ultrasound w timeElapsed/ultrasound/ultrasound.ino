//Vcc 5v 
//start from distance: 3-4cm
//can be inaccurate around 1-2cm
//can measure up to 200cm with good accuracy 

#include <elapsedMillis.h>
#define echoPin A0 // Echo Pin
#define trigPin A1 // Trigger Pin

int numSamples = 0;
int totDist = 0;
elapsedMillis timeElapsed;
// delay in milliseconds
//1 micro [μ] = 0.001 milli [m]
unsigned int interval = 0.01;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
}

void loop()
{
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, cm;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  digitalWrite(trigPin, LOW);
  //delayMicroseconds(2);
  timeElapsed = 0;
  while (timeElapsed < interval );
  timeElapsed = 0;

  digitalWrite(trigPin, HIGH);
  //delayMicroseconds(5);
  timeElapsed = 0;
  while (timeElapsed < interval );
  timeElapsed = 0;
  
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);


  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  totDist += cm;
  numSamples ++;


  if(numSamples = 1000){
    if(cm > 350){
      Serial.print("Too far!");
       Serial.print("\n");
  }
    else{
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
    totDist = numSamples = 0;
  }
  }  
  delay(100);
}


long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ultrasound sensor travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
