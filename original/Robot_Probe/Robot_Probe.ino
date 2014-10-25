// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 

#define echoPin A0 // Echo Pin
#define trigPin A1 // Trigger Pin

const int pinMotor1 = 11;
const int pinMotor2 = 10;
const int pinMotor3 = 9;
const int pinMotor4 = 6;
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance
 
int ultraSonicVals[101];
 
void setup() 
{ 
  myservo.attach(5);  // attaches the servo on pin 9 to the servo object 
  
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  pinMode(pinMotor3, OUTPUT);
  pinMode(pinMotor4, OUTPUT);
} 
 
 
void loop() 
{ 
  lookLeft();
  
  decide();
  
  //delay(5000);
  
  lookRight();
  
  decide();
  
  //delay(5000);
} 

void lookLeft() {
  for(pos = 20; pos < 120; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    scan(pos - 20);
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}

void lookRight() {
  for(pos = 120; pos>=21; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    scan(pos - 21);
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}

void sort(int a[], int size) {
    for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    int t = a[o];
                    a[o] = a[o+1];
                    a[o+1] = t;
                }
        }
    }
}

void decide() {
  int medianVals[11];
  int sectorVals[11];
  
  for(int i = 0; i <= 100; i+=10) {
    int index = i / 10;
    
    for(int j = i; j <= i+10; j+=1) {
      sectorVals[j-i] = ultraSonicVals[j];
      sort(sectorVals, 10);
      medianVals[index] = sectorVals[4];
    }
  }
  
  //Serial.println("Medians");
  // Compute move
  int maxVal = 0;
  int rotation = 50;
  for(int i = 0; i <= 10; i+=1) {
    //Serial.println(medianVals[i]);
    
    if (medianVals[i] < 10 && medianVals[i] > maxVal) {
      maxVal = medianVals[i];
      rotation = i * 10;
    }
  }
  
  /*Serial.println(" -------- ");
  Serial.println("value");
  Serial.println(maxVal);
  Serial.println("degrees");
  Serial.println(rotation);
  Serial.println(" -------- ");*/
  
  if (rotation >= 0 && rotation <= 33) {
    right();
  }
  else if (rotation > 66 && rotation <= 100) {
    left();
  }
  else {
    go_forward();
  }
}

void scan(int index) {
  /* The following trigPin/echoPin cycle is used to determine the
   distance of the nearest object by bouncing soundwaves off of it. */ 
   digitalWrite(trigPin, LOW); 
   delayMicroseconds(2); 
  
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10); 
   
   digitalWrite(trigPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   
   //Calculate the distance (in cm) based on the speed of sound.
   distance = duration/58.2;
   
   if (distance >= maximumRange || distance <= minimumRange){
   /* Send a negative number to computer and Turn LED ON 
   to indicate "out of range" */
   //Serial.println("-1");
   ultraSonicVals[index] = -1;
   //digitalWrite(LEDPin, HIGH); 
   }
   else {
   /* Send the distance to the computer using Serial protocol, and
   turn LED OFF to indicate successful reading. */
   //Serial.println(distance);
   ultraSonicVals[index] = distance;
   //digitalWrite(LEDPin, LOW); 
   }
}

/*void forward() {
  Serial.println("forward");
}

void backward() {
  Serial.println("backward");
}*/

void left() {
  //Serial.println("left");
  turn_left();
  delay(200);
  stop_motors();
  go_forward();
}

void right() {
  //Serial.println("right");
  turn_right();
  delay(200);
  stop_motors();
  go_forward();
}

void go_forward() {
  forward();
  delay(400);
  stop_motors();
}

void stop_motors() { // OK
  digitalWrite(pinMotor1, HIGH);
  digitalWrite(pinMotor2, HIGH);
  digitalWrite(pinMotor3, HIGH);
  digitalWrite(pinMotor4, HIGH);
}

void forward() { // OK
  Serial.println("forward");
  digitalWrite(pinMotor1, HIGH);
  digitalWrite(pinMotor2, LOW);
  digitalWrite(pinMotor3, HIGH);
  digitalWrite(pinMotor4, LOW);
}

void backward() { // OK
  digitalWrite(pinMotor1, LOW);
  digitalWrite(pinMotor2, HIGH);
  digitalWrite(pinMotor3, LOW);
  digitalWrite(pinMotor4, HIGH);
}

void turn_left() { // OK
  digitalWrite(pinMotor1, LOW);
  digitalWrite(pinMotor2, HIGH);
  digitalWrite(pinMotor3, HIGH);
  digitalWrite(pinMotor4, LOW);
}

void turn_right() { // OK
  digitalWrite(pinMotor1, HIGH);
  digitalWrite(pinMotor2, LOW);
  digitalWrite(pinMotor3, LOW);
  digitalWrite(pinMotor4, HIGH);
}

void left_only_forward() { // OK
  digitalWrite(pinMotor1, HIGH);
  digitalWrite(pinMotor2, LOW);
  digitalWrite(pinMotor3, HIGH);
  digitalWrite(pinMotor4, HIGH);
}

void right_only_forward() { // OK
  digitalWrite(pinMotor1, LOW);
  digitalWrite(pinMotor2, LOW);
  digitalWrite(pinMotor3, HIGH);
  digitalWrite(pinMotor4, LOW);
}

void left_only_backward() { // OK
  digitalWrite(pinMotor1, LOW);
  digitalWrite(pinMotor2, HIGH);
  digitalWrite(pinMotor3, HIGH);
  digitalWrite(pinMotor4, HIGH);
}

void right_only_backward() { // OK
  digitalWrite(pinMotor1, LOW);
  digitalWrite(pinMotor2, LOW);
  digitalWrite(pinMotor3, LOW);
  digitalWrite(pinMotor4, HIGH);
}
