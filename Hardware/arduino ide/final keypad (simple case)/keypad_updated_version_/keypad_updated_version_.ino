#include <Keypad.h>
#include <Arduino.h>
#include <elapsedMillis.h>

#define array_size 100 

elapsedMillis timeElapsed;
int debounceFlag = 0;
int ActivateVoiceRpiFlag = 0;
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





const byte ROWS = 4; 
const byte COLS = 3; 


char keys[ROWS][COLS] = {
  {'#','0','*'},
  {'9','8','7'},
  {'6','5','4'},
  {'3','2','1'}
};

//use in arduino mega 2560
//byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad // 2 - orange
//byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

//use in arduino uno
byte rowPins[ROWS] = {10, 9, 8, 7}; //connect to the row pinouts of the keypad 
byte colPins[COLS] = {13, 12, 11}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
  int a;
  for( a = 0; a<array_size; a++)
    buffer_keys[a] = '?';
}

void loop(){
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
          Serial.println(parser_buffer_keys[counter]); 
        for (counter= 0; counter< array_size; counter++)
          buffer_keys[counter] = '?';  
        buffer_keys_count = 0;
        EnterFlag = 1;
        Serial.print("EnterFlag: ");
        Serial.println(EnterFlag);
      }
    }         
    if( key != NO_KEY && key!= '*' && key!= '#'){   
      if (key == '5'){
        timeElapsed = 0;
        while (timeElapsed < interval_key5 ){
          key_state = keypad.getKeyState();
          if((key_state == 3 && prev_key_state == 1)){ 
            activate_voice = 0; 
            break;
          } 
          prev_key_state = key_state;
        }
         
        if (key_state == 1 && prev_key_state == 1){
          activate_voice = 1;
          ActivateVoiceRpiFlag = 1; //The sending function will set this flag to 0 when it is sent
        }    
        if (activate_voice == 0){
          buffer_keys[buffer_keys_count] = key;
          buffer_keys_count = (buffer_keys_count + 1)%(array_size) ;
        }

        Serial.print("activate_voice : "); //set some other flag to send data to RPi
        Serial.println(activate_voice);
        activate_voice = 0;
        prev_key_state = key_state;
        key = '?';       
        
      }    
      else{   
        Serial.println(key);
        buffer_keys[buffer_keys_count] = key;
        buffer_keys_count = (buffer_keys_count + 1)%(array_size);      
      }
     }  
   }


}
      




