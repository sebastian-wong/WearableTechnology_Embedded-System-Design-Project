#include <avr/interrupt.h>
#include <time.h>

#define F_CPU 16000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUDRATE * 16UL))) - 1)

uint8_t input = 0; //for incoming serial data
uint8_t sensor_Len = 19;
uint8_t divisor = 17;

//for UART1
static const int RX_BUFFER_SIZE = 20;
volatile unsigned char buffer[RX_BUFFER_SIZE];
volatile uint8_t StrRxFlag=0;
volatile unsigned int rx_head = 0, rx_tail = 0;



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
static uint8_t const NUM        = 9;
static uint8_t const ACK_NUM   = 40;
static uint8_t const NACK_NUM  = 41;
static uint8_t const ACK_NUM_CHECKSUM = 42;
static uint8_t const NACK_NUM_CHECKSUM = 43;
static uint8_t const VOICE     = 44;
static uint8_t const ACK_VOICE     = 45;

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

//Buffer storing all sensor values - Index of buffer represents id of sensor 
uint8_t sensorData[19]   = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118};

//to get from numpad file
volatile char parser_buffer_keys_copy[10] = {'1', '5', '7', '9'};
volatile int key_size = 4;
volatile uint8_t ActivateVoiceRpiFlag = 0;
volatile uint8_t enter_pressed = 1;
volatile uint8_t enter = 0;

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
  digitalWrite(12, HIGH);
  
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

void setupConnection(){
      if (USART_available()){
            input = USART_read();
            StrRxFlag = 0;
      } if(input == HANDSHAKE){
            USART_send(ACK);
//            Serial.write("Sent ACK\n");
    
      } else if(input == ACK){
//            Serial.write("---Connection Established---\n\n\n");
            connectionStatus = Connected;
      }
    
    input = -1;
}

void sendSensorValue(uint8_t index){
    uint8_t value = sensorData[index];
    USART_send(value);
//    Serial.print(char(index+48));
//    Serial.print(" Sensor data sent\n");
}

int computeChecksumSensor(){
    uint8_t checkSum = 0;
    uint8_t remainder = 0;
    uint8_t index = 0;
    
    for(index = 0; index < sensor_Len; index++){
        remainder = sensorData[index] % divisor;
        checkSum = checkSum + remainder;
    }
    
    return checkSum;
}

void sendCheckSum(){
   uint8_t checkSum = computeChecksumSensor();
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

void setup(){
  USART_interrupt_init();
}

void loop()
{
  
  uint8_t index = 0;
  switch(connectionStatus){
      
        case notConnected :{
             setupConnection();
             break;
        }
        
        case Connected : {
            
            if(ActivateVoiceRpiFlag == 1){
              USART_send(VOICE);
            }
            else if (enter_pressed == 1){
              USART_send(NUM);
              enter = 1;
              enter_pressed = 0;
            }
          
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
                case READ  : {
//                      Serial.print("Received READ\n");
                      USART_send(ACK_READ);
//                      Serial.print("Sent ACK_READ\n");
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
                
                }
            }
        }
  }
}
