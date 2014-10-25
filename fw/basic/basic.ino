
#define M_L 1
#define M_R 2

#define M_PWM_L 6
#define M_PWM_R 5
#define M_L1 9
#define M_L2 10
#define M_R1 7
#define M_R2 8
 
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

	delay(1000);
}

void loop()
{	
	if (Serial.available() > 0)
	{
		byte cmd = Serial.read();

		if (cmd = 'M')
		{
			float l = Serial.parseFloat();
			float r = Serial.parseFloat();

			Serial.print("Move: ");
			Serial.print(l);
			Serial.print(" ");
			Serial.println(r);

			motor(M_L, l);
			motor(M_R, r);
		}
		else
		{
			Serial.println("unknown cmd");
		}
	}
}



void motor(byte motor, float speed)
{
	Serial.println("");

	byte pwm = abs(speed) * 255;

	Serial.print("speed ");
	Serial.println(speed);

	Serial.print("pwm ");
	Serial.println(pwm);

	if (pwm > 1)
	{
		if (speed > 0) 
		{
			Serial.println("FWD");
			motor_drive_fwd(motor, pwm);
		} 
		else
		{
			Serial.println("BWD");
			motor_drive_bwd(motor, pwm);
		}
	}
	else
	{
		Serial.println("off");
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
		Serial.println(pwm);
		analogWrite(M_PWM_L, pwm);
		digitalWrite(M_L1, LOW);
		digitalWrite(M_L2, HIGH);
	}
	else
	{
		Serial.println(pwm);
		analogWrite(M_PWM_R, pwm);
		digitalWrite(M_R1, LOW);
		digitalWrite(M_R2, HIGH);	
	}
}

void motor_drive_bwd(byte motor, byte pwm)
{
	if (motor == M_L)
	{
		Serial.println(pwm);
		analogWrite(M_PWM_L, pwm);
		digitalWrite(M_L1, HIGH);
		digitalWrite(M_L2, LOW);
	}
	else
	{
		Serial.println(pwm);
		analogWrite(M_PWM_R, pwm);
		digitalWrite(M_R1, HIGH);
		digitalWrite(M_R2, LOW);	
	}
}
