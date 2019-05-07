#include <Servo.h>
#include <SoftwareSerial.h>

int sensorPin = A0;
const int backSwitchPin = 2;
const int frontSwitchPin = 3;
const long debounce_ms = 200;
volatile bool shootFinish = false;
const int dirReloadPin = 13;
const int stepReloadPin = 12;

//int bluetoothTx = 7;
//int bluetoothRx = 8;
//SoftwareSerial bluetooth(bluetoothTx, bluetoothRx); 

// Arduino uno 490 Hz pins: 3, 9, 10, 11
// Arduino uno 980 Hz pins: 5, 6
const int LWPin = 10;                   // right wheel pin
const int RWPin = 11;                   // left wheel pin
const int WMPin = 9;
const int rackServoPin = 6;
Servo RW; // Right wheel
Servo LW; // Left wheel
Servo WM; // Window motor
Servo RK; // Rack servo


// motor speeds
// These will need to be tuned as each direction of a motor is not consistant
int RW_FWD_max = 108;
int LW_FWD_max = 79;
int RW_REV_max = 80;
int LW_REV_max = 110;
int CW_max = 110;
int CCW_max = 75;
int neutral = 90;                       // the motor will do nothing, but will be powered
int RK_CW = 70;
int RK_stop = 92;
int RK_CCW = 115;

String commandString;
bool newCommand;

void driveForward() {
  RW.write(RW_FWD_max);
  LW.write(LW_FWD_max);
}
void driveReverse() {
  RW.write(RW_REV_max);
  LW.write(LW_REV_max);
}
void turnLeft() {
  RW.write(RW_FWD_max);
  LW.write(LW_REV_max);
}
void turnRight() {
  RW.write(RW_REV_max);
  LW.write(LW_FWD_max);
}
void driveStop() {
  RW.write(neutral);
  LW.write(neutral);
}
void moveRack(int dir, int time_ms) {
  if (dir == 1) {
    RK.write(RK_CW);
  }
  else {
    RK.write(RK_CCW);
  }
  delay(time_ms);
  RK.write(RK_stop);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//  bluetooth.begin(9600);
  while(!Serial);
  Serial.println("Ready");
  TCCR1B = TCCR1B & 0b11111000 | 0x04; // PWM Freq. at 122Hz
 
  RW.attach(RWPin);
  LW.attach(LWPin);
  RW.write(neutral);             // set motors to mid-point
  LW.write(neutral);
  WM.attach(WMPin);
  WM.write(neutral);
  RK.attach(rackServoPin);
  RK.write(RK_stop);

  pinMode(backSwitchPin,INPUT);
  pinMode(frontSwitchPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(backSwitchPin),pin_ISR,RISING);
  attachInterrupt(digitalPinToInterrupt(frontSwitchPin),pin_ISR,RISING);

  pinMode(dirReloadPin,OUTPUT);
  pinMode(stepReloadPin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
//  if(bluetooth.available()) {
//    String command = bluetooth.readString();
//    Serial.println(command);
  if (newCommand) {
    String command = commandString;
    Serial.println("Arduino received: " + command);
    if(command=="w") {
      driveForward();
//      Serial.println("forward");
    }
    if(command=="a") {
      turnLeft();
//      Serial.println("left");
    }
    if(command=="s") {
      driveReverse();
//      Serial.println("reverse");
    }
    if(command=="d") {
      turnRight();
//      Serial.println("right");
    }
    if(command == "f") {
//      Serial.println("Shoot");
      shootBeanbag();
    }
    if(command == " ") {
      driveStop();
//      Serial.println("stop");
    }
    if(command == "t") {
      moveRack(1,100);
//      Serial.println("move pinion forward");
    }
    if(command == "g") {
      moveRack(0,100);
//      Serial.println("move pinion backward");
    }
    if(command == "y") {
      reload(1,400);
    }
    if(command == "h") {
      reload(0,400);
    }
    if(command == "z") {
      Serial.println(String(readSensor()));
    }
    newCommand = false;
    commandString = "";
  }
}

void reload(int dir, int time_ms) {
  if (dir == 1) {
    digitalWrite(dirReloadPin,HIGH);
  }
  else {
    digitalWrite(dirReloadPin,LOW);
  }
  tone(stepReloadPin,50,time_ms);
  delay(time_ms);
  digitalWrite(stepReloadPin,LOW);
}

float readSensor() {
  int sensorValue = analogRead(sensorPin);
  float Vout = (sensorValue/1024.0)*5.0;
  float range = ((Vout - 1.4)/(2.5-1.4))*(0.01-0.002) + 0.002;
  return(1/range);
}

void shootBeanbag() {
  WM.write(CW_max);
  while(!shootFinish);
  WM.write(neutral);
  shootFinish = false;
}

void pin_ISR() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  int backSwitchState = digitalRead(backSwitchPin);
  int frontSwitchState = digitalRead(frontSwitchPin);
  if(interrupt_time - last_interrupt_time > debounce_ms) {
    if (backSwitchState == HIGH) {
      WM.write(CCW_max);
    }
    else if (frontSwitchState == HIGH) {
      WM.write(neutral);
      shootFinish = true;
    }
    last_interrupt_time = interrupt_time;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      newCommand = true;
      break;
    }
    commandString += inChar;
  }
}
