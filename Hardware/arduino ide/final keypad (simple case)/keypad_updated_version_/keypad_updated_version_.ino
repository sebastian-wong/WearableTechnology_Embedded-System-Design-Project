#include <Keypad.h>
#include <Arduino.h>
#include <elapsedMillis.h>

#define array_size 100 

elapsedMillis timeElapsed;
int debounceFlag = 0;

int counter = 0; //used in loops in various cases 

// delay in milliseconds
unsigned int interval = 1500;


char key = '?'; //invalid input from keypad

char buffer_keys[array_size]; //temp buffer to store inputs from user
char parser_buffer_keys[array_size]; //used to retrieve inputs directly 
int buffer_keys_count = 0;

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
 
    if(debounceFlag == 0){
      key = keypad.getKey();
      debounceFlag = 1;
      keypad.setDebounceTime(10);    
    }
    if(debounceFlag == 1) {
      debounceFlag = 0; //bounce finish
      if (key != NO_KEY)
  
        if (key == '*'){
          buffer_keys[buffer_keys_count-1]= '?';
          buffer_keys_count--;
       
        }
        
        else if (key == '#'){
          for(counter = 0; counter < array_size; counter++)
            parser_buffer_keys[counter] = buffer_keys[counter];
               
          for(counter = 0; counter <array_size; counter++)
            Serial.println(parser_buffer_keys[counter]); 
       
       
          for (counter= 0; counter< array_size; counter++)
            buffer_keys[counter] = '?';

        }
       
        else{      
          Serial.println(key);
          buffer_keys[buffer_keys_count] = key;
          buffer_keys_count = (buffer_keys_count + 1)%(array_size) ;
          //clear
        }  
    }


}
      




