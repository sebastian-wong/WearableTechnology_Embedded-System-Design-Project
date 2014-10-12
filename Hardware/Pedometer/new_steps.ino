
#include <elapsedMillis.h>
#include "Wire.h"
#include <String.h>
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h" 
#include "HMC5883L.h"
#include "Average.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//HMC5883L compass;
elapsedMillis timeElapsed;

unsigned long prevtime;
int16_t precisionx = 20; 
int16_t precisiony = 20; 
int16_t precisionz = 20; 
int16_t ax, ay, az,gx,gy,gz,minx,miny,minz,maxx,maxy,maxz, ptpx, ptpy, ptpz,ptp, diff;
int16_t magx[5];
int16_t magy[5];
int16_t magz[5];
int16_t accx[45];
int16_t accy[45];
int16_t accz[45];
int16_t prevx[45];
int16_t prevy[45];
int16_t prevz[45];
//int16_t minima[250];
//int16_t maxima[250];
int16_t cmp[3];
int16_t avgx,avgy,avgz,threshold,prevax,prevay,prevaz;
int ax_count, ay_count, az_count,x_count,y_count,z_count,len, cmp_full = 0;
int axis = -1; //0 for x, 1 for y, 2 for z
int stepcount,count = 0;
int pts = 0;
int first = 0;


//#define LED_PIN 13
#define MAX_STEP 2 //max of 2 steps per second

bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    accelgyro.setI2CMasterModeEnabled(0);
    accelgyro.setI2CBypassEnabled(1);

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    // configure Arduino LED for
    //pinMode(LED_PIN, OUTPUT);
}

void steps(){
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    if (x_count != 5 && y_count != 5 && z_count != 5){//fill in arrays before taking moving average
    magx[x_count] = ax;
    magy[y_count] = ay;
    magz[z_count] = az;
    x_count++;
    y_count++;
    z_count++;
    }
    
    else{
    avgx = rollingAverage(magx,5,ax); //moving average 
    avgy = rollingAverage(magy,5,ay); //moving average 
    avgz = rollingAverage(magz,5,az); //moving average       
     accx[ax_count] = avgx;
     accy[ay_count] = avgy;
     accz[az_count] = avgz;
    
     ax_count++;
     ay_count++;
     az_count++;

    }
   if (ax_count ==  50 && ay_count == 50 && az_count == 50 ){//calculate min and max every 50 samples
   ax_count = 0;
   ay_count = 0;
   az_count = 0; 
   first = 1;
   memcpy(&prevx, &accx, 50);//copy over array
   memcpy(&prevy, &accy, 50);//copy over array
   memcpy(&prevz, &accz, 50);//copy over array
     
   minx = minimum(accx, 50);
   miny = minimum(accy, 50);
   minz = minimum(accz, 50);
   
   maxx = maximum(accx, 50);
   maxy = maximum(accy, 50);
   maxz = maximum(accz, 50);
   
   ptpx = abs(maxx) + abs(minx);
   cmp[0] = ptpx;
   
   ptpy = abs(maxy) + abs(miny);
   cmp[1] = ptpy;
   
   ptpz = abs(maxz) + abs(minz);
   cmp[2] = ptpz;
   
   ptp = maximum(cmp,3);
   threshold = ptp/2;
   if (ptp = ptpx){
     axis = 0; //x axis
   }
 
   if (ptp = ptpy){
     axis = 1; //y axis
   }
   
   if (ptp = ptpz){
     axis = 2; //z axis
   }
   }
  if (first ==1){    
    if (axis == 0){ //active axis is x
            diff = prevx[count] - prevx[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
            if (count == 48){
              count =0;
            }
            if (diff > precisionx && prevx[count] > threshold && prevx[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisionx && prevx[count] > threshold && prevx[count+1] < threshold && (timeElapsed-prevtime) <= 2000 && (timeElapsed-prevtime) >= 200){ //subsequent steps
             stepcount++;
             prevtime = 0;    
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
          }
    if (axis == 1){ //active axis is y
            diff = prevy[count] - prevy[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
            if (count == 48){
              count =0;
            }
           if (diff > precisiony && prevy[count] > threshold && prevy[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisiony && prevy[count] > threshold && prevy[count+1] < threshold && (timeElapsed-prevtime) <= 2000 && (timeElapsed-prevtime) >= 200){ //subsequent steps
             stepcount++;
            prevtime = 0; 
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
    }
    if (axis == 2){ //active axis is z
          diff = prevz[count] - prevz[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
            if (count == 48){
              count =0;
            }
            if (diff > precisionz && prevz[count] > threshold && prevz[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisionz && prevz[count] > threshold && prevz[count+1] < threshold && (timeElapsed-prevtime) <= 2000 && (timeElapsed-prevtime) >= 200){ //subsequent steps
             stepcount++;  
            prevtime = 0; 
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
          }
 }
}

void loop(){
  steps();
  Serial.print("Steps:\t");
  Serial.print(stepcount);  Serial.print("\t");
    Serial.print("Threshold:\t");
  Serial.print(threshold); Serial.print("\t");
 //   Serial.print("Axis:\t");
 // Serial.print(az_count);
  Serial.print("\t\n");
}
   
