

#include <AccelStepper.h>
#include "IRremote.h"

int receiver = 13; // Signal Pin of IR receiver to Arduino Digital Pin 6


enum
stepper_Motor {
  FUNCTION = 0, DRIVER = 1, FULL2WIRE = 2, FULL3WIRE = 3,
  FULL4WIRE = 4, HALF3WIRE = 6, HALF4WIRE = 8
};


#define sleep_pwr 4


AccelStepper stepper_Motor(DRIVER, 3, 2); //stepper(driver, step, dir)
IRrecv irrecv(receiver);    // create instance of 'irrecv'
decode_results results;     // create instance of 'decode_results'

int stepsPerRev = 200; // Stepper motors are often 200 steps per revolution
int gearRatio = 4; // number of motor rotations required to rotate the drum one full revolution (e.g. motor pulley has 20 teeth, and the drum pulley has 80, so 4:1 ratio)
int microSteps = 16; // this can be manually specified using the "mode" pins on the DRV8825 board:

//         M0   M1    M2          RESOLUTION
//------------------------------------------------
//        LOW   LOW   LOW        FULL  STEP   1 Revolution  200  steps
//        HIGH  LOW   LOW        HALF  STEP   1 Revolution  400  steps
//        LOW   HIGH  LOW        1/4   STEP   1 Rrevolution  800  steps
//        HIGH  HIGH  LOW        1/8   STEP   1 Rrevolution  1600 steps
//        LOW   LOW   HIGH       1/16  STEP   1 Revolution  3200 steps
//        HIGH  LOW   HIGH       1/32  STEP   1 Revolution  6400 steps


float speedRPM = 1.0; // the speed of rotation. Optomotor experiments often use around 2-4 RPM
float timeRot = 45.0; // number of seconds to rotate in each direction
int nRot = 3; // the number of rotations in each direction
float stepperAccell = 5000; // acceleration and deceleration of stepper motor
int dir = -1; // initial direction: -1 = clockwise, 1 = anticlockwise

int nRem = 0;
//int nDec = 256; // number of decelleration steps when halt/reverse occurs

void setup() {

  Serial.begin(9600);
  
  Serial.println("Initialised"); 
  irrecv.enableIRIn(); // Start the receiver
  pinMode(sleep_pwr, OUTPUT);
  digitalWrite(sleep_pwr,LOW);
  stepper_Motor.setCurrentPosition(0);

}

void loop() {


  if (irrecv.decode(&results)){ // have we received an IR signal?

    Serial.println(results.value);
    switch(results.value){


      case 0xFFA25D: // power - stop everything!
          stepper_Motor.setCurrentPosition(0);
         //stepper_Motor.moveTo(stepper_Motor.currentPosition() + (nDec*dir));
         nRem = 0;
      break;

      case 0xFF02FD: // play/pause - switch direction manually
          stepper_Motor.setCurrentPosition(0);
         //stepper_Motor.moveTo(stepper_Motor.currentPosition() + (nDec*dir));
         nRem = nRem+1;
      break;

      case 0xFF906F: // up
         nRem = nRot;
         dir = -1;
      break;

      case 0xFFE01F: // down
         nRem = nRot;
         dir = 1;
      break;

      case 0xFF30CF: // 1
         speedRPM = 1.0;
      break;
      
      case 0xFF18E7: // 2
         speedRPM = 2.0;
      break;

      case 0xFF7A85: // 3
         speedRPM = 3.0;
      break;
      
      case 0xFF10EF: // 4
         speedRPM = 4.0;
      break;
      
      case 0xFF38C7: // 5
         speedRPM = 5.0;
      break;
      
      case 0xFF5AA5: // 6
         speedRPM = 6.0;
      break;

      case 0xFF42BD: // 7
         speedRPM = 7.0;
      break;
      
      case 0xFF4AB5: // 8
         speedRPM = 8.0;
      break;

      case 0xFF52AD: // 9
         speedRPM = 9.0;
      break;
      
      case 0xFF6897: // 0
         speedRPM = 10.0;
      break;
      
    }
      irrecv.resume(); // receive the next value
  }

  if(stepper_Motor.distanceToGo()==0)  {
    if(nRem > 0){

      stepper_Motor.setCurrentPosition(0);
      
      nRem = nRem-1;
      dir = dir*-1;
      long nSteps = round(stepsPerRev * gearRatio * microSteps * (speedRPM/60) * timeRot * dir);
      float stepperSpeed = nSteps/timeRot;
      
      stepper_Motor.setMaxSpeed(stepperSpeed);
      stepper_Motor.setAcceleration(stepperAccell);
      stepper_Motor.moveTo(nSteps);
    
      Serial.println((String)"nSteps = " + (nSteps));
      Serial.println((String)"stepperSpeed = " + (stepperSpeed));
    
      digitalWrite(sleep_pwr,HIGH);
  
    } else {
      digitalWrite(sleep_pwr,LOW);
    }
  
  }

  stepper_Motor.run();
  


}
