#include <AFMotor.h>
#include <Ultrasonic.h>
#include <Servo.h>

Servo front_servo; // define servo for right-left turns

AF_DCMotor left_back_Motor(3); // left back dc motor
AF_DCMotor right_back_Motor(4); // right back dc motor

Ultrasonic ultrasonic_back(A8, A9); // ultrasonic for back
Ultrasonic ultrasonic_right_back(A10, A11); // ultrasonic for right back
Ultrasonic ultrasonic_right_front(A12, A13); // ultrasonic for right front
Ultrasonic ultrasonic_front(A14, A15); // ultrasonic for front


// data from ultrasonic sensors
long front_Sensor = ultrasonic_front.distanceRead(CM); 
long right_front_Sensor = ultrasonic_right_front.distanceRead(CM);
long right_back_Sensor = ultrasonic_right_back.distanceRead(CM);
long back_Sensor = ultrasonic_back.distanceRead(CM);

bool park_starter_boolen = true; // to start park movements if park area found false

int close_distance = 5; // to check distance for car to next car
int delayMS = 200; // delay for movements


#define left 0             //left direction command
#define right 1            //right direction command
#define forward 2          //forward direction command
#define backward 3         //backward command
#define middle 4          // middee command
#define car_width 15   //Width of the car (cm)
#define car_length 28  //the length of the car (cm)

// Movement functions for directions
// motor for movement decision
// speed for motor speed
void Robot_Movement(byte motor, byte speed) {
  if (motor == forward) {
    left_back_Motor.setSpeed(speed);
    right_back_Motor.setSpeed(speed);
    left_back_Motor.run(FORWARD);
    right_back_Motor.run(FORWARD);
  }
  if (motor == backward) {
    left_back_Motor.setSpeed(speed);
    right_back_Motor.setSpeed(speed);
    left_back_Motor.run(BACKWARD);
    right_back_Motor.run(BACKWARD);
  }
  if (motor == left) {
    front_servo.write(180);
  }
  if (motor == right) {
    front_servo.write(0);
  }
  if (motor == middle) {
    front_servo.write(0);
  }
}
// motor stop funcition dc:stop servo:90
void Robot_Stop() {
  left_back_Motor.run(RELEASE);
  right_back_Motor.run(RELEASE);
  front_servo.write(90);
}
// Search for parking place
bool Parking_Place_Finder() {
  long front_Sensor = ultrasonic_front.distanceRead(CM);
  long right_Sensor = ultrasonic_right_front.distanceRead(CM);
  long right_back_Sensor = ultrasonic_right_back.distanceRead(CM);

}

// move for parking place control
void Parking_Place_Initial_Move(){
  while(park_starter_boolen){
    if(front_Sensor > 0 && right_front_Sensor <close_distance){ // if front empty and right front has car
      Robot_Movement(forward,100);
      delay(delayMS);
    }
    if(right_front_Sensor > car_width && right_back_Sensor > car_width){  // right front and right back is clear start parking
      Parking_Algorithm();
    }
  }

}
void Parking_Algorithm(){
  while(right_back_Sensor < close_distance){ // fisrt park move
    Robot_Movement(forward,100);
    delay(delayMS);
  }
  Robot_Movement(right,0); // turn right
  Robot_Movement(backward,100); // move back
  Robot_Movement(left,0); // turn left
  Robot_Movement(backward,100); // move back
  Robot_Movement(middle,0);// turn mid
  Robot_Movement(forward,100);

}

void setup() {
  Serial.begin(9600); // monitor start
  front_servo.attach(10); // servo connection
}
void loop() {
  Parking_Place_Finder();
  Parking_Place_Initial_Move();
  Parking_Algorithm();
  Robot_Stop();

}