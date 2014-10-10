#include <time.h>

uint8_t in_Data = 0;   // for incoming serial data
uint8_t sensorLen    = 10;
uint8_t act_len  = 10;
uint8_t divisor      = 17;

enum STATE {
    newConnection,
    Connected
} status_connection;

enum STATE _status = newConnection;

static uint8_t const HAND       = 1;
static uint8_t const ACK       = 2;
static uint8_t const NAK       = 3;
static uint8_t const READ      = 4;
static uint8_t const ACK_READ  = 5;
static uint8_t const WRITE     = 6;
static uint8_t const ACK_WRITE = 7;
static uint8_t const ACK_CHECKSUM = 8;

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

//Actuator Acknowlegements 
//static uint8_t const  ACK_A0 = 20;
//static uint8_t const  ACK_A1 = 21;
//static uint8_t const  ACK_A2 = 22;
//static uint8_t const  ACK_A3 = 23;
//static uint8_t const  ACK_A4 = 24;
//static uint8_t const  ACK_A5 = 25;
//static uint8_t const  ACK_A6 = 26;
//static uint8_t const  ACK_A7 = 27;
//static uint8_t const  ACK_A8 = 28;
//static uint8_t const  ACK_A9 = 29;

static uint8_t const ACK_A[10] = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
//static uint8_t const ACK_S[10] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

//Buffer storing all sensor values - Index of buffer represents id of sensor 
uint8_t sensorData[10]   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

//Buffer storing all actuator values - Index of buffer represents id of actuator
uint8_t actuatorData[10] = {20, 21, 22, 23, 24, 25, 26, 27, 208, 209};

//Temporary actuator buffer used when reading actuator values
uint8_t actuatorDataTemp[10] = {0};


void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        Serial1.begin(9600);
}

void sendToRpi(uint8_t value){
        Serial1.write(char (value));
}

void setupConnection(){
      if (Serial1.available()){
            in_Data = Serial1.read();
            
      } if(in_Data == HAND){
            Serial.write("HAND Received\n");
            sendToRpi(ACK);
            Serial.write("ACK Sent\n");
    
      } else if(in_Data == ACK){
            Serial.write("ACK Received\n");
            Serial.write("---Connected---\n\n\n");
            _status = Connected;
      }
    
    in_Data = -1;
}

void send_sensorVal(uint8_t index){
    uint8_t value = sensorData[index];
    sendToRpi(value);
    Serial.print(char(index+48));
    Serial.print(" Sensor data sent\n");
}

void sendCheckSum(){
   uint8_t checkSum = computeChecksumSensor();
   sendToRpi(checkSum);
   
   int temp = checkSum;
   
   int ones = temp % 10;
   temp = temp/10;
   int tens = temp % 10;
}

int computeChecksumSensor(){
    uint8_t checkSum = 0;
    uint8_t remainder = 0;
    uint8_t index = 0;
    
    for(index = 0; index < sensorLen; index++){
        remainder = sensorData[index] % divisor;
        checkSum = checkSum + remainder;
    }
    
    return checkSum;
}

void storeTempActuatorData(uint8_t in_Data, uint8_t index){
    actuatorDataTemp[index] = in_Data;  
    Serial.print(in_Data);
    Serial.print("\n");
}

//void sendActuatorAck(uint8_t index){
//    sendToRpi(ACK_A[index]);
//}

int verifyCheckSum(uint8_t checkSumOld){
    uint8_t checkSumNew = computeChecksumActuator();
    if(checkSumOld == checkSumNew)
       return 1;
       
    return 0;
}


void storeActuatorData(){
    int index = 0;
    for (index = 0; index < act_len; index++){
        actuatorData[index] = actuatorDataTemp[index];
        actuatorDataTemp[index] = 0;
    }
}

int computeChecksumActuator(){
    uint8_t checkSum = 0;
    uint8_t remainder = 0;
    uint8_t index = 0;
    
    for(index = 0; index < act_len; index++){
        remainder = actuatorDataTemp[index] % divisor;
        checkSum = checkSum + remainder;
    }
    
    return checkSum;
}

void loop() {
        switch(_status){
            
              case newConnection :{
                   setupConnection();
                   break;
              }
              
              case Connected : {
                  
                  if( Serial1.available() ){
                      in_Data = Serial1.read();
                  }
                  uint8_t dataByte = in_Data;
                  in_Data = 0;
                  
                  switch(dataByte){
                      case HAND   : {
                            _status = newConnection; 
                            in_Data = HAND;
                            break;
                      }
                      case READ  : {
                            Serial.print("READ Received\n");
                            sendToRpi(ACK_READ);
                            Serial.print("ACK_READ Sent\n");
                            delay(1000);
                            send_sensorVal(0);
                            break;
                      }
                      
                      case ACK_S0 : send_sensorVal(1); break; 
                      case ACK_S1 : send_sensorVal(2); break;
                      case ACK_S2 : send_sensorVal(3); break;
                      case ACK_S3 : send_sensorVal(4); break;
                      case ACK_S4 : send_sensorVal(5); break;
                      case ACK_S5 : send_sensorVal(6); break;
                      case ACK_S6 : send_sensorVal(7); break;
                      case ACK_S7 : send_sensorVal(8); break;
                      case ACK_S8 : send_sensorVal(9); break;
                      case ACK_S9 : sendCheckSum();     break;
                      
                      case ACK_CHECKSUM : break;
                      
                      case WRITE : {
                            sendToRpi(WRITE);
                            int index = 0;
                            while (index != act_len+1){
                                 if( Serial1.available() ){
                                     in_Data = Serial1.read();
                                     if(index < act_len){
                                         storeTempActuatorData(in_Data, index);
                                         sendToRpi(ACK_A[index]);
                                         
                                    } else if(index == act_len) {
                                         uint8_t checkSum = in_Data;
                                       //  Serial.print(checkSum);
                                          if(verifyCheckSum(checkSum)){
                                              storeActuatorData();
                                          }
                                    }
                                    index++;
                                    in_Data = 0;
                                 }  
                            }
                            
                            break;
                      }
                  }
              }
              
        }
}
