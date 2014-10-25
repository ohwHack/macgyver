

#include <Servo.h> 

Servo servo;

#define SONAR_ECHO 2
#define SONAR_TRIGR	A0

#define SERVO_PIN 11



#define SONAR_STEPS 100
#define SONAR_MIN 20
#define SONAR_MAX 140
#define SONAR_STEP (256 - ((256 - SONAR_MAX) + SONAR_MIN)) / SONAR_STEPS
 


void setup()
{
	Serial.begin(9600);

	pinMode(SONAR_TRIGR, OUTPUT);
	pinMode(SONAR_ECHO, INPUT);

	servo.attach(SERVO_PIN);
	
}

int pos = 0;
bool dir = true;


void loop()
{

	delay(50);

	int val = (SONAR_MIN + pos * SONAR_STEP);
	servo.write(val);
	
	pos += dir ? 1 : (-1);

	if (pos >= SONAR_STEPS)
	{
		dir = false;
	}
	else if (pos <= 0)
	{
		dir = true;
	}

	Serial.print("S ");
	Serial.print(pos);
	Serial.print(" ");
	Serial.println(sonar_measure());

}



float sonar_measure()
{
	digitalWrite(SONAR_TRIGR, LOW); 
	delayMicroseconds(2); 
  
	digitalWrite(SONAR_TRIGR, HIGH);
	delayMicroseconds(10); 

	digitalWrite(SONAR_TRIGR, LOW);
	float duration = pulseIn(SONAR_ECHO, HIGH, 30 * 1000);

   	//Calculate the distance (in cm) based on the speed of sound.
   	float distance = duration/58.2;

   	return distance;
}


