

#include <Servo.h> 

#define M_L 1
#define M_R 2

#define M_PWM_L 6
#define M_PWM_R 5
#define M_L1 9
#define M_L2 10
#define M_R1 7
#define M_R2 8


Servo servo;

#define SONAR_ECHO 2
#define SONAR_TRIGR	A0

#define SERVO_PIN 11

#define SONAR_STEPS 50
#define SONAR_MIN 20
#define SONAR_MAX 140
#define SONAR_STEP (256 - ((256 - SONAR_MAX) + SONAR_MIN)) / SONAR_STEPS
#define SONAR_SPEED 50

unsigned long measure;


void setup()
{
	Serial.begin(9600);

	pinMode(M_L1, OUTPUT);
	pinMode(M_L2, OUTPUT);
	pinMode(M_R1, OUTPUT);
	pinMode(M_R2, OUTPUT);
	pinMode(M_PWM_L, OUTPUT);
	pinMode(M_PWM_R, OUTPUT);

	motor(M_L, 0);
	motor(M_R, 0);

	pinMode(SONAR_TRIGR, OUTPUT);
	pinMode(SONAR_ECHO, INPUT);

	servo.attach(SERVO_PIN);
	servo.write(90);

	delay(1000);

	measure = millis();
}



void loop()
{	
	motor_loop();

	if (millis() - measure > SONAR_SPEED)
	{
		sonar_loop();
		measure = millis();
	}
}


int pos = 0;
bool dir = true;

void sonar_loop()
{
	Serial.print("S ");
	Serial.print(pos);
	Serial.print(" ");
	Serial.println(sonar_measure());


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
}


void motor_loop()
{
	if (Serial.available() > 0)
	{
		byte cmd = Serial.read();

		if (cmd == 'M')
		{
			float l = Serial.parseFloat();
			float r = Serial.parseFloat();

			motor(M_L, l);
			motor(M_R, r);
		}

		cmd = 0;
	}
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




void motor(byte motor, float speed)
{
	Serial.print("V motor ");
	Serial.print(motor);
	Serial.print(": ");
	Serial.println(speed);


	byte pwm = abs(speed) * 255;

	if (pwm > 1)
	{
		if (speed > 0) 
		{
			motor_drive_fwd(motor, pwm);
		} 
		else
		{
			motor_drive_bwd(motor, pwm);
		}
	}
	else
	{
		motor_off(motor);
	}
}

void motor_off(byte motor)
{
	if (motor == M_L)
	{
		analogWrite(M_PWM_L, 0);
		digitalWrite(M_L1, LOW);
		digitalWrite(M_L2, LOW);
	}
	else
	{
		analogWrite(M_PWM_R, 0);
		digitalWrite(M_R1, LOW);
		digitalWrite(M_R2, LOW);	
	}
}

void motor_drive_fwd(byte motor, byte pwm)
{
	if (motor == M_L)
	{
		analogWrite(M_PWM_L, pwm);
		digitalWrite(M_L1, LOW);
		digitalWrite(M_L2, HIGH);
	}
	else
	{
		analogWrite(M_PWM_R, pwm);
		digitalWrite(M_R1, LOW);
		digitalWrite(M_R2, HIGH);	
	}
}

void motor_drive_bwd(byte motor, byte pwm)
{
	if (motor == M_L)
	{
		analogWrite(M_PWM_L, pwm);
		digitalWrite(M_L1, HIGH);
		digitalWrite(M_L2, LOW);
	}
	else
	{
		analogWrite(M_PWM_R, pwm);
		digitalWrite(M_R1, HIGH);
		digitalWrite(M_R2, LOW);	
	}
}
