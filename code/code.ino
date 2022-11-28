#include <AFMotor.h>
#include <Ultrasonic.h>

AF_DCMotor left_front_Motor(4);
AF_DCMotor right_front_Motor(3);
AF_DCMotor left_back_Motorr(1);
AF_DCMotor right_back_Motor(2);

Ultrasonic ultrasonic_back(40, 41), ultrasonic_left_back(38, 39), ultrasonic_left_front(36, 37), ultrasonic_front(34, 35);
// define ultrasonic sensors
#define left 0             //left direction command
#define right 1            //right direction command
#define forward 2          //forward direction command
#define backward 3         //backward command
#define minimum_limit 15   //Width of the car (cm)
#define minimum_limit1 28  //the length of the car (cm)
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
      left_front_Motor.run(RELEASE);
      right_front_Motor.run(RELEASE);
      left_back_Motorr.run(RELEASE);
      right_back_Motor.run(RELEASE);
    }
  }
}
void motor_pinSetup() {
  left_front_Motor.run(RELEASE);
  right_front_Motor.run(RELEASE);

  left_back_Motorr.run(RELEASE);
  right_back_Motor.run(RELEASE);
}
// Movement functions
void Robot_Movement(byte motor, byte spd) {
  if (motor == forward) {
    left_front_Motor.setSpeed(spd);
    right_front_Motor.setSpeed(spd);
    left_back_Motorr.setSpeed(spd);
    right_back_Motor.setSpeed(spd);
    left_front_Motor.run(FORWARD);
    right_front_Motor.run(FORWARD);
    left_back_Motorr.run(FORWARD);
    right_back_Motor.run(FORWARD);
  }
  if (motor == backward) {
    left_front_Motor.setSpeed(spd);
    right_front_Motor.setSpeed(spd);
    left_back_Motorr.setSpeed(spd);
    right_back_Motor.setSpeed(spd);
    left_front_Motor.run(BACKWARD);
    right_front_Motor.run(BACKWARD);
    left_back_Motorr.run(BACKWARD);
    right_back_Motor.run(BACKWARD);
  }
  if (motor == left) {

    left_front_Motor.setSpeed(spd);
    right_front_Motor.setSpeed(spd);
    left_back_Motorr.setSpeed(spd);
    right_back_Motor.setSpeed(spd);
    left_front_Motor.run(BACKWARD);
    right_front_Motor.run(FORWARD);
    left_back_Motorr.run(BACKWARD);
    right_back_Motor.run(FORWARD);
  }
  if (motor == right) {
    left_front_Motor.setSpeed(spd);
    right_front_Motor.setSpeed(spd);
    left_back_Motorr.setSpeed(spd);
    right_back_Motor.setSpeed(spd);
    left_front_Motor.run(FORWARD);
    right_front_Motor.run(BACKWARD);
    left_back_Motorr.run(FORWARD);
    right_back_Motor.run(BACKWARD);
  }
}
void Robot_Stop() {
  left_front_Motor.run(RELEASE);
  right_front_Motor.run(RELEASE);
  left_back_Motorr.run(RELEASE);
  right_back_Motor.run(RELEASE);
}
// Search for parking
bool Parking_Place_Control() {
  long front_Sensor = ultrasonic_front.distanceRead(CM);
  long right_Sensor = ultrasonic_left_front.distanceRead(CM);
  long right_back_Sensor = ultrasonic_left_back.distanceRead(CM);
  if ((right_Sensor <= minimum_limit) && (right_back_Sensor <= minimum_limit) && (park_status == 0)) {
    Robot_Movement(forward, 100);
    park_status = 1;
    Serial.println(park_status);
  }
  if ((right_Sensor > minimum_limit) && (right_Sensor < minimum_limit1) && (right_back_Sensor > minimum_limit) && (right_back_Sensor < minimum_limit1) && (park_status == 1)) {
    Robot_Movement(forward, 100);
    park_status = 2;
    Serial.println(park_status);
  }
  if ((right_Sensor >= minimum_limit1) && (right_back_Sensor >= minimum_limit1) && (park_status == 1)) {
    /* Vertical Parking Decision */
    Robot_Stop();
    delay(500);
    park_status = 10;
    Serial.println(park_status);
  }
  if ((right_Sensor <= minimum_limit) && (right_back_Sensor <= minimum_limit) && (park_status == 2)) {
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
    long right_Sensor = ultrasonic_left_front.distanceRead(CM);
    Serial.println(right_Sensor);
    long right_back_Sensor = ultrasonic_left_back.distanceRead(CM);
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
  Serial.begin(9600);
  attachInterrupt(5, say, CHANGE);
  pinMode(signal_pin, INPUT);
  motor_pinSetup();
}
void loop() {
  Park_bul();
}