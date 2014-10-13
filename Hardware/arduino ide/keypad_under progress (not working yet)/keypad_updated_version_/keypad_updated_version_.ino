#include <Keypad.h>
#include <Arduino.h>


long fastLoopTimer = 0;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

int mapToOutput [8] = { 0 }; 


char prev_key = '0'; //invalid key
char print_key = '0';

char keys[ROWS][COLS] = {
  {'#','0','*'},
  {'9','8','7'},
  {'6','5','4'},
  {'3','2','1'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad // 2 - orange
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
}

void loop(){
  char key = keypad.getKey();  
//  if (key !=NO_KEY)
//    Serial.println(key);

     while(millis()-fastLoopTimer<=5000){
       //Serial.println(millis()-fastLoopTimer);
       fastLoopTimer=millis();
       prev_key = key;
       key = keypad.getKey();
       Serial.println(prev_key);
       Serial.println(key);
       
       
       if (key != NO_KEY){
         
         //key not equal to prev_key
          if (key != prev_key  && (key != '1' || key != '0' || key != '*'|| key != '#')){
            //check how many times prev_key was pressed and print the correct output for prev key
           checkKey();
           Serial.println(print_key);
           mapToOutput[prev_key-2] = 0;
          }
         
          else if (key == '*' && (prev_key != '1' || prev_key != '0' || prev_key != '*'|| prev_key != '#')){ //clear previous input
                mapToOutput[prev_key-2]--;  
          }
          
          else if (key == '#'){ //clear all inputs
             for(int i=0; i<8;i++){
                mapToOutput[i] = 0; 
              }  
          }
          
         else if (key == prev_key  && (key != '1' || key != '0' || key != '*'|| key != '#')){
            //check how many times it is pressed.
            mapToOutput[prev_key-2]++;
          }
               
       }
      
       if (key == NO_KEY && prev_key != NO_KEY){
         checkKey();
         Serial.println(print_key);
         mapToOutput[prev_key-2] = 0;
         
       }
      
       //Serial.println(millis()-fastLoopTimer);
     }  
     
}
    
   
void checkKey(){
  switch (prev_key) {
    case 2:  
      if (mapToOutput[prev_key-2] == '1'){
       print_key ='a';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'b';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'c';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = '1';    
      } 
      break;
      
    case 3:  
      if (mapToOutput[prev_key-2] == '1'){
       print_key ='d';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'e';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'f';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = '3';    
      } 
      break;     

    case 4:  
      if (mapToOutput[prev_key-2] == '1'){
       print_key ='g';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'h';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'i';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = '4';    
      } 
      break;
      
    case 5:  
      if (mapToOutput[prev_key-2] == '1'){
       print_key ='j';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'k';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'l';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = '5';    
      } 
      break;
      
    case 6:  
      if (mapToOutput[prev_key-2] == '1'){
       print_key ='m';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'n';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'o';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = '6';    
      } 
      break;
      
    case 7:  
      if (mapToOutput[prev_key-2] == '1'){
       print_key ='p';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'q';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'r';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = 's';    
      } 
      
      else if (mapToOutput[prev_key-2] == '5'){
       print_key = '7';    
      } 
      
      break;    
      
    case 8:  
       if (mapToOutput[prev_key-2] == '1'){
       print_key ='t';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'u';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'v';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = '8';    
      } 
      
      break;    
      
    case 9:  
       if (mapToOutput[prev_key-2] == '1'){
       print_key ='w';    
      }
      
      else if (mapToOutput[prev_key-2] == '2'){
       print_key = 'x';    
      }      
      
      else if (mapToOutput[prev_key-2] == '3'){
       print_key = 'y';    
      }
      
      else if (mapToOutput[prev_key-2] == '4'){
       print_key = 'z';    
      } 
      
      else if (mapToOutput[prev_key-2] == '5'){
       print_key = '9';    
      } 
      
      break;       
 
    default:  
      break;
    }
  
}



