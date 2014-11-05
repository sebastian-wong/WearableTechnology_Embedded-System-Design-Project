#include <Keypad.h>

#include <avr/interrupt.h>
#include <time.h>
#include <Arduino.h>

#include <elapsedMillis.h>
#include <Wire.h>
#include <String.h>
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h" 
#include "HMC5883L.h"

//compass
#include "compass.h"

#define F_CPU 16000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUDRATE * 16UL))) - 1)

#define MAX_INTERVAL 1300 //Milliseconds
#define MIN_INTERVAL 100 //Milliseconds

uint8_t input = 0; //for incoming serial data
uint8_t sensor_Len = 25;
uint8_t divisor = 17;

//Sensor pins
#define sensorPin0 A0 //headIR
#define sensorPin1 A1 //belt IR
#define sensorPin2 A2 //left IR
#define sensorPin3 A3 //right IR
//Ultrasound pins are 2 and 3

byte shortRangeInfraredPin[] = {A2, A3}; //For shortrange polling
int head_IR, head_ultrasound, waistCentre_IR, chestLeft, chestRight;
int ultrasoundTrigPin = 2;
int ultrasoundEchoPin = 3;

//for UART1
static const int RX_BUFFER_SIZE = 20;
volatile unsigned char buffer[RX_BUFFER_SIZE];
volatile uint8_t StrRxFlag=0;
volatile unsigned int rx_head = 0, rx_tail = 0;

uint8_t compass_index = 1;

enum STATE {
  notConnected,
  Connected
} connectionSTATE;

enum STATE connectionStatus = notConnected;

static uint8_t const HANDSHAKE = 1;
static uint8_t const ACK       = 2;
static uint8_t const NAK       = 3;
static uint8_t const READ      = 4;
static uint8_t const ACK_READ  = 5;
static uint8_t const ACK_CHECKSUM = 8;
static uint8_t const NACK_CHECKSUM = 46;
static uint8_t const NUM        = 9;
static uint8_t const ACK_NUM   = 40;
static uint8_t const NACK_NUM  = 41;
static uint8_t const ACK_NUM_CHECKSUM = 42;
static uint8_t const NACK_NUM_CHECKSUM = 43;
static uint8_t const VOICE     = 44;
static uint8_t const ACK_VOICE     = 45;
static uint8_t const PEDO      = 46;
static uint8_t const ACK_PEDO = 47;

//Sensor Acknowlegements 
static uint8_t const ACK_S0 = 10;
static uint8_t const ACK_S1 = 11;
static uint8_t const ACK_S2 = 12;
static uint8_t const ACK_S3 = 13;
static uint8_t const ACK_S4 = 14;
static uint8_t const ACK_S5 = 15;
static uint8_t const ACK_S6 = 16;
static uint8_t const ACK_S7 = 17;
static uint8_t const ACK_S8 = 18;
static uint8_t const ACK_S9 = 19;
static uint8_t const ACK_S10 = 20;
static uint8_t const ACK_S11 = 21;
static uint8_t const ACK_S12 = 22;
static uint8_t const ACK_S13 = 23;
static uint8_t const ACK_S14 = 24;
static uint8_t const ACK_S15 = 25;
static uint8_t const ACK_S16 = 26;
static uint8_t const ACK_S17 = 27;
static uint8_t const ACK_S18 = 28;
static uint8_t const ACK_S19 = 29;
static uint8_t const ACK_S20 = 30;
static uint8_t const ACK_S21 = 31;
static uint8_t const ACK_S22 = 32;
static uint8_t const ACK_S23 = 33;
static uint8_t const ACK_S24 = 34;

/*
#Steps for this second up to 5 steps: 1 byte
#Compass bearings for those steps: 10 bytes #2 bytes = 1 compass reading
#Gyro readings: 3 bytes
#Barometer reading: 1 byte
#Ultrasound readings: 5 bytes
#IR readings: 5 bytes
#Total right now: 25 bytes
*/

//Buffer storing all sensor values - Index of buffer represents id of sensor 
uint8_t sensorData[25]   = {0, 1, 105, 1, 105, 1, 105, 1, 105, 1, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119};
uint8_t sensorDatacopy[25] = {1};
//to get from numpad file
volatile char parser_buffer_keys_copy[10] = {'\0'};
volatile int key_size = 0;
volatile uint8_t ActivateVoiceRpiFlag = 0;
volatile uint8_t ActivatepedoFlag = 0;
volatile uint8_t enter_pressed = 0;
volatile uint8_t enter = 0;
int8_t checkSum = 0;

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//HMC5883L compass;
elapsedMillis timeElapsed;
unsigned long previousHeadIRMillis = 0;

unsigned long prevtime;
int16_t precisionx = 200; 
int16_t precisiony = 200; 
int16_t precisionz = 150; 
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

//for pedo
//#define LED_PIN 13
#define MAX_STEP 2 //max of 2 steps per second

bool blinkState = false;

//for keypad
#define array_size 100 

elapsedMillis timeElapsed_keypad;
int debounceFlag = 0;
int EnterFlag = 0;

int counter = 0; //used in loops in various cases 

// delay in milliseconds
unsigned int interval = 1500;


char key = '?'; //invalid input from keypad


char buffer_keys[array_size]; //temp buffer to store inputs from user
char parser_buffer_keys[array_size]; //used to retrieve inputs directly 
int buffer_keys_count = 0;


//for key 5

int check_key = 0; //return respond if 5 was pressed to activate voice recognition 
elapsedMillis timeElapsed_in_case5; 
unsigned int interval_key5 = 500;
KeyState key_state = IDLE ;
KeyState prev_key_state = IDLE ;

int activate_voice = 0;
int activate_pedo = 0;

int head_IR_State = LOW; 


const byte ROWS = 4; 
const byte COLS = 3; 


char keys[ROWS][COLS] = {
  {'0','#','*'},
  {'8','9','7'},
  {'5','6','4'},
  {'2','3','1'}
};

//use in arduino mega 2560
//byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad // 2 - orange
//byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

//use in arduino uno
byte rowPins[ROWS] = {10, 9, 8, 7}; //connect to the row pinouts of the keypad 
byte colPins[COLS] = {13, 12, 11}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//end of keypad

//Ultrasound detection
void ultrasoundSensorPoll() //Note that ultrasound sensors read after each other
{
  long duration;
  int ultrasoundPinIndex = 0; //Scoped variable which resets to 0 at every beginning
  digitalWrite(ultrasoundTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasoundTrigPin, HIGH);
  delayMicroseconds(5);
  
  digitalWrite(ultrasoundTrigPin, LOW);
  duration = pulseIn(ultrasoundEchoPin,HIGH);
  
  duration = duration / 29 / 2;
  if(duration < 10)
    duration = 10;
  else if (duration > 200)
    duration = 200;
  head_ultrasound = duration;

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
        break;
      case 1:
        chestRight = currentTotalReading;
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

void longRangeInfraredSensorPoll()
{
  int infraredPinIndex = 0;
  float v = 0;
  float currentTotalReading = 0; //This thing is not just currentTotalReading, it's also used to hold the average and the return value.
  while (infraredPinIndex < 1)
  {  
    if (infraredPinIndex == 0){
    currentTotalReading=analogRead(sensorPin0);
    }
//    
//    if (infraredPinIndex == 1){
//    currentTotalReading=analogRead(sensorPin1);
//    }

    v=(float)currentTotalReading*5.0/1024.0;
 
   currentTotalReading = -270.79*pow(v, 5) + 3289.7*pow(v, 4) - 15920*pow(v, 3) + 38434*pow(v, 2) - 46483*v + 22797;
    if (currentTotalReading > 320)  //Limit the return value of the sensor.
   {
     currentTotalReading = 320;
   } 
   else if (currentTotalReading < 80)
   {
     currentTotalReading = 80;
   }
//   currentTotalReading -=80;
    switch(infraredPinIndex)
    {
      case 0:
        head_IR = currentTotalReading;
//        Serial.print("head_IR: ");
//        Serial.println(head_IR);
        break;
//      case 1:
//        waistCentre_IR = currentTotalReading;
//        Serial.print("waistCentre: ");
//        Serial.println(waistCentre_IR);
//        break;
    }
    
   currentTotalReading = 0;
   infraredPinIndex++; 
  }
  if(infraredPinIndex==2)
  {
    //Reset pins
    infraredPinIndex = 0;
  }   
}

void assignDistanceToData()
{
  sensorData[20]=head_ultrasound;
  sensorData[21]=head_IR;
  sensorData[22]=chestLeft;
  sensorData[23]=chestRight;
  sensorData[24]=waistCentre_IR;
}

int return_motor_strength(int distance, int type)
{
  float unit;
  int approxfreq = MIN_INTERVAL;
  int cmrange = 0;
  //Based on the distance detected by the sensor and the type of sensor, return the interval for the motor to vibrate at.
  //Sensors are expected to ONLY return values withi their ranges.

  if (type == 3) //Long-range IR, distance returned from 100 to 320 cm
  {
    cmrange = 220; //This may require an inverse of approxfreq - longer distances indicate obstacles or drops. Or I just use my brain
  }
  unit = (float)(MAX_INTERVAL - MIN_INTERVAL)/(float)cmrange;
  
  if (type == 3) {
  approxfreq = MAX_INTERVAL - (distance*unit);
  }  
  return approxfreq;
}

void longrangeIR_motordriver(){
 int longrangeHeadIR_delay = return_motor_strength(head_IR, 3);
 Serial.println(longrangeHeadIR_delay);
  unsigned long currentMillis = millis();

    if(currentMillis - previousHeadIRMillis > longrangeHeadIR_delay) {
      previousHeadIRMillis = currentMillis;   

      if (head_IR_State == LOW)
        head_IR_State = HIGH;
      else
        head_IR_State = LOW; 
        
      digitalWrite(5, head_IR_State);  
    }
}

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
//end of pedo

void setupConnection(){
      if (USART_available()){
            input = USART_read();
            //StrRxFlag = 0;
      } if(input == HANDSHAKE){
            USART_send(ACK);
           Serial.println("Sent ACK\n");
    
      } else if(input == ACK){
            Serial.write("---Connection Established---\n\n\n");
            connectionStatus = Connected;
            //flushRx();
      }
    
    input = -1;
}

void USART_init(void)
{
  UBRR0 = BAUD_PRESCALER;
  UCSR0C = ((0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)); //Set frame format: 8 data, 1 stop bit
  UCSR0B = ((1<<RXEN0)|(1<<TXEN0)); //Enable receiver and transmitter
}

//void USART_send(unsigned char data)
//{
//   //while transmit buffer not empty
//  while(!(UCSR1A & (1<<UDRE1)));
//   //When buffer is empty, write data to the transmitted.
//   digitalWrite(13, HIGH);
//   UDR1 = data;
//}

void USART_send(uint8_t value)
{
  char data = char(value);
   //while transmit buffer not empty
  while(!(UCSR1A & (1<<UDRE1)));
   //When buffer is empty, write data to the transmitted.
   digitalWrite(13, HIGH);
   UDR1 = data;
}

void USART_send_c(char value)
{
   //while transmit buffer not empty
  while(!(UCSR1A & (1<<UDRE1)));
   //When buffer is empty, write data to the transmitted.
   digitalWrite(13, HIGH);
   UDR1 = value;
}

int USART_read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (rx_head == rx_tail) {
    return -1;
  } else {
    unsigned char c = buffer[rx_head];
    rx_head = (unsigned int)(rx_head + 1) % RX_BUFFER_SIZE;
    return c;
  }
}

int USART_available(void)
{
  return (unsigned int)(RX_BUFFER_SIZE + rx_head - rx_tail) % RX_BUFFER_SIZE;
}

void flushRx()
{
  while(USART_available()){
    USART_read();
  }
}

void USART_putstring(char* StringPtr)
{
  while(*StringPtr != 0x00)
  {
      //USART_send(*StringPtr);
      StringPtr++;
  }
} //For sending whole string, e.g. USART_putstring("Hello!\n");

/*
unsigned char USART_Receive(void)
{
  while(!(UCSR0A & (1<<RXC0))); //Wait for data
  return UDR0; //Get and return data
}
*/

void USART_interrupt_init(void)
{
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  cli();
  UBRR1 = BAUD_PRESCALER; //Set baud rate prescaler
  UCSR1B = ((1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)); //Enable receiver/transmitter and RX interrupt
 // UCSR1C = ((0<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)); //Set frame format: 8 data, 1 stop bit.
  sei();
}

ISR(USART1_RX_vect)
{
  //digitalWrite(12, HIGH);
  
    //if terminator detected
    
    //buffer[i-1]=0x00; //Set string terminator to 0x00
    //i=0; //Reset buffer index
    if (bit_is_clear(UCSR1A, UPE1)) {
      buffer[rx_tail] = UDR1;
      rx_tail = (rx_tail+1)%RX_BUFFER_SIZE;
      StrRxFlag=1;  //Set string received flag
    } else {
      unsigned char c = UDR1;
    };

}

void sendSensorValue(uint8_t index){
    uint8_t value = sensorDatacopy[index];
    USART_send(value);
   // Serial.print(char(sensorDatacopy[index]+48));
   // Serial.print(" Sensor data sent\n");
   // flushRx();
}

int computeChecksumSensor(){
    uint8_t checkSum = 0;
    uint8_t remainder = 0;
    uint8_t index = 0;
    
    for(index = 0; index < sensor_Len; index++){
        remainder = sensorDatacopy[index] % divisor;
        checkSum = checkSum + remainder;
    }
    
    return checkSum;
}

void sendCheckSum(int8_t checkSum){
   USART_send(checkSum);
   
   int temp = checkSum;
   
   int ones = temp % 10;
   temp = temp/10;
   int tens = temp % 10;
   
//   Serial.write("CheckSum ");
//   Serial.write(char(tens+48));
//   Serial.write(char(ones+48));
//   Serial.write(" Sent\n");
}

uint8_t computeChecksumKeypad(){
    uint8_t checkSum = 0;
    uint8_t remainder = 0;
    uint8_t index = 0;
    
    for(index = 0; index < key_size; index++){
        remainder = (uint8_t)(parser_buffer_keys_copy[index]-48) % divisor;
        checkSum = checkSum + remainder;
    }
    
    return checkSum;
}

void copySensordata(){
  int i=0;
  for(i=0; i<25; i++){
    sensorDatacopy[i] = sensorData[i];
    sensorData[i] = 0;
  }
  compass_index = 1;
}

void UART(){
    uint8_t index = 0;
  switch(connectionStatus){
      
        case notConnected :{
             setupConnection();
             break;
        }
        
        case Connected : {
            
//            if(ActivateVoiceRpiFlag == 1){
//              USART_send(VOICE);
//              ActivateVoiceRpiFlag = 0;
//            }
//            else if (enter_pressed == 1){
//              USART_send(NUM);
//              enter = 1;
//              enter_pressed = 0;
//            }
          
            if( USART_available() ){
                input = USART_read();
            }
            uint8_t dataByte = input;
            input = 0;
            switch(dataByte){
                case HANDSHAKE   : {
                      connectionStatus = notConnected; 
                      input = HANDSHAKE;
                      break;
                }
                case ACK_NUM:{
                  USART_send(key_size);
                  for(index = 0; index < key_size; index++){
                      USART_send_c(parser_buffer_keys_copy[index]);
                    //  parser_buffer_keys_copy[index] = 0;
                  }
                  USART_send(computeChecksumKeypad());
                  enter = 0;
                  break;
                }
                case ACK_VOICE:{
                  break;
                }
                case NACK_NUM:{
                  USART_send(NUM);
                  break;
                }
                case ACK_PEDO:{
                  break;
                }
                case READ  : {
                     Serial.print("Received READ\n");
                      USART_send(ACK_READ);
                      Serial.print("Sent ACK_READ\n");
                      assignDistanceToData();
                      copySensordata();
                      sensorDatacopy[11] = (int8_t)(((int16_t)bearing)>>8);
                      sensorDatacopy[12] = (int8_t)((int16_t)bearing & 0xFF); 
                      sendSensorValue(0);
 
                      //flushRx();
                      checkSum = computeChecksumSensor();
                      //delay(1000);

                      Serial.print("Sent first value.\n");
                      break;
                }
                
                case ACK_S0 : sendSensorValue(1); break; 
                case ACK_S1 : sendSensorValue(2); break;
                case ACK_S2 : sendSensorValue(3); break;
                case ACK_S3 : sendSensorValue(4); break;
                case ACK_S4 : sendSensorValue(5); break;
                case ACK_S5 : sendSensorValue(6); break;
                case ACK_S6 : sendSensorValue(7); break;
                case ACK_S7 : sendSensorValue(8); break;
                case ACK_S8 : sendSensorValue(9); break;
                case ACK_S9 : sendSensorValue(10); break;
                case ACK_S10 : sendSensorValue(11); break;
                case ACK_S11 : sendSensorValue(12); break;
                case ACK_S12 : sendSensorValue(13); break;
                case ACK_S13 : sendSensorValue(14); break;
                case ACK_S14 : sendSensorValue(15); break;
                case ACK_S15 : sendSensorValue(16); break;
                case ACK_S16 : sendSensorValue(17); break;
                case ACK_S17 : sendSensorValue(18); break;
                case ACK_S18 : sendSensorValue(19); break;
                case ACK_S19 : sendSensorValue(20); break;
                case ACK_S20 : sendSensorValue(21); break;
                case ACK_S21 : sendSensorValue(22); break;
                case ACK_S22 : sendSensorValue(23); break;
                case ACK_S23 : sendSensorValue(24); break;
                case ACK_S24 : sendCheckSum(checkSum);  StrRxFlag = 0;   break;
                
                case ACK_CHECKSUM : break;
                
                case NACK_CHECKSUM: break;
                
            }
        }
        
  }
}

int steps(){
    int stepincrease = 0;
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
//              Serial.print("old:\t");
//              Serial.print(accx[count]); Serial.print("\t");
//              Serial.print("Threshold:\t");
//              Serial.print(threshold); Serial.print("\t");
//              Serial.print("new:\t");
//              Serial.print(accx[count+1]); Serial.print("\t");
//              Serial.print("Steps:\t");
//              Serial.print(stepcount);  Serial.print("\t"); 
            if (count == 49){
              count =0;
            }
            if (diff > precisionx && accx[count] > threshold && accx[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisionx && accx[count] < threshold && accx[count+1] > threshold && (timeElapsed - prevtime) <= 2000 && (timeElapsed-prevtime) >= 400){ //subsequent steps
             stepcount++;
             //Serial.println("Returning stepincrease for x.\n");
             //Serial.println(stepcount);
             stepincrease = 1;
             prevtime = 0;  
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
          }
    else if (axis == 1 & ay_count != 1){ //active axis is y
            
            diff = accy[count] - accy[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
//              Serial.print("old:\t");
//              Serial.print(accy[count]); Serial.print("\t");
//              Serial.print("Threshold:\t");
//              Serial.print(threshold); Serial.print("\t");
//              Serial.print("new:\t");
//              Serial.print(accy[count+1]); Serial.print("\t");
//              Serial.print("Steps:\t");
//              Serial.print(stepcount);  Serial.print("\t");
            if (count == 49){
              count =0;
            }
           if (diff > precisiony && accy[count] > threshold && accy[count+1] < threshold && prevtime == 0 ){ //first instance of a step 
              prevtime = timeElapsed;
            }
            if (diff > precisiony && accy[count] > threshold && accy[count+1] < threshold && (timeElapsed - prevtime) <= 2000 && (timeElapsed-prevtime) >= 400){ //subsequent steps
             stepcount++;
             stepincrease = 1;
             //Serial.println("Returning stepincrease for y.\n");
             //Serial.println(stepcount);
            prevtime = 0; 
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
    }
    else if (axis == 2 && az_count != 1){ //active axis is z
          diff = accz[count] - accz[count+1]; // sample old - sample new
            diff = abs(diff);
            count++;
//              Serial.print("old:\t");
//              Serial.print(accz[count]); Serial.print("\t");
//              Serial.print("Threshold:\t");
//              Serial.print(threshold); Serial.print("\t");
//              Serial.print("new:\t");
//              Serial.print(accz[count+1]); Serial.print("\t");
//              Serial.print("Steps:\t");
//              Serial.print(stepcount);  Serial.print("\t");

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
            if (diff > precisionz && accz[count] > threshold && accz[count+1] < threshold && (timeElapsed - prevtime) <= 2000 && (timeElapsed-prevtime) >= 400){ //subsequent steps
             stepcount++;
            // Serial.println("Returning stepincrease for z.\n");
             //Serial.println(stepcount);
             stepincrease = 1;
            prevtime = 0; 
             }
            if ((timeElapsed-prevtime) > 2000){
            prevtime=0; //reset time
            }
          }
 }
 return stepincrease;
}

int stepdifference = 0;
int lastrecordedcount = 0;

//void freeze_step_data()
//{
// stepdifference = stepcount - lastrecordedcount; 
// lastrecordedcount = stepcount;
// compass_index = 1;
// //Assign step data here.
// sensorData[0] = (int8_t)stepdifference; //#yq
//}

void copy_sensordata(){
  int i=0;
  for(i=0; i<25; i++){
    sensorDatacopy[i] = sensorData[i];
    sensorData[i] = 0;
  }
}

void get_Keypad_input(){
  ActivatepedoFlag = 0;
  ActivateVoiceRpiFlag = 0; //This represents the entirety of the function that sends data to the Rpi and resets the flag
  EnterFlag = 0;
  if(debounceFlag == 0){
    key = keypad.getKey();
    debounceFlag = 1;
    keypad.setDebounceTime(10);    
  }
  if(debounceFlag == 1) {
    debounceFlag = 0; //bounce finish
    if (key != NO_KEY){  
      if (key == '*'){
        Serial.println("case *");
        buffer_keys[buffer_keys_count-1]= '?';
        buffer_keys_count--;
      } 
      if (key == '#'){
        Serial.println("case #");
        for(counter = 0; counter < array_size; counter++)
          parser_buffer_keys[counter] = buffer_keys[counter];     
        for(counter = 0; counter <array_size; counter++)
         // Serial.println(parser_buffer_keys[counter]); 
        for (counter= 0; counter< array_size; counter++)
          buffer_keys[counter] = '?';  
        key_size = buffer_keys_count;
        buffer_keys_count = 0;
        EnterFlag = 1;
        Serial.print("EnterFlag: ");
        Serial.println(EnterFlag);
      }
    }         
    if( key != NO_KEY && key!= '*' && key!= '#'){   
     // if (key == '5'){
      if (key == '5'|| key == '1'){
        timeElapsed = 0;
        while (timeElapsed < interval_key5 ){
          key_state = keypad.getKeyState();
          if((key_state == 3 && prev_key_state == 1)){ 
           // activate_voice = 0;
            if (key == '5'){
              activate_voice = 0;
            }
            else if (key == '3'){
              activate_pedo = 0;
            }            
            break;
          } 
          prev_key_state = key_state;
        }
         
//        if (key_state == 1 && prev_key_state == 1){
//          activate_voice = 1;
//          ActivateVoiceRpiFlag = 1; //The sending function will set this flag to 0 when it is sent
//        }    
//        if (activate_voice == 0){
//          buffer_keys[buffer_keys_count] = key;
//          buffer_keys_count = (buffer_keys_count + 1)%(array_size) ;
//        }

         if (key_state == 1 && prev_key_state == 1 && key == '5'){
          activate_voice = 1;
          ActivateVoiceRpiFlag = 1; //The sending function will set this flag to 0 when it is sent
        }
        
          if (key_state == 1 && prev_key_state == 1 && key == '1'){
          activate_pedo = 1;
          ActivatepedoFlag = 1; //The sending function will set this flag to 0 when it is sent
        }
 
        if (activate_pedo == 0 && key == '1'){
          buffer_keys[buffer_keys_count] = '1';
          buffer_keys_count = (buffer_keys_count + 1)%(array_size) ;
        }
        else if (activate_voice == 0 && key =='5'){
          buffer_keys[buffer_keys_count] = '5';
          buffer_keys_count = (buffer_keys_count + 1)%(array_size) ;
        }

        //Serial.println(key);
        //Serial.println("testing values :");
        //Serial.print("activate_pedo : "); //set some other flag to send data to RPi
        //Serial.println(activate_pedo);
        
        //Serial.print("activate_voice : "); //set some other flag to send data to RPi
        //Serial.println(activate_voice);
        
        
        if (activate_voice == 1){
          activate_voice = 0;
        }
        if (activate_pedo == 1){
          activate_pedo = 0;
        }
        prev_key_state = key_state;
        key = '?';       
        
      }    
      else{   
        //Serial.println(key);
        buffer_keys[buffer_keys_count] = key;
        buffer_keys_count = (buffer_keys_count + 1)%(array_size);      
      }
     }  
   }
}

void setup(){
  USART_interrupt_init();
  Serial.begin(9600);
  Wire.begin();
  
  //pedo
    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    accelgyro.setI2CMasterModeEnabled(0);
    accelgyro.setI2CBypassEnabled(1);

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
  
  //keypad
  int a;
  for( a = 0; a<array_size; a++)
    buffer_keys[a] = '?';

  pinMode(sensorPin0, INPUT); 
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  
  //motor for head
  pinMode(5, OUTPUT);
}

void loop()
{
   //Serial.print("doing steps: \n");
  int step_count = steps();
 int i=0;
//  Serial.print("Steps:\t");
//  Serial.print(stepcount);  Serial.print("\t");
// Serial.print("Threshold:\t");
//Serial.print(threshold); Serial.print("\t");
 //   Serial.print("Axis:\t");
 // Serial.print(az_count);
//  Serial.print("\t\n");
  
  //compass
//   Serial.print("compass_x_offset = ");
//  Serial.println(compass_x_offset);
//  Serial.print("compass_y_offset = ");
//  Serial.println(compass_y_offset);
//  Serial.print("compass_z_offset = ");
//  Serial.println(compass_z_offset);
//  Serial.println();
// 
  compass_scalled_reading(); 
//  Serial.print("x = ");
//  Serial.println(compass_x_scalled);
//  Serial.print("y = ");
//  Serial.println(compass_y_scalled);
//  Serial.print("z = ");
//  Serial.println(compass_z_scalled);
//
  compass_heading();
//  Serial.print ("Heading angle = ");
//  Serial.print (bearing);
//  Serial.println(" Degree");
//  Serial.println();
 
 //keypad
 get_Keypad_input();
 
  if(ActivateVoiceRpiFlag == 1){
    USART_send(VOICE);
    //Serial.print("VOICE sent\n");
    ActivateVoiceRpiFlag = 0;
  }
  else if (EnterFlag == 1){
    USART_send(NUM);
    //Serial.print("NUM sent\n");
    //Serial.println(buffer_keys_count);
    //Serial.println(key_size);
    for(i=0; i<(int)key_size; i++){
      parser_buffer_keys_copy[i] = parser_buffer_keys[i];
     // Serial.println((char)parser_buffer_keys_copy[i]);
    }
    enter = 1;
  }
 
 if(compass_index <10 && step_count == 1){
   sensorData[compass_index++] = (int8_t)(((int16_t)bearing)>>8); //MSB
   sensorData[compass_index++] = (int8_t)((int16_t)bearing & 0xFF); //LSB
   sensorData[0]++;
   //Serial.println(sensorData[0]);
   step_count = 0;
}
 
 if (compass_index == 10){
   compass_index = 1;
 }
  longRangeInfraredSensorPoll();
   longrangeIR_motordriver();
 if(StrRxFlag == 1)
 {
   UART();
 }
}


