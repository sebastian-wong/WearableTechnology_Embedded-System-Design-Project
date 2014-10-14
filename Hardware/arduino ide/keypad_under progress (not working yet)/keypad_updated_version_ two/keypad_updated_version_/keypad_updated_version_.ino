#include <Keypad.h>
#include <Arduino.h>
#include <elapsedMillis.h>


elapsedMillis timeElapsed;
int mapToOutput [12] = { 0 }; // 0 - 9 map to mapToOutput 0-9 , * - 10 , # - 11

// delay in milliseconds
unsigned int interval = 200;
char key = '-';
char prev_key = '-'; //invalid input
char buffer_keys[30] = {'.'};
char print_key = '-';
int buffer_keys_count = 0;

//int start = 0;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns


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
  key = keypad.getKey(); 
  if (key != NO_KEY){
   checkKey(); 
  } 

}
      
void checkKey(){
  
  switch (key) {
    
    case 1:  
      buffer_keys[buffer_keys_count] = '1';
      buffer_keys_count++;
      prev_key = key;
      Serial.print('1');
      break;
      
    case 2:
      mapToOutput[2]=( mapToOutput[2] % 4 )+1; //press once
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if( ( mapToOutput[2] % 4 )== '1')
               print_key = 'a';
             else if( ( mapToOutput[2] % 4 ) == '2')
               print_key = 'b';
             else if( ( mapToOutput[2] % 4 ) == '3')
               print_key = 'c';  
             else if( ( mapToOutput[2] % 4 ) == '4')
               print_key = '1';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[2]=( mapToOutput[2] % 4 )+1;
	 }
        
         else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;  
   
    case 3:  
      mapToOutput[3]=( mapToOutput[3] % 4 )+1;//press once
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if( ( mapToOutput[3] % 4 ) == '1')
               print_key = 'd';
             else if( ( mapToOutput[3] % 4 ) == '2')
               print_key = 'e';
             else if( ( mapToOutput[3] % 4 ) == '3')
               print_key = 'f';  
             else if( ( mapToOutput[3] % 4 ) == '4')
               print_key = '2';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[3]=( mapToOutput[3] % 4 )+1;    
	 }
        
         else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;  
      
    case 4:  
      mapToOutput[4]=( mapToOutput[4] % 4 )+1;
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if( ( mapToOutput[4] % 4 )== '1')
               print_key = 'g';
             else if( ( mapToOutput[4] % 4 ) == '2')
               print_key = 'h';
             else if( ( mapToOutput[4] % 4 ) == '3')
               print_key = 'i';  
             else if( ( mapToOutput[4] % 4 ) == '4')
               print_key = '4';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[4]=( mapToOutput[4] % 4 )+1;
	 }
        
         else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;  
    
     case 5:  
      mapToOutput[5]=( mapToOutput[5] % 4 )+1;
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if( ( mapToOutput[5] % 4 )== '1')
               print_key = 'j';
             else if( ( mapToOutput[5] % 4 ) == '2')
               print_key = 'k';
             else if( ( mapToOutput[5] % 4 ) == '3')
               print_key = 'l';  
             else if( ( mapToOutput[5] % 4 ) == '4')
               print_key = '5';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
          mapToOutput[5]=( mapToOutput[5] % 4 )+1; 
	 }
        
         else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;  
      
    case 6:  
      mapToOutput[6]=( mapToOutput[6] % 4 )+1;
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if(  ( mapToOutput[6] % 4 ) == '1')
               print_key = 'm';
             else if(  ( mapToOutput[6] % 4 ) == '2')
               print_key = 'n';
             else if(  ( mapToOutput[6] % 4 ) == '3')
               print_key = 'o';  
             else if(  ( mapToOutput[6] % 4 ) == '4')
               print_key = '6';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[6]=( mapToOutput[6] % 4 )+1; 
	 }
        
         else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;  
    
      
    case 7:  
      mapToOutput[7]=( mapToOutput[7] % 4 )+1;
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if(  ( mapToOutput[7] % 4 ) == '1')
               print_key = 'p';
             else if(  ( mapToOutput[7] % 4 ) == '2')
               print_key = 'q';
             else if(  ( mapToOutput[7] % 4 ) == '3')
               print_key = 'r';  
             else if(  ( mapToOutput[7] % 4 ) == '4')
               print_key = 's';
             else if(  ( mapToOutput[7] % 4 ) == '5')
               print_key = '7';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[7]=( mapToOutput[7] % 4 )+1;
	 }
        
         else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;  
       
      
    case 8:  
      mapToOutput[8]=( mapToOutput[8] % 4 )+1;
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if( ( mapToOutput[8] % 4 ) == '1')
               print_key = 't';
             else if( ( mapToOutput[8] % 4 ) == '2')
               print_key = 'u';
             else if( ( mapToOutput[8] % 4 ) == '3')
               print_key = 'v';  
             else if( ( mapToOutput[8] % 4 ) == '4')
               print_key = '8';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[8]=( mapToOutput[8] % 4 )+1; 
	 }
        
         else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;  
        
    case 9: 
      mapToOutput[9]=( mapToOutput[9] % 4 )+1;
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if( ( mapToOutput[9] % 4 ) == '1')
               print_key = 'w';
             else if( ( mapToOutput[9] % 4 ) == '2')
               print_key = 'x';
             else if( ( mapToOutput[9] % 4 ) == '3')
               print_key = 'y';  
             else if( ( mapToOutput[9] % 4 ) == '4')
               print_key = 'z';
             else if( ( mapToOutput[9] % 4 ) == '5')
               print_key = '9';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[9]=( mapToOutput[9] % 4 )+1;
	 }
        
        else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }
      //timeElapsed = 0;
      break;   

    case 0:  // press one time - spacebar ; press two time - '0' 
      mapToOutput[0]=( mapToOutput[0] % 4 )+1;
      prev_key = key; 
      timeElapsed = 0;
      while (timeElapsed < interval) 
	{				
	  key = keypad.getKey(); 
          if (key != NO_KEY && key != prev_key){  //key != prev_key
             if(  ( mapToOutput[0] % 4 ) == '1')
               print_key = ' ';
             else if(  ( mapToOutput[0] % 4 ) == '2')
               print_key = '0';
             buffer_keys[buffer_keys_count]= print_key;
             buffer_keys_count++;
             break;
         }
         else if (key != NO_KEY && key == prev_key){//key == prev_key
           mapToOutput[0]=( mapToOutput[0] % 4 )+1;
	 }
        
        else //if (key == NO_KEY)
           break;
         
         prev_key = key; 
      }  
      //timeElapsed = 0;
      break;       
      
   //case # -enter & *  -clear previous input
    default: 
      int i = 0;
      if (key == '#'){
        //print out everything in the buffer
        while(buffer_keys[i] != '.'){
         Serial.println(buffer_keys[i]);
         i++;
        }
        //clear everything in the buffer
         while(buffer_keys[i] != '.'){
          buffer_keys[i] = '.';
          i++;
         }         
      }
      
      else if (key == '*'){
       buffer_keys[buffer_keys_count-1]= '.';
      } 
     prev_key = key; 
      break;
    }
  
}



