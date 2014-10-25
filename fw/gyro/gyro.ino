#include <Servo.h> 
#include "Accel.h"

#define M_L 1
#define M_R 2

#define M_PWM_L 6
#define M_PWM_R 5
#define M_L1 9
#define M_L2 10
#define M_R1 7
#define M_R2 8



#define SONAR_ECHO 2
#define SONAR_TRIGR	A0

#define SERVO_PIN 11

#define SONAR_STEPS 50
#define SONAR_MIN 20
#define SONAR_MAX 140
#define SONAR_STEP (256 - ((256 - SONAR_MAX) + SONAR_MIN)) / SONAR_STEPS
#define SONAR_SPEED 50


Servo servo;

unsigned long measure;



float real_angle;

float angle = 0;
float speed = 0;

float motor_l = 0;
float motor_r = 0;

const float reg_p = 0.01;

#define REG_SPEED 50
unsigned long reg_time;



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

	pinMode(A4, INPUT_PULLUP);
	pinMode(A5, INPUT_PULLUP);

	servo.attach(SERVO_PIN);
	servo.write(70);

	ImuSetup();

	delay(1000);

	reg_time = millis();
}




void loop()
{	
	comm_loop();

	// relative	
	if (imuRead(&real_angle) < 0) {
		Serial.println("E imm");
		return;
	}

	if (millis() - reg_time > REG_SPEED)
	{
		reg_loop();
		reg_time = millis();
	}
}


void comm_loop()
{
	if (Serial.available() > 0)
	{
		byte cmd = Serial.read();

		if (cmd == 'M')
		{
			motor_l = Serial.parseFloat();
			motor_r = Serial.parseFloat();
		}
		else if (cmd == 'R')
		{
			float a = Serial.parseFloat();
			speed = Serial.parseFloat();

			if (a >= -1 || a <= 1) 
			{
				angle += (a * 20);
			}
		}
		else if (cmd == 'C')
		{
			int a = Serial.parseInt();
			servo.write(constrain(a, 20, 140));
		}

		cmd = 0;
	}
}


void reg_loop()
{
	Serial.print("A ");
	Serial.println(real_angle);

	if (abs(speed) < 0.01) 
	{
		// manual
		motor(M_L, motor_l);
		motor(M_R, motor_r);

		angle = real_angle;
	}
	else
	{
	
		float da = angle - real_angle;

		Serial.print("D ");
		Serial.println(da);


		float sl = speed - da * reg_p;
		float sr = speed + da * reg_p;

		motor(M_L, crop(sl));
		motor(M_R, crop(sr));
	}
	
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


float crop(float v)
{
	return max(min(v, 1.0), -1.0);
}
