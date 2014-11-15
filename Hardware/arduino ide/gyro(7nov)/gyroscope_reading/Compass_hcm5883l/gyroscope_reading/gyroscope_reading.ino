// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-10-07 - initial release
 
/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================*/
 
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"
 
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "elapsedMillis.h"
#include "compass.h"

 
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
HMC5883L compass2; 
 
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;


int16_t current_ax, current_ay, current_az;
int16_t prev_ax, prev_ay, prev_az;

int16_t diff_ax;
int16_t diff_ay;
 
#define LED_PIN 13
bool blinkState = false;
 
// accelerometer values
int accel_reading;
int accel_corrected;
int accel_offset = 200;
float accel_angle;
float accel_scale = 1; // set to 0.01
 
// gyro values
int gyro_offset = 151; // 151
int gyro_corrected;
int gyro_reading;
float gyro_rate;
float gyro_scale = 0.02; // 0.02 by default - tweak as required
float gyro_angle;
float gyro_angle2degree;
float holding_gyro_angle2degree;
float gyro_angledifference;
float gyro_mapped_onto_compass;
float loop_time = 0.05; // 50ms loop
float angle = 0.00; // value to hold final calculated gyro angle
 
// time stamp variables
//int last_update;
//int cycle_time;
//long last_cycle = 0;

int compassCalibrated = 0;

elapsedMillis timeElapsed;
elapsedMillis timeOut;
// delay in milliseconds
unsigned int interval = 2000;


int bearing_int ;


void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();
 
  // initialize serial communication
  //Serial.begin(9600);
  Serial.begin(115200);
 
  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  accelgyro.setI2CBypassEnabled(true);
  compass2=HMC5883L();
  compass2.SetScale(1.3); 
  compass2.SetMeasurementMode(Measurement_Continuous);
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
 
 
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
 

// // configure Arduino LED for
//  pinMode(LED_PIN, OUTPUT);
}
 
void loop(){
 
  MagnetometerScaled scaled = compass2.ReadScaledAxis();
  mx=scaled.XAxis;
  my=scaled.YAxis;
  mz=scaled.ZAxis;
 
//calibration
//1) ultraperfect calibration 
//2) Time out calibration
 
 // read raw accel/gyro measurements from device
 accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
 current_ax = ax;
 current_ay = ay;
 current_az = az;

 timeElapsed = 0;
// while (timeElapsed < interval ){
   prev_ax = current_ax;
   prev_ay = current_ay;
   prev_az = current_az;
   accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
   current_ax = ax;
   current_ay = ay;
   current_az = az;
 //} 
 timeElapsed = 0;
 diff_ax = abs (current_ax - prev_ax);
 diff_ay = abs (current_ay - prev_ay);
 
 //timeOut = 0;
 //if chip is placed parallel to the ground, take readings from compass
 //else take readings from gyro after mapping gyroangle to degrees
  if (az < 16000 && az >= 15000 && diff_ax <= 500 && diff_ay <= 500){
    compass_scalled_reading(); 
    compass_heading();
    holding_gyro_angle2degree = gyro_angle2degree;
    //gyro_angle2degree=bearing; //re calibrate gyro when user is not moving
    compassCalibrated = 1;
    bearing_int = int(bearing);
    timeOut=0;
    
    Serial.print ("Heading angle = ");
    Serial.print (bearing_int);
    Serial.println(" Degree");
    Serial.println();
  }
  else{
  // gyro_z_Axis angle calc  
    if(timeOut >= 5000){  //re calibrate gyro readings under timed conditions
      compass_heading();
      holding_gyro_angle2degree = gyro_angle2degree;
      //gyro_angle2degree = bearing; 
      compassCalibrated = 1;
      bearing_int = int(bearing);
      timeOut = 0;
    }
    gyro_reading = gz;
    gyro_corrected = (float)((gyro_reading/131) - gyro_offset);  // 131 is sensivity of gyro from data sheet
    gyro_rate = (gyro_reading * gyro_scale) * -loop_time;      // loop_time = 0.05 ie 50ms     
    gyro_angle = gyro_angle + gyro_rate;
        
   gyro_angle2degree = map(gyro_angle, -830, 830, 0, 360);
   gyro_angle2degree = constrain(gyro_angle2degree, 0, 360);
   
   gyro_angle2degree = gyro_angle2degree*2; //map to 0 to 360 degrees
   
   if(gyro_angle2degree > 359){
     gyro_angle2degree = float(int(gyro_angle2degree) % 360) ; 
    }
    
   gyro_angledifference=(holding_gyro_angle2degree - gyro_angle2degree);
   holding_gyro_angle2degree = gyro_angle2degree;
   
   if(compassCalibrated == 1){
   gyro_mapped_onto_compass = bearing + gyro_angledifference;
   }
    
   gyro_mapped_onto_compass = abs(gyro_mapped_onto_compass);
        
      
      
  bearing_int = int(gyro_mapped_onto_compass);
  Serial.print ("Heading angle = ");
  Serial.print (bearing_int);
  Serial.println(" Degree");
  Serial.println(); 
  
 }
  
  
//  // accelerometer_X_Axis angle calc
//  accel_reading = az;
//  accel_corrected = accel_reading - accel_offset; //accel_offset set by compass in some way
//  accel_corrected = map(accel_corrected, -16800, 16800, -90, 90);
//  accel_corrected = constrain(accel_corrected, -90, 90);
//  accel_angle = (float)(accel_corrected * accel_scale);
//  Serial.print(accel_angle);
//  Serial.print("\t");
//   
//// // gyro_z_Axis angle calc  
////
//  gyro_reading = gz;
//  gyro_corrected = (float)((gyro_reading/131) - gyro_offset);  // 131 is sensivity of gyro from data sheet
//  gyro_rate = (gyro_reading * gyro_scale) * -loop_time;      // loop_time = 0.05 ie 50ms     
//  gyro_angle = gyro_angle + gyro_rate;
//  gyro_angle2degree = gyro_angle *(180/50);
////  
////  
//   // Correct for when signs are reversed.
//  if(gyro_angle2degree < 0.0)
//    gyro_angle2degree += 360.0;
//    
//  // Check for wrap due to addition of declination.
//  if(gyro_angle2degree > 360.0)
//    gyro_angle2degree -= 360.0;
//  
  
//  // print values to serial monitor for checking
//  Serial.print(ax);
//  Serial.print("\t"); 
//  Serial.print(ay);
//  Serial.print("\t");
//  Serial.print(az);
//  Serial.print("\t");
//  Serial.print(gx);
//  Serial.print("\t"); 
//  Serial.print(gy);
//  Serial.print("\t");
//  Serial.print(gz);
//  Serial.print("\t");
//  Serial.print(mx);
//  Serial.print("\t"); 
//  Serial.print(my);
//  Serial.print("\t");
//  Serial.print(mz);
//  Serial.print("\t");
//  Serial.print(gyro_corrected);
//  Serial.print("\t");
//  Serial.print(gyro_angle);
//  Serial.print("\t");
//  Serial.println(" ");
//  Serial.print(gyro_angle2degree);
//  Serial.print("\t");
//  Serial.println(" ");
    
//  // blink LED to indicate activity
//  blinkState = !blinkState;
//  digitalWrite(LED_PIN, blinkState);

  
 //timestamp
 //time_stamp();

 
 if(timeOut >= 5000){  //re calibrate gyro readings under timed conditions
//   compass_scalled_reading(); 
   compass_heading();
   gyro_angle2degree = bearing; 
   bearing_int = int(bearing);
   timeOut = 0;
 }

}

//void time_stamp(){
//  while ((millis() - last_cycle) < 50){
//  //delay(1);
//  }
//  // once loop cycle reaches 50ms, reset timer value and continue
//  cycle_time = millis() - last_cycle;
//  last_cycle = millis();
//}
