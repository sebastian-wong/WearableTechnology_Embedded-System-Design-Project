// Use this code to test your motor with the Arduino board:

// if you need PWM, just use the PWM OUTPUTs on the Arduino
// and instead of digitalWrite, you should use the analogWrite command


//motor for the detection of the obstacles at close range

// --------------------------------------------------------------------------- Motors
//int motor_left[] = {2, 3};
//int motor_right[] = {7, 8};

//int motor_left[] = {53}; //arduino mega


#define DELAY_TIME 25 //will be later passed in as a variable from transalation file

int obstacle_left = 0;
int obstacle_right = 0;
int obstacle_forward = 0;
int obstacle_groundlvl = 0;

int motor_left[] = {8};
int motor_right[] = {9}; 
int motor_forward[] = {10}; //bottom at the waist area 
int motor_groundlvl[] = {11};

// --------------------------------------------------------------------------- Setup
void setup() {
  Serial.begin(9600);

// Setup motors
/**
int i;
for(i = 0; i < 2; i++){
pinMode(motor_left[i], OUTPUT);
pinMode(motor_right[i], OUTPUT);
}
**/

  int i;
  for(i = 0; i < 1; i++){
    pinMode(motor_left[i], OUTPUT);
    pinMode(motor_right[i], OUTPUT);
    pinMode(motor_forward[i], OUTPUT);
    pinMode(motor_groundlvl[i], OUTPUT);
  }
}
// --------------------------------------------------------------------------- Loop
void loop() {  //not needed. intepretation will be done in translation file
//  obstacle_left = 1;
//  if(obstacle_left){
//    turn_left();
//    delay(1000);
//    motor_stop();
//  }

  obstacle_left = 1;
  if (obstacle_left){
    turn_left();
    delay(1000);
  }

}
  
// --------------------------------------------------------------------------- Drive

//void motor_stop(){
//digitalWrite(motor_left[0], LOW); 
//digitalWrite(motor_right[0], LOW); 
//digitalWrite(motor_forward[0], LOW);
//digitalWrite(motor_groundlvl[0], LOW);
//delay(DELAY_TIME);
//}

void drive_forward(){
digitalWrite(motor_forward[0], HIGH);  
digitalWrite(motor_left[0], LOW); 
digitalWrite(motor_right[0], LOW); 
digitalWrite(motor_groundlvl[0], LOW); 

delay(DELAY_TIME);
digitalWrite(motor_forward[0], LOW);  

}

void drive_groundlvl(){
digitalWrite(motor_forward[0], LOW);  
digitalWrite(motor_left[0], LOW); 
digitalWrite(motor_right[0], LOW); 
digitalWrite(motor_groundlvl[0], HIGH);  

delay(DELAY_TIME);
digitalWrite(motor_groundlvl[0], LOW); 
}

void turn_left(){
digitalWrite(motor_forward[0], LOW);  
digitalWrite(motor_left[0], HIGH); 
digitalWrite(motor_right[0], LOW); 
digitalWrite(motor_groundlvl[0], LOW); 

delay(DELAY_TIME);
digitalWrite(motor_left[0], LOW); 
}

void turn_right(){
digitalWrite(motor_forward[0], LOW);  
digitalWrite(motor_left[0], LOW); 
digitalWrite(motor_right[0], HIGH); 
digitalWrite(motor_groundlvl[0], LOW); 

delay(DELAY_TIME);
digitalWrite(motor_right[0], LOW); 
}



