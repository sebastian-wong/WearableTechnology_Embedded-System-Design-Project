#include <time.h>

uint8_t incomingByte = 0;   // for incoming serial data
uint8_t sensorLen    = 19;
uint8_t actuatorLen  = 8;
uint8_t divisor      = 17;

enum STATE {
    newConnection,
    establishedConnection
} connectionSTATE;

enum STATE connectionState = newConnection;

static uint8_t const SYN       = 1;
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
static uint8_t const ACK_S10 = 20;
static uint8_t const ACK_S11 = 21;
static uint8_t const ACK_S12 = 22;
static uint8_t const ACK_S13 = 23;
static uint8_t const ACK_S14 = 24;
static uint8_t const ACK_S15 = 25;
static uint8_t const ACK_S16 = 26;
static uint8_t const ACK_S17 = 27;
static uint8_t const ACK_S18 = 28;

//Actuator Acknowlegements 
static uint8_t const  ACK_A0 = 30;
static uint8_t const  ACK_A1 = 31;
static uint8_t const  ACK_A2 = 32;
static uint8_t const  ACK_A3 = 33;
static uint8_t const  ACK_A4 = 34;
static uint8_t const  ACK_A5 = 35;
static uint8_t const  ACK_A6 = 36;
static uint8_t const  ACK_A7 = 37;
static uint8_t const  ACK_A_SUCCESS = 240;
static uint8_t const  ACK_A_FAILURE = 241;


//Buffer storing all sensor values - Index of buffer represents id of sensor 
uint8_t sensorData[19]   = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118};

//Buffer storing all actuator values - Index of buffer represents id of actuator
uint8_t actuatorData[10] = {200, 201, 202, 203, 204, 205, 206, 207, 208, 209};

//Temporary actuator buffer used when reading actuator values
uint8_t actuatorDataTemp[10] = {0};


int dataCorrupted = -1;


void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        Serial1.begin(9600);
}

void sendToRpi(uint8_t value){
        Serial1.write(char (value));
}

void setupConnection(){
      if (Serial1.available()){
            incomingByte = Serial1.read();
            
      } if(incomingByte == SYN){
            Serial.write("Received SYN\n");
            sendToRpi(ACK);
            Serial.write("Sent ACK\n");
    
      } else if(incomingByte == ACK){
            Serial.write("Received ACK\n");
            Serial.write("---Connection Established---\n\n\n");
            connectionState = establishedConnection;
      }
    
    incomingByte = -1;
}

void sendSensorValue(uint8_t index){
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
   
   Serial.write("CheckSum ");
   Serial.write(char(tens+48));
   Serial.write(char(ones+48));
   Serial.write(" Sent\n");
}

void printCheckSum(uint8_t checksum){   
   int temp = checksum;
   
   int ones = temp % 10;
   temp = temp/10;
   int tens = temp % 10;
   
   Serial.write("CheckSum ");
   Serial.write(char(tens+48));
   Serial.write(char(ones+48));
   Serial.write(" received\n");
   
   uint8_t checkSumNew = computeChecksumActuator();
   temp = checkSumNew;
   ones = temp % 10;
   temp = temp/10;
   tens = temp % 10;
   
   Serial.write("CheckSum new ");
   Serial.write(char(tens+48));
   Serial.write(char(ones+48));
   Serial.write(" calculated\n");
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

void storeTempActuatorData(uint8_t incomingByte, uint8_t index){
    actuatorDataTemp[index] = incomingByte;  
    Serial.print(incomingByte);
    Serial.print("\n");
}

void sendActuatorAck(uint8_t index){
    switch (index){
        case 0: sendToRpi(ACK_A0); Serial.write("ACK_A0 sent\n");break;
        case 1: sendToRpi(ACK_A1); Serial.write("ACK_A1 sent\n");break;
        case 2: sendToRpi(ACK_A2); Serial.write("ACK_A2 sent\n");break;
        case 3: sendToRpi(ACK_A3); Serial.write("ACK_A3 sent\n");break;
        case 4: sendToRpi(ACK_A4); Serial.write("ACK_A4 sent\n"); break;
        case 5: sendToRpi(ACK_A5); Serial.write("ACK_A5 sent\n"); break;
        case 6: sendToRpi(ACK_A6); Serial.write("ACK_A6 sent\n"); break;
        case 7: sendToRpi(ACK_A7); Serial.write("ACK_A7 sent\n"); break;
        case 8: sendToRpi(ACK_A_SUCCESS); Serial.write("ACK_A_SUCCESS sent\n"); break;
        case 9: sendToRpi(ACK_A_FAILURE); Serial.write("ACK_A_FAILURE sent\n"); break;
    }
}

int verifyCheckSum(uint8_t checkSumOld){
    uint8_t checkSumNew = computeChecksumActuator();
    if(checkSumOld == checkSumNew)
       return 1;
       
    return 0;
}


void storeActuatorData(){
    int index = 0;
    for (index = 0; index < actuatorLen; index++){
        actuatorData[index] = actuatorDataTemp[index];
        actuatorDataTemp[index] = 0;
    }
}

int computeChecksumActuator(){
    uint8_t checkSum = 0;
    uint8_t remainder = 0;
    uint8_t index = 0;
    
    for(index = 0; index < actuatorLen; index++){
        remainder = actuatorDataTemp[index] % divisor;
        checkSum = checkSum + remainder;
    }
    
    return checkSum;
}



int retrieveDataCorruption(){
    int Status = dataCorrupted;
    dataCorrupted = -1;
    return Status;
} 


void loop() {
        uint8_t index = 0;
        switch(connectionState){
            
              case newConnection :{
                   setupConnection();
                   break;
              }
              
              case establishedConnection : {
                  
                  if( Serial1.available() ){
                      incomingByte = Serial1.read();
                  }
                  uint8_t dataByte = incomingByte;
                  incomingByte = 0;
                  
                  switch(dataByte){
                      case SYN   : {
                            connectionState = newConnection; 
                            incomingByte = SYN;
                            break;
                      }
                      case READ  : {
                            Serial.print("Received READ\n");
                            sendToRpi(ACK_READ);
                            Serial.print("Sent ACK_READ\n");
                            delay(1000);
                            sendSensorValue(0);
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
                      case ACK_S18 : sendCheckSum();     break;
                      
                      case ACK_CHECKSUM : break;
                      
                      case WRITE : {
                            sendToRpi(ACK_WRITE);
                            while (index != actuatorLen+1){
                                  Serial.write("Rpi writing...\n");
                                 if( Serial1.available() ){
                                     incomingByte = Serial1.read();
                                     if(incomingByte)
                                     {
                                     if(index < actuatorLen){ 
                                         storeTempActuatorData(incomingByte, index);
                                         Serial.write(char(index+48));
                                         Serial.write("\n");
                                         sendActuatorAck(index);
                                         
                       
                                    } else if(index == actuatorLen) {
                                         uint8_t checkSum = incomingByte;
                                         printCheckSum(checkSum);
                                          if(verifyCheckSum(checkSum)){
                                              sendActuatorAck(8);
                                              storeActuatorData();
                                          }
                                          else{
                                            sendActuatorAck(9);
                                          }
                                    }
                                    index++;
                                    incomingByte = 0;
                                     }
                                 }  
                            }
                            
                            break;
                      }
                  }
              }
              
        }
}
