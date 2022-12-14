#include <AFMotor.h>
#include <Ultrasonic.h>
#include <Servo.h>

Servo front_servo; // define servo for front

AF_DCMotor left_back_Motor(3); // left back dc motor
AF_DCMotor right_back_Motor(4); // right back dc motor

Ultrasonic ultrasonic_back(A8, A9), ultrasonic_right_back(A10, A11), ultrasonic_right_front(A12, A13), ultrasonic_front(A14, A15);
// define ultrasonic sensors
#define left 0             //left direction command
#define right 1            //right direction command
#define forward 2          //forward direction command
#define backward 3         //backward command
#define car_width 15   //Width of the car (cm)
#define car_length 28  //the length of the car (cm)

byte park_status = 0;      //park status
int signal_pin = 21;       //signal pin speed
volatile int val;
int counter = 0;
int current_status = 0;
int previous_situation = 0;

void say(int saydir) {
  for (int i = 0; i <= saydir; i + 1) {
    val = digitalRead(signal_pin);
    if (val == LOW) {

      current_status = 0;
    }

    else {
      current_status = 1;
    }
    if (current_status != previous_situation) {
      if (current_status == 1) {
        counter = counter + 1;
        Serial.println(counter);
        i = i + 1;
      } else {
        i = i;
      }
      previous_situation = current_status;
    }
    if (i == saydir) {
      left_back_Motor.run(RELEASE);
      right_back_Motor.run(RELEASE);
    }
  }
}
void motor_pinSetup() {
  left_back_Motor.run(RELEASE);
  right_back_Motor.run(RELEASE);
}
// Movement functions for directions
void Robot_Movement(byte motor, byte spd) {
  if (motor == forward) {
 
    left_back_Motor.setSpeed(spd);
    right_back_Motor.setSpeed(spd);

    left_back_Motor.run(FORWARD);
    right_back_Motor.run(FORWARD);
  }
  if (motor == backward) {

    left_back_Motor.setSpeed(spd);
    right_back_Motor.setSpeed(spd);
 
    left_back_Motor.run(BACKWARD);
    right_back_Motor.run(BACKWARD);
  }
  if (motor == left) {

    front_servo.write(180);

  }
  if (motor == right) {

    front_servo.write(0);

  }
}
// motor stop funcition dc:stop servo:90
void Robot_Stop() {

  left_back_Motor.run(RELEASE);
  right_back_Motor.run(RELEASE);
  front_servo.write(90);
}
// Search for parking
bool Parking_Place_Control() {
  // read distances
  long front_Sensor = ultrasonic_front.distanceRead(CM);
  long right_Sensor = ultrasonic_right_front.distanceRead(CM);
  long right_back_Sensor = ultrasonic_right_back.distanceRead(CM);
  
  if ((right_Sensor <= car_width) && (right_back_Sensor <= car_width) && (park_status == 0)) {
    Robot_Movement(forward, 100);
    park_status = 1;
    Serial.println(park_status);
  }
  if ((right_Sensor > car_width) && (right_Sensor < car_length) && (right_back_Sensor > car_width) && (right_back_Sensor < car_length) && (park_status == 1)) {
    Robot_Movement(forward, 100);
    park_status = 2;
    Serial.println(park_status);
  }
  if ((right_Sensor >= car_length) && (right_back_Sensor >= car_length) && (park_status == 1)) {
    /* Vertical Parking Decision */
    Robot_Stop();
    delay(500);
    park_status = 10;
    Serial.println(park_status);
  }
  if ((right_Sensor <= car_width) && (right_back_Sensor <= car_width) && (park_status == 2)) {
    /* Parallel Parking Decision */
    park_status = 3;
    Serial.println(park_status);
  }
  return park_status;
}
void Park_bul() {
  Parking_Place_Control();
  if (park_status == 3) {
    Robot_Stop();
    Serial.println(park_status);
    delay(400);
    park_status = 4;
  }
  if (park_status == 4) {
    Robot_Movement(backward, 120);
    say(18);
    Robot_Stop();
    Serial.println(park_status);
    delay(500);
    Robot_Movement(right, 150);
    say(9);
    Robot_Stop();
    delay(500);
    park_status = 5;
  }
  if (park_status == 5) {
    Robot_Movement(backward, 120);
    long arka_Sensor = ultrasonic_back.distanceRead(CM);
    Serial.println(arka_Sensor);
    if (arka_Sensor > 0 && arka_Sensor <= 13) {
      Robot_Stop();
      delay(400);
      park_status = 6;
    }

    return arka_Sensor;
  }
  if (park_status == 6) {
    Robot_Movement(left, 150);
    long right_Sensor = ultrasonic_right_front.distanceRead(CM);
    Serial.println(right_Sensor);
    long right_back_Sensor = ultrasonic_right_back.distanceRead(CM);
    Serial.println(right_back_Sensor);
    if (right_Sensor == right_back_Sensor) {
      Robot_Stop();
      park_status = 7;
    }
    return right_Sensor, right_back_Sensor;
  }
  if (park_status == 7) {
    long front_Sensor = ultrasonic_front.distanceRead(CM);
    if (front_Sensor <= 6) {
      Robot_Stop();
      park_status = 8;
    } else {
      Robot_Movement(forward, 100);
    }
    return front_Sensor;
  }
  if (park_status == 10) {

    Robot_Movement(left, 180);
    say(14);
    Robot_Stop();
    delay(500);
    park_status = 7;
  }
}
void setup() {
  Serial.begin(9600); // monitor start
  front_servo.attach(10); // servo connection
  attachInterrupt(5, say, CHANGE); 
  pinMode(signal_pin, INPUT);
  motor_pinSetup();
}
void loop() {
  Park_bul();
}
