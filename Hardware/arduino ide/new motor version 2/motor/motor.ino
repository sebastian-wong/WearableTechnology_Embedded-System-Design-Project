 // Use this code to test your motor with the Arduino board:

// if you need PWM, just use the PWM OUTPUTs on the Arduino
// and instead of digitalWrite, you should use the analogWrite command


//motor for the detection of the obstacles at close range

// --------------------------------------------------------------------------- Motors
//int motor_left[] = {2, 3};
//int motor_right[] = {7, 8};

//int motor_left[] = {53}; //arduino mega


#define DELAY_TIME_VERYHIGH_INTENSITY 1000 
#define DELAY_TIME_HIGH_INTENSITY 750 
#define DELAY_TIME_MEDIUM_INTENSITY 500 
#define DELAY_TIME_LOW_INTENSITY 250 

extern int shoulderLeft; 
extern int shoulderRight;
extern int chestLeft;
extern int chestRight;
extern int head_IR;
extern int head_ultrasound;
extern int waistLeft;
extern int waistCentre;
extern int waistRight;

int motor_shoulderLeft[] = {8};
int motor_shoulderRight[] = {9}; 
int motor_chestLeft[] = {10};
int motor_chestRight[] = {11};
int motor_head_IR[] = {12};
int motor_head_ultrasound[] = {13};
int motor_waistLeft[] = {5};
int motor_waistCentre[] = {6};
int motor_waistRight[] = {7};

// --------------------------------------------------------------------------- Setup
void setup() {
  Serial.begin(9600);

// Setup motors
  int i;
  for(i = 0; i < 1; i++){
    pinMode(motor_shoulderLeft[i], OUTPUT);
    pinMode(motor_shoulderRight[i], OUTPUT);
    pinMode(motor_chestLeft[i], OUTPUT);
    pinMode(motor_chestRight[i], OUTPUT);  
    pinMode(motor_head_IR[i], OUTPUT);
    pinMode(motor_head_ultrasound[i], OUTPUT);
    pinMode(motor_waistLeft[i], OUTPUT);
    pinMode(motor_waistCentre[i], OUTPUT);
    pinMode(motor_waistRight[i], OUTPUT);
  }
}
// --------------------------------------------------------------------------- Loop
void loop() {  //not needed. intepretation will be done in translation file
  ultrasound_motordriver();
  shortrangeIR_motordriver();
  longrangeIR_motordriver();

}
  
// --------------------------------------------------------------------------- Drive


void ultrasound_motordriver(){
 if(shoulderLeft < 200){
  digitalWrite(motor_shoulderLeft[0], HIGH);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(shoulderLeft < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(shoulderLeft >= 80 && shoulderLeft <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_shoulderLeft[0], LOW);
 }
 
  if(shoulderRight < 200){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], HIGH); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(shoulderRight < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(shoulderRight >= 80 && shoulderRight <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_shoulderRight[0], LOW);
 } 
 
  if(chestLeft < 200){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], HIGH);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(chestLeft < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(chestLeft >= 80 && chestLeft <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_chestLeft[0], LOW);
 } 
 
 if(chestRight < 200){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], HIGH);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(chestRight < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(chestRight >= 80 && chestRight <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_chestRight[0], LOW);
 } 
 
 if(head_ultrasound < 200){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], HIGH);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(head_ultrasound < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(head_ultrasound >= 80 && head_ultrasound <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_head_ultrasound[0], LOW);
 } 
}


void shortrangeIR_motordriver(){  
 if(head_IR < 140){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], HIGH);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(head_IR < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(head_IR >= 80 && head_IR <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_head_IR[0], LOW);
 }
  
 if(waistLeft < 140){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],HIGH);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(waistLeft < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(waistLeft >= 80 && waistLeft <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_waistLeft[0], LOW);
 }
 
 if(waistRight < 140){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],LOW);
  digitalWrite(motor_waistRight[0],HIGH); 
    //low intensity
  if(waistRight < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(waistRight >= 80 && waistRight <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_waistRight[0], LOW);
 }
 
}

void longrangeIR_motordriver(){
 if(waistCentre < 140){
  digitalWrite(motor_shoulderLeft[0], LOW);
  digitalWrite(motor_shoulderRight[0], LOW); 
  digitalWrite(motor_chestLeft[0], LOW);
  digitalWrite(motor_chestRight[0], LOW);
  digitalWrite(motor_head_IR[0], LOW);
  digitalWrite(motor_head_ultrasound[0], LOW);
  digitalWrite(motor_waistLeft[0],LOW);
  digitalWrite(motor_waistCentre[0],HIGH);
  digitalWrite(motor_waistRight[0],LOW); 
    //low intensity
  if(waistCentre < 80)
    delay(DELAY_TIME_HIGH_INTENSITY);
  else if(waistCentre >= 80 && waistCentre <140)
    delay(DELAY_TIME_MEDIUM_INTENSITY);
  else 
    delay(DELAY_TIME_LOW_INTENSITY);
 
  digitalWrite(motor_waistCentre[0], LOW);
 }
}






