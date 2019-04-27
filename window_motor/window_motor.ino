// this script is used to drive a window motor through a 
// BD65496MUV Motor controller, and to taggle the direction of the 
// motor based off of a toggle switch

#include <Servo.h> 

int PWM_A_pin = 9;            // set this to control the speed of the motor
//int PWM_B_pin = 10;           // set this high to go CCW and low to go CW
int dir = 1;                  // to remember the direction we are going in, 1 for CCW, -1 for CW
int num = 110;                // will be changed to be the pulse width command

int lim_pin = 4;              // this will be read to see if the limit switch has been pressed
int f_limSwitch = 0;          // 0 for when no signal, and 1 for when the switch has been pressed
int last_lim_signal = 0;      // we really only wnat to change direction when the button goes from 0 to 1

Servo WM_A;               // for the window motor
Servo WM_B;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin


void check_limit()
{
  // this will check if the limit switch has been triggered
  // will raise a flag to tell the motor driver to reverse direction
// read the state of the switch into a local variable:
  int reading = digitalRead(lim_pin);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) 
    {
      Serial.println("layer 2");
      buttonState = reading;

      // only toggle the flag if the new button state is HIGH
      if (buttonState == HIGH) 
      {
        f_limSwitch = 1;
//        ledState = !ledState;
      }
      else
      {
        f_limSwitch = 0;
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  
//  bool val = digitalRead(lim_pin);
//  if (val == HIGH)
//  {
//    // The switch has been pressed
//    f_limSwitch = 1;
//  }
//  else
//  {
//    f_limSwitch = 0;
//  }
}

void drive_motor()
{
  // this drives the motor, and will toggle direction based on a limit_switch

  // Read the limit_switch
  check_limit();

  // based off of the limit switch 
  if (last_lim_signal == 0 and f_limSwitch == 1)          // notice the rising edge
  {
    Serial.println("reverse!");
    if (dir == 1)                                         // we are currently going CCW, swap dir
    {
      num = 75;                                      // write out low to go CW
      dir = -1;
    }
    else                                            // we are currently going CW, swap dir.
    {
      num = 110;                                      // write out high to go CCW
      dir = 1;
    }
  }
  WM_A.write(num);                                    // this should always be on unless we want to brake
  last_lim_signal = f_limSwitch;                      // save the received flag value as the last flag value for next round
}


void setup() 
{
  Serial.begin(9600);
  while(!Serial){};
  
  TCCR1B = TCCR1B & 0b11111000 | 0x04; // PWM Freq. at 122Hz
  pinMode(lim_pin,INPUT);
  WM_A.attach(PWM_A_pin);
//  WM_B.attach(PWM_B_pin);
  WM_A.write(90);                   // set motors to mid-point
//  WM_B.write(90); 

}

void loop() 
{
  drive_motor();
  delay(5);
}
