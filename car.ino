#define FORWARD 8
#define BACKWARD 2
#define STOP 5
 
int Motor[2][2] = //two dimensional array
{
{4 , 5},   //input pin to control Motor1--> Motor[0][0]=4, Motor[0][1]=5
{6 , 7},   //input pin to control Motor2--> Motor[1][0]=6, Motor[1][1]=7
};
 
void setup() {
  pinMode(Motor[0][0], OUTPUT);  
  pinMode(Motor[0][1], OUTPUT);
  pinMode(Motor[1][0], OUTPUT);  
  pinMode(Motor[1][1], OUTPUT);  
}
 
void loop() {

  // Get images

  /*

  if(){ // case to go left
    motor_left();
  }
  else if(){ // case to go right
    motor_right();
  }
  else if(){ // case to stop
    motor_run(0, STOP);
    motor_run(1, STOP);
  }
  else{
    motor_run(0, FORWARD);
    motor_run(1, FORWARD);
  }
  
  */
}
 
void motor_run(int motor, int movement) {
  switch (movement) {
    case FORWARD:  
      digitalWrite(Motor[motor][0], HIGH);
      digitalWrite(Motor[motor][1], LOW);
      break;
    case BACKWARD:   
      digitalWrite(Motor[motor][0], LOW);
      digitalWrite(Motor[motor][1], HIGH);
      break; 
    case STOP:  
      digitalWrite(Motor[motor][0], LOW);
      digitalWrite(Motor[motor][1], LOW);
      break;      
  }     
}

void motor_left(){
  motor_run(0, FORWARD);
  motor_run(1, BACKWARD);
  delay(1000);
}

void motor_right(){
  motor_run(0, BACKWARD);
  motor_run(1, FORWARD);
  delay(1000);
}
