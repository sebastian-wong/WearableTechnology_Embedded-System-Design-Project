
#include <elapsedMillis.h>
#include "Wire.h"
#include <String.h>
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h" 
#include "HMC5883L.h"


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
int16_t magx[10];
int16_t magy[10];
int16_t magz[10];
int16_t accx[50];
int16_t accy[50];
int16_t accz[50];
int16_t prevx[50];
int16_t prevy[50];
int16_t prevz[50];
int16_t cmp[3];
int16_t avgx,avgy,avgz,threshold,prevax,prevay,prevaz;
int ax_count, ay_count, az_count,x_count,y_count,z_count,len, cmp_full = 0;
int axis = -1; //0 for x, 1 for y, 2 for z
int stepcount,count = 0;
int pts = 0;
int first = -1;
int flag = 0;


//#define LED_PIN 13
#define MAX_STEP 2 //max of 2 steps per second

bool blinkState = false;

int16_t rollingAverage(int16_t *store, int size, int16_t entry)
{
	int l;
	long total = 0;
	int16_t result;

	for(l=0; l<size-1; l++)
	{
		store[l] = store[l+1];
		total += store[l];
	}
	store[size-1] = entry;
	total += entry;
	result = total / size;

	return result;
}

int16_t maximum(int16_t *data,int count)
{
	int l;
	int16_t maxval;

	maxval = -32768;

	for(l=0; l<count; l++)
	{
		if(data[l] > maxval)
		{
			maxval = data[l];
		}
	}
	return maxval;
}

int16_t minimum(int16_t *data,int count)
{
	int l;
	int16_t minval;

	minval = 32767;

	for(l=0; l<count; l++)
	{
		if(data[l] < minval)
		{
			minval = data[l];
		}
	}
	return minval;
}

int16_t mean(int16_t *data, int count)
{
	int i;
	long total;
	int16_t result;

	total = 0;
	for(i=0; i<count; i++)
	{
		total = total + data[i];
	}
	result = total / count;
	return result;
}

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
    if (x_count != 10 && y_count != 10 && z_count != 10){//fill in arrays before taking moving average
    magx[x_count] = ax;
    magy[y_count] = ay;
    magz[z_count] = az;
    x_count++;
    y_count++;
    z_count++;
    }
    
    else{
      
    if (ax_count ==  50 && ay_count == 50 && az_count == 50 ){
         ax_count = 0;
         ay_count = 0;
         az_count = 0; 
         first = 1;  
  }
    
    avgx = rollingAverage(magx,10,ax); //moving average 
    avgy = rollingAverage(magy,10,ay); //moving average 
    avgz = rollingAverage(magz,10,az); //moving average       
     accx[ax_count] = avgx;
     accy[ay_count] = avgy;
     accz[az_count] = avgz;
//     Serial.print("count: ");
//     Serial.print(az_count);
//     Serial.print(" array: ");
//     Serial.print(accz[az_count]);
//      Serial.print("\n");
     ax_count++;
     ay_count++;
     az_count++;

    }
   if (ax_count ==  50 && ay_count == 50 && az_count == 50 ){//calculate min and max every 50 samples
//   Serial.print(accx[49]);
//   Serial.print("\t");
//   Serial.print("\n");
   memcpy(&prevx, &accx, sizeof(accx));//copy over array
   memcpy(&prevy, &accy, sizeof(accy));//copy over array
   memcpy(&prevz, &accz, sizeof(accz));//copy over array
//   Serial.print(accx[49]);
//   Serial.print("\n");
//   Serial.print(prevx[49]);
//   Serial.print("\n");
         
   minx = minimum(accx, 50);
   miny = minimum(accy, 50);
   minz = minimum(accz, 50);
//  Serial.print(minz);
//   Serial.print("\n");
   
   maxx = maximum(accx, 50);
   maxy = maximum(accy, 50);
   maxz = maximum(accz, 50);
//   Serial.print(maxz);
//   Serial.print("\n");
   
   ptpx = abs(maxx) + abs(minx);
   cmp[0] = ptpx;
   
   ptpy = abs(maxy) + abs(miny);
   cmp[1] = ptpy;
   
   ptpz = abs(maxz) + abs(minz);
   cmp[2] = ptpz;
   
   ptp = maximum(cmp, 3);
   threshold = ptp/2;
   if (ptp == ptpx){
     axis = 0; //x axis
     //threshold = mean(accx, 50);
   }
 
   if (ptp == ptpy){
     axis = 1; //y axis
      //threshold = mean(accy, 50);
   }
   
   if (ptp == ptpz){
     axis = 2; //z axis
      //threshold = mean(accz, 50);
      //threshold = threshold - 1000;
      //threshold -= (int16_t)-1200;
   }
   }
  if (first ==1){    
    if (axis == 0 && ax_count != 1){ //active axis is x
            diff = accx[count] - accx[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
              Serial.print("old:\t");
              Serial.print(accx[count]); Serial.print("\t");
              Serial.print("Threshold:\t");
              Serial.print(threshold); Serial.print("\t");
              Serial.print("new:\t");
              Serial.print(accx[count+1]); Serial.print("\t");
              Serial.print("Steps:\t");
              Serial.print(stepcount);  Serial.print("\t"); 
            if (count == 49){
              count =0;
            }
            if (diff > precisionx && accx[count] > threshold && accx[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisionx && accx[count] < threshold && accx[count+1] > threshold && (timeElapsed - prevtime) <= 2000 && (timeElapsed-prevtime) >= 300){ //subsequent steps
             stepcount++;
             prevtime = 0;  
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
          }
    if (axis == 1 & ay_count != 1){ //active axis is y
            
            diff = accy[count] - accy[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
              Serial.print("old:\t");
              Serial.print(accy[count]); Serial.print("\t");
              Serial.print("Threshold:\t");
              Serial.print(threshold); Serial.print("\t");
              Serial.print("new:\t");
              Serial.print(accy[count+1]); Serial.print("\t");
              Serial.print("Steps:\t");
              Serial.print(stepcount);  Serial.print("\t");
            if (count == 49){
              count =0;
            }
           if (diff > precisiony && accy[count] > threshold && accy[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisiony && accy[count] > threshold && accy[count+1] < threshold && (timeElapsed - prevtime) <= 2000 && (timeElapsed-prevtime) >= 300){ //subsequent steps
             stepcount++;
            prevtime = 0; 
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
    }
    if (axis == 2 && az_count != 1){ //active axis is z
          diff = accz[count] - accz[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
              Serial.print("old:\t");
              Serial.print(accz[count]); Serial.print("\t");
              Serial.print("Threshold:\t");
              Serial.print(threshold); Serial.print("\t");
              Serial.print("new:\t");
              Serial.print(accz[count+1]); Serial.print("\t");
              Serial.print("Steps:\t");
              Serial.print(stepcount);  Serial.print("\t");

//            Serial.print(diff);
//            Serial.print("\n"); 
//            Serial.print(prevz[count]);
//            Serial.print("\n"); 
//            Serial.print(prevz[count+1]);
//            Serial.print("\n"); 
            
            if (count == 49){ 
              count =0;
            }
            if (diff > precisionz && accz[count] > threshold && accz[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisionz && accz[count] > threshold && accz[count+1] < threshold && (timeElapsed - prevtime) <= 2000 && (timeElapsed-prevtime) >= 300){ //subsequent steps
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
//  Serial.print("Steps:\t");
//  Serial.print(stepcount);  Serial.print("\t");
// Serial.print("Threshold:\t");
//Serial.print(threshold); Serial.print("\t");
 //   Serial.print("Axis:\t");
 // Serial.print(az_count);
  Serial.print("\t\n");
}
   
