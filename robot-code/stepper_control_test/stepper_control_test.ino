// Define pins for direction and step control
int dirControl = 12;
int stepControl = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(dirControl,OUTPUT);
  pinMode(stepControl,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Turn for 1 second
  digitalWrite(dirControl,HIGH);
  tone(stepControl,50,1000);
  delay(1000);
  // Stop turning for 1 second
  digitalWrite(stepControl,LOW);
  delay(1000);
}
