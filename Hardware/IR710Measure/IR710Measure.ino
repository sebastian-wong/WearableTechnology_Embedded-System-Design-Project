#define sensorPin 0
#define VOLTS_PER_UNIT    .0049F        // (.0049 for 10 bit A-D)
#define SAMPLESIZE 5000
float volts;
float inches;
float proxSens = 0;
float avgVoltage = 0;
float distance = 0;
int cm;

int count = 0;
unsigned long total = 0;

void setup() {
  
  Serial.begin(9600); 
    pinMode(sensorPin, INPUT);
    
}

void loop() {
 proxSens = analogRead(sensorPin);
 //proxSens *= (5.0/1024.0); //voltage measured 
 total += proxSens;
 count++;
 if(count==SAMPLESIZE)
 {
   avgVoltage=(float)total*5.0/1024.0/(float)SAMPLESIZE;
   count = 0;
   total = 0;
   Serial.print(avgVoltage);
   distance = -270.79*pow(avgVoltage, 5) + 3289.7*pow(avgVoltage, 4) - 15920*pow(avgVoltage, 3) + 38434*pow(avgVoltage, 2) - 46483*avgVoltage + 22797;
   Serial.print("\n");
   if(distance>=80 && distance <=320)
   {
   Serial.print("Distance: ");
   Serial.print(distance);
   Serial.print("\n");
   distance=distance/10;
   Serial.print("Abstracted distance: ");
   Serial.print(int(distance));
   Serial.print("\n");
   }
   else
   {
     Serial.print("Distance beyond reliable range.\n");
   }
 }
 /*
 volts = (float)proxSens * VOLTS_PER_UNIT; // ("proxSens" is from analog read)
  inches = 23.897 * pow(volts,-1.1907); //calc inches using "power" trend line from Excel
  cm = 60.495 * pow(volts,-1.1904);     // same in cm
  if (volts < .2) inches = -1.0;        // out of range    
    //Serial.print(cm);
    Serial.print('\n');
*/
}
