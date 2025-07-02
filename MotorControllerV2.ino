#include <AFMotor.h> 
#include <SoftwareSerial.h>

// GSM module setup
SoftwareSerial mySerial(6, 7); // RX, TX for SIM900A
const char* mobileNumber = "+919347265708"; // Mobile number to send SMS
bool smsSent = false;     // Flag to prevent multiple SMS sends

// Ultrasonic sensor pins
#define echoPin 9   // Echo pin on pin 9
#define trigPin 10  // Trigger pin on pin 10

// Initialize the four motors
AF_DCMotor front_left(1, MOTOR12_64KHZ);
AF_DCMotor rear_left(2, MOTOR12_64KHZ);
AF_DCMotor front_right(3, MOTOR12_64KHZ);
AF_DCMotor rear_right(4, MOTOR12_64KHZ);

int count = 1;
int Direction = 1;
int distance = 0;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  mySerial.begin(9600);
  Serial.println("GSM Modem and Motors Ready");
  delay(3000);
  // sendSMS();
}

void loop() {
  // Measure distance with ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  int duration = pulseIn(echoPin, HIGH);
  distance = (duration - 10) * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  Serial.print("count: ");
  Serial.println(count);

  if (distance < 10 && Direction == 1 && !smsSent) {
    Direction = -1;
    stopMotors();
    sendSMS();
    smsSent = true;
  }

  // if (distance < 10 && Direction == 1) {
  //   Direction = -1;
  //   stopMotors();
  //   delay(2000);
  // }
  if (count <= 1 && Direction == -1) {
    Direction = 0;
  }

  if (Direction == 1) {
    moveForward();
    count = count + 1;
  }
  if (Direction == -1) {
    moveBackward();
    count = count - 1;
  }
  if (Direction == 0) {
    stopMotors();
    count = 1;
    Direction = 1;
    distance = 0;
    smsSent = false;
    delay(5000);
  }

  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}

// Motor control functions
void stopMotors() {
  front_left.setSpeed(0);
  rear_left.setSpeed(0);
  front_right.setSpeed(0);
  rear_right.setSpeed(0);
  front_left.run(RELEASE);
  rear_left.run(RELEASE);
  front_right.run(RELEASE);
  rear_right.run(RELEASE);
}

void moveForward() {
  front_left.setSpeed(255);
  rear_left.setSpeed(255);
  front_right.setSpeed(255);
  rear_right.setSpeed(255);
  front_left.run(FORWARD);
  rear_left.run(FORWARD);
  front_right.run(FORWARD);
  rear_right.run(FORWARD);
}

void moveBackward() {
  front_left.setSpeed(255);
  rear_left.setSpeed(255);
  front_right.setSpeed(255);
  rear_right.setSpeed(255);
  front_left.run(BACKWARD);
  rear_left.run(BACKWARD);
  front_right.run(BACKWARD);
  rear_right.run(BACKWARD);
}

// Function to send an SMS
void sendSMS() {
  Serial.println("Sending SMS through GSM Modem");
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.print("AT+CMGS=\"");
  mySerial.print(mobileNumber);
  mySerial.println("\"\r");
  delay(1000);
  mySerial.println("Object detected by the radar!");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
  Serial.print("SMS Sent to ");
  Serial.println(mobileNumber);
}
