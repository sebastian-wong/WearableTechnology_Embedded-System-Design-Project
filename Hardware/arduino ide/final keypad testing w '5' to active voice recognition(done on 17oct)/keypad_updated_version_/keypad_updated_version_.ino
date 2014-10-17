#include <Keypad.h>
#include <Arduino.h>
#include <elapsedMillis.h>

#define array_size 100 

elapsedMillis timeElapsed;

//for key 5

int check_key = 0; //return respond if 5 was pressed to activate voice recognition 
elapsedMillis timeElapsed_in_case5; 
unsigned int interval_key5 = 500;
KeyState key_state = IDLE ;
KeyState prev_key_state = IDLE ;




int debounceFlag = 0;

int counter = 0; //used in loops in various cases 

// delay in milliseconds
unsigned int interval = 1500;


int mapToOutput [12] = { 0 }; // 0 - 9 map to mapToOutput 0-9 , * - 10 , # - 11

char key = '-'; //invalid input from keypad
char prev_key = '-'; 
char print_key = '-';

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
    for( a = 0; a<array_size; a++){
         buffer_keys[a] = '-';
         parser_buffer_keys[a] = '-';
    }
}

void loop(){
 
    if(debounceFlag == 0){
      key = keypad.getKey();
      debounceFlag = 1;
      keypad.setDebounceTime(10); 
    }
    if(debounceFlag == 1) {
      debounceFlag = 0; //bounce finish
      key_state = keypad.getKeyState();
      
      //to activate voice recognition 
      if (key != NO_KEY && key == '5' && key_state == 1){
//        Serial.println("i am here");
        prev_key_state = key_state;
//        Serial.println("prev_key_state: ");
//        Serial.println(prev_key_state);
        timeElapsed = 0;
        while (timeElapsed < interval_key5 ){
         key_state = keypad.getKeyState();
         Serial.println("key_state: ");
         Serial.println(key_state);
         
         Serial.println("prev_key_state: ");
         Serial.println(prev_key_state);
         if (key_state == 1 && prev_key_state == 1){
            Serial.println("button hold ");
         }
         else{
            Serial.println("button not hold ");
            break;
         }
         prev_key_state = key_state;
               
        } 
      
        if(timeElapsed >= interval_key5 && (key_state == 1 && prev_key_state == 1)){
          Serial.println("time to activate voice recognition");
          check_key = 1;
          key = '-';
          prev_key = '-';
          Serial.println(check_key);
        }     
      }
//        timeElapsed = 0;
//        prev_key_state = key_state;
//        while (timeElapsed < interval_key5){
//          key_state = keypad.getKeyState();
//          if(!(key_state == 2 && prev_key_state == 2)){
//            break;
//          }
//          prev_key_state = key_state;
//        }
//        if(timeElapsed >= interval_key5 && (key_state == 2 && prev_key_state == 2)){
//          Serial.println("time to activate voice recognition");
//        }
//      }       
      if (key != NO_KEY)
       checkKey();
    }

}
      
void checkKey(){
  switch (key) {    
    case '0':
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 0");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[0]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[0]++;
             Serial.print("mapToOutput[0] = ");
             Serial.print(mapToOutput[0]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;    
          }  
        
          debounceFlag = 0; 
        } 
     }
    
     
     Serial.println("after calculation");
     Serial.print("mapToOutput[0]: ");
     Serial.print(mapToOutput[0]);
     Serial.println();
     
     //check what to store in buffer 
     if(buffer_keys_count < array_size){
       Serial.println("buffer_keys_count < array_size");
       
       if((mapToOutput[0]) >= 3)
         mapToOutput[0] = (mapToOutput[0]%3) +1;
         
       if((mapToOutput[0]) == 1) 
         buffer_keys[buffer_keys_count] = '/';
       else if((mapToOutput[0]) == 2) 
         buffer_keys[buffer_keys_count] = '0';
     }
     
     Serial.print("buffer_keys[buffer_keys_count]: ");
     Serial.print(buffer_keys[buffer_keys_count]);
     Serial.println();

     buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
    
     Serial.println("after");
     Serial.print("buffer_keys_count: ");
     Serial.print(buffer_keys_count);
     Serial.println();
    
     
     mapToOutput[0] = 0; 
  
     break;  
 
    
    case '1':  
      Serial.print("before:");
      Serial.print("buffer_keys_count: ");
      Serial.print(buffer_keys_count);
      Serial.println();
      
      Serial.println("case 1"); 
      if(buffer_keys_count < array_size){
        buffer_keys[buffer_keys_count] = '1';
        buffer_keys_count = (buffer_keys_count + 1)% (array_size) ;     
      }
      prev_key = key;
      key = '-';
   
      Serial.print("after:");
      Serial.print("buffer_keys_count: ");
      Serial.print(buffer_keys_count);
      Serial.println();
      
      break;
   
    case '2': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 2");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[2]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[2]++;
             Serial.print("mapToOutput[2] = ");
             Serial.print(mapToOutput[2]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;    
          } 
        
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[2]: ");
      Serial.print(mapToOutput[2]);
      Serial.println();
     
     
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[2]) >= 5)
          mapToOutput[2] = (mapToOutput[2]%5) +1;
       
        if((mapToOutput[2]) == 1)
          buffer_keys[buffer_keys_count] = 'a'; 
        else if((mapToOutput[2]) == 2)
          buffer_keys[buffer_keys_count] = 'b';
        else if((mapToOutput[2]) == 3)
          buffer_keys[buffer_keys_count] = 'c';
        else if((mapToOutput[2]) == 4)
          buffer_keys[buffer_keys_count] = '2';
      }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

      buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
     
      Serial.println("after");
      Serial.print("buffer_keys_count: ");
      Serial.print(buffer_keys_count);
      Serial.println();
    
     
      mapToOutput[2] = 0;
  
      break;  
    
    case '3': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 3");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[3]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[3]++;
             Serial.print("mapToOutput[3] = ");
             Serial.print(mapToOutput[3]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;    
          } 
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[3]: ");
      Serial.print(mapToOutput[3]);
      Serial.println();
     
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[3]) >= 5)
          mapToOutput[3] = (mapToOutput[3]%5) +1;
   
        if((mapToOutput[3]) == 1)
         buffer_keys[buffer_keys_count] = 'd'; 
       else if((mapToOutput[3]) == 2)
         buffer_keys[buffer_keys_count] = 'e';
       else if((mapToOutput[3]) == 3)
         buffer_keys[buffer_keys_count] = 'f';
       else if((mapToOutput[3]) == 4)
         buffer_keys[buffer_keys_count] = '3';
      }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

      buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
    
      Serial.println("after");
      Serial.print("buffer_keys_count: ");
      Serial.print(buffer_keys_count);
      Serial.println();
    
      mapToOutput[3] = 0; //reset
      break;  

    case '4': //key = 2 , prev_key
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 4");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[4]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[4]++;
             Serial.print("mapToOutput[4] = ");
             Serial.print(mapToOutput[4]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;  
          }  
        
          debounceFlag = 0; 
        } 
     }
    
     
     Serial.println("after calculation");
     Serial.print("mapToOutput[4]: ");
     Serial.print(mapToOutput[4]);
     Serial.println();
    
     if(buffer_keys_count < array_size){
       Serial.println("buffer_keys_count < array_size");
         
       if((mapToOutput[4]) >= 5){
         mapToOutput[4] = (mapToOutput[4]%5) +1;
       } 
       
       if((mapToOutput[4]) == 1)
         buffer_keys[buffer_keys_count] = 'g'; 
       else if((mapToOutput[4]) == 2)
         buffer_keys[buffer_keys_count] = 'h';
       else if((mapToOutput[4]) == 3)
         buffer_keys[buffer_keys_count] = 'i';
       else if((mapToOutput[4]) == 4)
         buffer_keys[buffer_keys_count] = '4';
     }
     
     Serial.print("buffer_keys[buffer_keys_count]: ");
     Serial.print(buffer_keys[buffer_keys_count]);
     Serial.println();

     buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
    
    Serial.println("after");
    Serial.print("buffer_keys_count: ");
    Serial.print(buffer_keys_count);
    Serial.println();
    
    mapToOutput[4] = 0; 
 
    break;  


    case '5': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 5");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[5]++;
      timeElapsed = 0;

      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[5]++;
             Serial.print("mapToOutput[5] = ");
             Serial.print(mapToOutput[5]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;  
          } 
        
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[5]: ");
      Serial.print(mapToOutput[5]);
      Serial.println();
    
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[5]) >= 5)
          mapToOutput[5] = (mapToOutput[5]%5) +1;
          
        if((mapToOutput[5]) == 1)
          buffer_keys[buffer_keys_count] = 'j'; 
        else if((mapToOutput[5]) == 2)
          buffer_keys[buffer_keys_count] = 'k';
        else if((mapToOutput[5]) == 3)
          buffer_keys[buffer_keys_count] = 'l';
        else if((mapToOutput[5]) == 4)
          buffer_keys[buffer_keys_count] = '5';
      }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

      buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
     
     Serial.println("after");
     Serial.print("buffer_keys_count: ");
     Serial.print(buffer_keys_count);
     Serial.println();
       
     mapToOutput[5] = 0; 
     break;  


    case '6': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 6");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[6]++;
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[6]++;
             Serial.print("mapToOutput[6] = ");
             Serial.print(mapToOutput[6]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;    
          }  
        
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[6]: ");
      Serial.print(mapToOutput[6]);
      Serial.println();
     
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[6]) >= 5)
          mapToOutput[6] = (mapToOutput[6]%5) +1;

        if((mapToOutput[6]) == 1)
          buffer_keys[buffer_keys_count] = 'm'; 
        else if((mapToOutput[6]) == 2)
          buffer_keys[buffer_keys_count] = 'n';
        else if((mapToOutput[6]) == 3)
          buffer_keys[buffer_keys_count] = 'o';
        else if((mapToOutput[6]) == 4)
          buffer_keys[buffer_keys_count] = '6';
      }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

      buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
    
     Serial.println("after");
     Serial.print("buffer_keys_count: ");
     Serial.print(buffer_keys_count);
     Serial.println();

     mapToOutput[6] = 0; 
     break;  



    case '7': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 7");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[7]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[7]++;
             Serial.print("mapToOutput[7] = ");
             Serial.print(mapToOutput[7]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;    
          } 
        
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[7]: ");
      Serial.print(mapToOutput[7]);
      Serial.println();
     
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[7]) >= 6){
          mapToOutput[7] = (mapToOutput[7]%6) +1;
        } 
       
        if((mapToOutput[7]) == 1)
          buffer_keys[buffer_keys_count] = 'p'; 
        else if((mapToOutput[7]) == 2)
          buffer_keys[buffer_keys_count] = 'q';
        else if((mapToOutput[7]) == 3)
          buffer_keys[buffer_keys_count] = 'r';
        else if((mapToOutput[7]) == 4)
          buffer_keys[buffer_keys_count] = 's';
        else if((mapToOutput[7]) == 5)
          buffer_keys[buffer_keys_count] = '7';
      }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

      buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
    
     Serial.println("after");
     Serial.print("buffer_keys_count: ");
     Serial.print(buffer_keys_count);
     Serial.println();
    
     
     mapToOutput[7] = 0;
 
     break;  

    case '8': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 8");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[8]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[8]++;
             Serial.print("mapToOutput[8] = ");
             Serial.print(mapToOutput[8]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;    
          } 
        
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[8]: ");
      Serial.print(mapToOutput[8]);
      Serial.println();
     
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[8]) >= 5)
          mapToOutput[8] = (mapToOutput[8]%5) +1;
        
        if((mapToOutput[8]) == 1)
          buffer_keys[buffer_keys_count] = 't'; 
        else if((mapToOutput[8]) == 2)
          buffer_keys[buffer_keys_count] = 'u';
        else if((mapToOutput[8]) == 3)
          buffer_keys[buffer_keys_count] = 'v';
        else if((mapToOutput[8]) == 4)
          buffer_keys[buffer_keys_count] = '8';
      }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

      buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
    
     Serial.println("after");
     Serial.print("buffer_keys_count: ");
     Serial.print(buffer_keys_count);
     Serial.println();
    
     mapToOutput[8] = 0; 
     break;  


    case '9': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case 9");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[9]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[9]++;
             Serial.print("mapToOutput[9] = ");
             Serial.print(mapToOutput[9]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;    
          } 
        
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[9]: ");
      Serial.print(mapToOutput[9]);
      Serial.println();
   
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[9]) >= 6)
          mapToOutput[9] = (mapToOutput[9]%6) +1;
 
        if((mapToOutput[9]) == 1)
          buffer_keys[buffer_keys_count] = 'w'; 
        else if((mapToOutput[9]) == 2)
          buffer_keys[buffer_keys_count] = 'x';
        else if((mapToOutput[9]) == 3)
          buffer_keys[buffer_keys_count] = 'y';
        else if((mapToOutput[9]) == 4)
          buffer_keys[buffer_keys_count] = 'z';
        else if((mapToOutput[9]) == 5)
          buffer_keys[buffer_keys_count] = '9';
     }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

      buffer_keys_count = (buffer_keys_count + 1)%(array_size) ; 
    
     Serial.println("after");
     Serial.print("buffer_keys_count: ");
     Serial.print(buffer_keys_count);
     Serial.println();
     
     mapToOutput[9] = 0; 
     break;  
    
    
    // case * clear previous input  
    case '*': 
      Serial.println();
      Serial.println("before");
      Serial.print("buffer_keys_count: ");
      Serial.println(buffer_keys_count);

      Serial.println("case *");   
      Serial.println();
    
      prev_key = key;
      key = '-';
      mapToOutput[10]++; 
      timeElapsed = 0;


      while (timeElapsed < interval ){
        if(debounceFlag == 0){
          key = keypad.getKey();
          debounceFlag = 1;
          keypad.setDebounceTime(10);
        }
        if(debounceFlag == 1) { 
          if(key != NO_KEY){
           if(key == prev_key){
             timeElapsed = 0;
             mapToOutput[10]++;
             Serial.print("mapToOutput[10] = ");
             Serial.print(mapToOutput[10]);
             Serial.println();
             prev_key = key; 
             key = '-';  
           }
           else
             break;  
          } 
        
          debounceFlag = 0; 
        } 
      }
    
     
      Serial.println("after calculation");
      Serial.print("mapToOutput[10]: ");
      Serial.print(mapToOutput[10]);
      Serial.println();
     
      if(buffer_keys_count < array_size){
        Serial.println("buffer_keys_count < array_size");
         
        if((mapToOutput[10]) >= 3)
          mapToOutput[10] = (mapToOutput[10]%3)+1;

        if((mapToOutput[10]) == 1){ // clear one input
          if ( buffer_keys_count > 0 && buffer_keys_count <= (array_size-1)){
            buffer_keys[buffer_keys_count-1]= '-';
            buffer_keys_count--;
          }
        }  
        else if((mapToOutput[10]) == 2){ //clear all inputs
          for (counter= 0; counter< array_size; counter++)
            buffer_keys[counter] = '-';   
          buffer_keys_count = 0;   
        
        }
      }
     
      Serial.print("buffer_keys[buffer_keys_count]: ");
      Serial.print(buffer_keys[buffer_keys_count]);
      Serial.println();

    
     Serial.println("after");
     Serial.print("buffer_keys_count: ");
     Serial.print(buffer_keys_count);
     Serial.println();
    
     mapToOutput[10] = 0; 
     break;  

   
    // case # - enter all input in the buffer    
    case '#': 
      Serial.println("case #");   
       
      for(counter = 0; counter < array_size; counter++){
        parser_buffer_keys[counter] = buffer_keys[counter];
       }
               
      for(counter = 0; counter <array_size; counter++)
        Serial.println(parser_buffer_keys[counter]); 
       
       
      for (counter= 0; counter< array_size; counter++)
        buffer_keys[counter] = '-';

   
 
      prev_key = key;
      key = '-';
      break;  
    } 
 }



