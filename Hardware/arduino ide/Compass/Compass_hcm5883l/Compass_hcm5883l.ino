//values printed quite stable 
// bearing is the output from the compass 
//accuracy up to +-5 degrees usually as compared to the magnetometer downloaded on my handphone
// but sometimes can go up to difference of 10 degrees
//the orientation of the board is very important
#include <Wire.h>
#include "compass.h"

void setup(){
  Serial.begin(9600);
  // Serial.print("Setting up I2C ........\n");
  Wire.begin();
  
  //default offset set to the three axis based 
  // in the orientation that the magnetometer will move 
  compass_x_offset = 193.91;
  compass_y_offset = -95.05;
  compass_z_offset = 180.94;
  
 
  compass_x_gainError = 1.12;
  compass_y_gainError = 1.13;
  compass_z_gainError = 1.03;
  
  compass_init(1);
  compass_debug = 1;
  compass_calibration(3);
}

void loop(){
  
  Serial.print("compass_x_offset = ");
  Serial.println(compass_x_offset);
  Serial.print("compass_y_offset = ");
  Serial.println(compass_y_offset);
  Serial.print("compass_z_offset = ");
  Serial.println(compass_z_offset);
  Serial.println();
 
  compass_scalled_reading(); 
  Serial.print("x = ");
  Serial.println(compass_x_scalled);
  Serial.print("y = ");
  Serial.println(compass_y_scalled);
  Serial.print("z = ");
  Serial.println(compass_z_scalled);

  compass_heading();
  Serial.print ("Heading angle = ");
  Serial.print (bearing);
  Serial.println(" Degree");
  Serial.println();
  
  delay(3000);
  
  
}











