
// Authorship Statement Fill in later//

// Credit for ESP 32 Functionality:
// https://randomnerdtutorials.com/installing-esp32-arduino-ide-2-0/

// Credit for Sensor Functionality:
// 

// Credit for PS5 Library:
// https://www.instructables.com/ESP-32-WITH-PS5-CONTROLLER-CAR/

// Libraries
#include <ps5Controller.h>

// Pin definitions
#define LED 2
// LED 2 is for testing purposes
#define sensorTrigger 23
#define echoFront 22
#define echoBack 19
#define echoLeft 21
#define echoRight 18
#define MotorLeft1 15
#define MotorLeft2 4
#define MotorRight1 17
#define MotorRight2 16
#define MotorLeftEnable 26
#define MotorRightEnable 27

// Global Variables
float durationFront, durationLeft, durationRight, durationBack;
float distanceFront, distanceLeft, distanceRight, distanceBack;
//for action variables, 0 for not moving, 1 for forward, 2 for backwards
int leftAction = 0;
int rightAction = 0;
bool obstacle = false;
int sensorSkipper = 0;
int iterationAmount = 8;

//pwm variables
const int freq = 30000;
const int leftPwmChannel = 0;
const int rightPwmChannel = 1;
const int resolution = 8;
int leftSpeed = 128;
int rightSpeed = 128;

// Gives control to the operator
// void PSFiveTime(float distance) {
//   while (ps5.isConnected() == true) {

//     if (ps5.LStickX()) {
//       Serial.printf("Left Stick x at %d\n", ps5.LStickX());
//     }

//     if (ps5.LStickY()) {
//       Serial.printf("Left Stick y at %d\n", ps5.LStickY());

//       if (ps5.LStickY() > 100) {
//         digitalWrite(RightMotor, HIGH);
//         digitalWrite(LeftMotor, LOW);

//       }

//       else {
//         digitalWrite(LeftMotor, LOW);
//         digitalWrite(RightMotor, LOW);
//       }
//     }

//     if (ps5.LStickY() < 20) {

//       digitalWrite(LeftMotor, HIGH);
//       digitalWrite(RightMotor, LOW);

//     }

//     else{
//       digitalWrite(RightMotor, LOW);
//       digitalWrite(LeftMotor, LOW);
//     }
//   }
//   return;
// }

void setup() {

  // Defines baud rate
  Serial.begin(9600);

  // Setup for Sensor Pins
  pinMode(sensorTrigger, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(echoBack, INPUT);
  pinMode(echoLeft, INPUT);
  pinMode(echoRight, INPUT);

  // Setup for Motor and MC Pins
  pinMode(MotorRight1, OUTPUT);
  pinMode(MotorRight2, OUTPUT);
  pinMode(MotorLeft1, OUTPUT);
  pinMode(MotorLeft2, OUTPUT);
  pinMode(MotorLeftEnable, OUTPUT);
  pinMode(MotorRightEnable, OUTPUT);

  //attach pwm to enable pin
  ledcAttach(MotorLeftEnable, freq, resolution);
  ledcAttach(MotorRightEnable, freq, resolution);

  // Setup for PS5 remote
  ps5.begin("84:30:95:DE:45:AE"); //replace with MAC address of your
  // controller
  Serial.println("Ready.");
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, LOW);
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, LOW);
}

void loop() {

  while(ps5.isConnected() == true){
    if(sensorSkipper > iterationAmount){
      sensorSkipper = 0;
      // Main Sensor Logic //
      // Left
      digitalWrite(sensorTrigger, LOW);
      delayMicroseconds(2);
      digitalWrite(sensorTrigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(sensorTrigger, LOW);
      durationLeft = pulseIn(echoLeft, HIGH);
      distanceLeft = (durationLeft * .0343) / 2;
      //Front
      digitalWrite(sensorTrigger, LOW);
      delayMicroseconds(2);
      digitalWrite(sensorTrigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(sensorTrigger, LOW);
      durationFront = pulseIn(echoFront, HIGH);
      distanceFront = (durationFront * .0343) / 2;
      //Right
      digitalWrite(sensorTrigger, LOW);
      delayMicroseconds(2);
      digitalWrite(sensorTrigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(sensorTrigger, LOW);
      durationRight = pulseIn(echoRight, HIGH);
      distanceRight = (durationRight * .0343) / 2;
      //Back
      digitalWrite(sensorTrigger, LOW);
      delayMicroseconds(2);
      digitalWrite(sensorTrigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(sensorTrigger, LOW);
      durationBack = pulseIn(echoBack, HIGH);
      distanceBack = (durationBack * .0343) / 2;
    }else{
      sensorSkipper++;
      distanceFront = 0;
      distanceBack = 0;
      distanceRight = 0;
      distanceLeft = 0;
    }
    //checks for obstacle close to ultrasonic sensor, makes motors move in opposite direction if true
    obstacle = false;
    leftSpeed = 255;
    rightSpeed = 255;

    if((distanceFront < 10 && distanceFront > 0) || (distanceLeft < 10 && distanceLeft > 0) || (distanceRight < 10 && distanceRight > 0) || (distanceBack < 10 &&  distanceBack > 0)){
      obstacle = true;
      Serial.print("DistanceFront: ");
      Serial.println(distanceFront);

      Serial.print("DistanceBack: ");
      Serial.println(distanceBack);

      Serial.print("DistanceLeft: ");
      Serial.println(distanceLeft);

      Serial.print("DistanceRight: ");
      Serial.println(distanceRight);
      
      if(distanceFront < 10) {
        if(distanceBack > 10){
          leftAction = 2;
          rightAction = 2;
        }else if(distanceLeft > 10){
          leftAction = 2;
          rightAction = 1;
        }else if(distanceRight > 10){
          leftAction = 1;
          rightAction = 2;
        }else{
          leftAction = 0;
          rightAction = 0;
        }
      }else if(distanceLeft < 10){
        if(distanceRight > 10){
          leftAction = 1;
          rightAction = 2;
        }else if(distanceFront > 10){
          leftAction = 1;
          rightAction = 2;
        }else if(distanceBack > 10){
          leftAction = 2;
          rightAction = 2;
        }else{
          leftAction = 0;
          rightAction = 0;
        }
      }else if(distanceRight < 10){
        if(distanceLeft > 10){
          leftAction = 2;
          rightAction = 1;
        }else if(distanceFront > 10){
          leftAction = 1;
          rightAction = 2;
        }else if(distanceBack > 10){
          leftAction = 2;
          rightAction = 2;
        }else{
          leftAction = 0;
          rightAction = 0;
        }
      }else if(distanceBack < 10) {
        if(distanceFront > 10){
          leftAction = 1;
          rightAction = 1;
        }else if(distanceLeft > 10){
          leftAction = 2;
          rightAction = 1;
        }else if(distanceRight > 10){
          leftAction = 1;
          rightAction = 2;
        }else{
          leftAction = 0;
          rightAction = 0;
        }
      }
    }else{
      Serial.printf("Left Stick x at %d\n", ps5.LStickX());
      Serial.printf("Left Stick y at %d\n", ps5.LStickY());

      if(ps5.Cross()){
        // Serial.println("Cross Pressed");
        leftAction = 1;
        rightAction = 1;
        if(ps5.LStickX() > 20){
          leftSpeed += ps5.LStickX() / 2;
        }else if(ps5.LStickX() < -20){
          rightSpeed -= ps5.LStickX() / 2;
        }
      }else if(ps5.Circle()){
        // Serial.println("Circle Pressed");
        leftAction = 2;
        rightAction = 2;
        if(ps5.LStickX() > 20){
          leftSpeed += ps5.LStickX() / 2;
        }else if(ps5.LStickX() < -20){
          rightSpeed -= ps5.LStickX() / 2;
        }
      }else{
        if(ps5.LStickX() > 20){
          leftAction = 1;
          rightAction = 2;
        }else if(ps5.LStickX() < -20){
          leftAction = 2;
          rightAction = 1;
        }else{
          leftAction = 0;
          rightAction = 0;
        }
      }  

    }

    if(leftAction == 1){
      digitalWrite(MotorLeft1, HIGH);
      digitalWrite(MotorLeft2, LOW);
    }else if(leftAction == 2){
      digitalWrite(MotorLeft1, LOW);
      digitalWrite(MotorLeft2, HIGH);
    }else{
      digitalWrite(MotorLeft1, LOW);
      digitalWrite(MotorLeft2, LOW);
    }

    if(rightAction == 1){
      digitalWrite(MotorRight1, HIGH);
      digitalWrite(MotorRight2, LOW);
    }else if(rightAction == 2){
      digitalWrite(MotorRight1, LOW);
      digitalWrite(MotorRight2, HIGH);
    }else{
      digitalWrite(MotorRight1, LOW);
      digitalWrite(MotorRight2, LOW);
    }

    //writing speed to motors
    ledcWrite(MotorLeftEnable, leftSpeed);
    ledcWrite(MotorRightEnable, rightSpeed);  

    if(obstacle){
      delay(500);
      Serial.println("Obstacle Detected");
    }
  }
}  
