#include <Servo.h> 

// this script will drive the bot forward, then will turn at various angles and speeds 
// to test out the turnability of the bot

// Arduino uno 490 Hz pins: 3, 9, 10, 11
// Arduino uno 980 Hz pins: 5, 6
int RWPin = 10;                   // right wheel pin
int LWPin = 11;                   // left wheel pin

Servo RW;
Servo LW;

float starttime;
float endtime;

// motor speeds
// These will need to be tuned as each direction of a motor is not consistant
int CCW_max = 110;                  // will turn CCW when looking down shaft towards the motor
int CCW_part = 100;                 // will turn CCW when looking down shaft towards the motor
int CW_max = 75;                  // will turn CW when looking down shaft towards the motor
int CW_part = 83;                 // will turn CW when looking down shaft towards the motor
int neutral = 90;                       // the motor will do nothing, but will be powered


void forward()
{
  ///////////////////////////////////////////////////////
  // drive forward for 5 seconds then delay for 2 seconds
  ///////////////////////////////////////////////////////
  // remeber that the motors are pointing in opposite directions
  RW.write(CW_max);                          
  LW.write(CCW_max);
  // delay(5000)
  starttime = millis();
  endtime = starttime;
  
  while ((endtime - starttime) <=5000)      // do this loop for up to 5S
  {
    endtime = millis();
  }
  RW.write(neutral);                        // stop the motors                       
  LW.write(neutral);
  delay(2000);                             // pause for two seconds 
}

void backward()
{
  ///////////////////////////////////////////////////////
  // drive back for 5 seconds then delay for 2 seconds
  ///////////////////////////////////////////////////////
  // remeber that the motors are pointing in opposite directions
  RW.write(CCW_max);                          
  LW.write(CW_max);
  // delay(5000)
  starttime = millis();
  endtime = starttime;
  
  while ((endtime - starttime) <=5000)      // do this loop for up to 5S
  {
    endtime = millis();
  }
  RW.write(neutral);                        // stop the motors                       
  LW.write(neutral);
  delay(2000);                             // pause for two seconds 
}

void broad_turn(int dir)
{
  ///////////////////////////////////////////////////////
  // turn for 5 seconds then delay for 2 seconds
  ///////////////////////////////////////////////////////

  if (dir == 1)                         // turn to the right
  {
    RW.write(CW_part);                          
    LW.write(CCW_max);
  }
  else if (dir == 2)                         // turn to the left
  {
    RW.write(CW_max);                          
    LW.write(CCW_part);
  }
  else
  {
    RW.write(neutral);                          
    LW.write(neutral);
  }
  
  // delay(5000)
  starttime = millis();
  endtime = starttime;
  
  while ((endtime - starttime) <=5000)      // do this loop for up to 5S
  {
    endtime = millis();
  }
  
  RW.write(neutral);                        // stop the motors                       
  LW.write(neutral);
  delay(2000);                             // pause for two seconds 
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// Main code
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  TCCR1B = TCCR1B & 0b11111000 | 0x04; // PWM Freq. at 122Hz
 
  RW.attach(RWPin);
  LW.attach(LWPin);
  RW.write(90);             // set motors to mid-point
  LW.write(90); 
}

void loop()
{
  forward();
}
