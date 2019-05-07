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
int RW_FWD_max = 120;
int LW_FWD_max = 60;
int RW_REV_max = 60;
int LW_REV_max = 120;
int CW_max = 150;
int CCW_max = 30;
int neutral = 90;                       // the motor will do nothing, but will be powered
int RK_CW = 70;
int RK_stop = 92;
int RK_CCW = 115;
int RW_FWD_range = RW_FWD_max - neutral;
int RW_REV_range = neutral - RW_REV_max;
int LW_FWD_range = LW_FWD_max - neutral;
int LW_REV_range = neutral - LW_REV_max;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

// variables to hold the parsed data
char commandFromPC[numChars] = {0};
float rightWheelSpeed = 0.0;
float leftWheelSpeed = 0.0;

boolean newCommand = false;

//============

void setup() {
  Serial.begin(115200);
  Serial.println("<Arduino Ready for Commands>");
  Serial.println("Manual control entry should open with < and close with >");
  Serial.println("Enter data for driving in this style: <w,0.43,-0.54>");
  Serial.println("Enter single commands in this style: <g>");
  Serial.println();

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

//============

void loop() {
    recvWithStartEndMarkers();
    if (newCommand == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        showParsedData();
        actOnCommand();
        newCommand = false;
    }
}

//============

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newCommand == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newCommand = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index
  if (tempChars[0] == 'w') {
    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(commandFromPC, strtokIndx); // copy it to commandFromPC
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    leftWheelSpeed = atof(strtokIndx);     // convert this part to a float
    strtokIndx = strtok(NULL, ",");
    rightWheelSpeed = atof(strtokIndx);     // convert this part to a float
  }
  else {
    strcpy(commandFromPC,&tempChars[0]);
  }
}

//============

void showParsedData() {
    Serial.print("Message ");
    Serial.println(commandFromPC);
    Serial.print("Left Wheel Speed ");
    Serial.println(leftWheelSpeed);
    Serial.print("Right Wheel Speed ");
    Serial.println(rightWheelSpeed);
}

void driveForward() {
  int commandRW = neutral;
  int commandLW = neutral;
  if (rightWheelSpeed > 0) {
    commandRW += rightWheelSpeed*RW_FWD_range;
  }
  else {
    commandRW += rightWheelSpeed*RW_REV_range;
  }
  if (leftWheelSpeed > 0) {
    commandLW += leftWheelSpeed*LW_FWD_range;
  }
  else {
    commandLW += leftWheelSpeed*LW_REV_range;
  }
  Serial.println("Left speed: " + String(commandLW));
  Serial.println("Right speed: " + String(commandRW));
  RW.write(commandRW);
  LW.write(commandLW);
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

void actOnCommand() {
  Serial.println("Command from PC: " + String(commandFromPC));
  if(commandFromPC[0]=='w') {
    driveForward();
    Serial.println("drive");
  }
  else if(commandFromPC[0] == 'a') {
    turnLeft();
    Serial.println("left");
  }
  else if(commandFromPC[0] == 's') {
    driveReverse();
    Serial.println("reverse");
  }
  else if(commandFromPC[0] == 'd') {
    turnRight();
    Serial.println("right");
  }
  else if(commandFromPC[0] == 'f') {
    Serial.println("Shoot");
    shootBeanbag();
  }
  else if(commandFromPC[0] == ' ') {
    driveStop();
    Serial.println("stop");
  }
  else if(commandFromPC[0] == 't') {
    moveRack(1,100);
    Serial.println("move pinion forward");
  }
  else if(commandFromPC[0] == 'g') {
    moveRack(0,100);
    Serial.println("move pinion backward");
  }
  else if(commandFromPC[0] == 'y') {
    reload(1,1000);
    Serial.println("Reload forward");
  }
  else if(commandFromPC[0] == 'h') {
    reload(0,1000);
    Serial.println("Reload backward");
  }
  else if(commandFromPC[0] == 'z') {
    Serial.println("Sensor reading: " + String(readSensor()));
  }
  else {
    Serial.println("Command not recognized!");
  }
}
