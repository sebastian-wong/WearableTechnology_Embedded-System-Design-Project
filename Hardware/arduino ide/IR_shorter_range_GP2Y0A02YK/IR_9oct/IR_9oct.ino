//Vcc 5v

#define voltoutPin A0
  
int numSamples = 0;
float currentir = 0;
float currentir_in_volts = 0;
float distance = 0;
float totdistance=0;
float avedistance = 0;

//convert voltage to Dis
int convertIRvoltsToDis(float v) { 
    return (-34.159*v*v*v*v*v+ 298.19*v*v*v*v -1004.3*v*v*v + 1638.3*v*v -1333.7*v + 497.11); 
}
//-34.159*v^5 +298.19v^4  - 1004.3 v^3 + 1638.3 v^2 -1333.7v +497.11

void setup() {
   Serial.begin(9600);
}

void loop() {
   currentir = analogRead(voltoutPin);
   
   currentir_in_volts = ( float(currentir /1024.0) * float (5.0) ) ;
   
   if (currentir_in_volts >= 0.35 && currentir_in_volts <= 2.5){
     distance = convertIRvoltsToDis( float(currentir_in_volts));
     totdistance += distance;
     numSamples++;
   }
   
    if (numSamples >= 1000){
     avedistance = totdistance / numSamples ;
     Serial.println(avedistance);
     numSamples = currentir = currentir_in_volts = distance = totdistance = avedistance = 0;
   }
   delay(10);
}   
   

