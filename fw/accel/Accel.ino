#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#include <Wire.h>

MPU6050 mpu;

#define LED_PIN 13 
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector

float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
static void dmpDataReady() {
	mpuInterrupt = true;
}



int ImuSetup()
{
	Wire.begin();
	TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

	// initialize device
	mpu.initialize();

	if(!mpu.testConnection()){
		return -1;	
	}

	// load and configure the DMP
	devStatus = mpu.dmpInitialize();

	// supply your own gyro offsets here, scaled for min sensitivity
	mpu.setXGyroOffset(220);
	mpu.setYGyroOffset(76);
	mpu.setZGyroOffset(-85);
	mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

	// make sure it worked (returns 0 if so)
	if (devStatus == 0) {
		// turn on the DMP, now that it's ready
		mpu.setDMPEnabled(true);

		// enable Arduino interrupt detection		
		attachInterrupt(0, dmpDataReady, RISING);
		mpuIntStatus = mpu.getIntStatus();

		dmpReady = true;

		// get expected DMP packet size for later comparison
		packetSize = mpu.dmpGetFIFOPacketSize();
		
		return 0;
	} else {
		// ERROR!
		// 1 = initial memory load failed
		// 2 = DMP configuration updates failed
		// (if it's going to break, usually the code will be 1)
		//Serial.print(F("DMP Initialization failed (code "));
		//Serial.print(devStatus);
		//Serial.println(F(")"));
		return -1;
	}

}


/*
void setup(void){
	Serial.begin(9600);
	Serial.println("init");
	ImuSetup();
	Serial.println("done");
}

void loop(void){
	float i = 0;
	if(imuRead(&i) == 0){
		Serial.print(i);
	}
	
}*/

int imuRead(float *val)
{

 // if programming failed, don't try to do anything
 if (!dmpReady) return -1;

 // wait for MPU interrupt or extra packet(s) available
 if (!mpuInterrupt && fifoCount < packetSize) {
	 return -1;
 }

 // reset interrupt flag and get INT_STATUS byte
 mpuInterrupt = false;
 mpuIntStatus = mpu.getIntStatus();

 // get current FIFO count
 fifoCount = mpu.getFIFOCount();

 // check for overflow (this should never happen unless our code is too inefficient)
 if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
	 // reset so we can continue cleanly
	 mpu.resetFIFO();
	 //Serial.println(F("FIFO overflow!"));

	 // otherwise, check for DMP data ready interrupt (this should happen frequently)
 } else if (mpuIntStatus & 0x02) {
	 // wait for correct available data length, should be a VERY short wait
	 while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

	 // read a packet from FIFO
	 mpu.getFIFOBytes(fifoBuffer, packetSize);
	 
	 // track FIFO count here in case there is > 1 packet available
	 // (this lets us immediately read more without waiting for an interrupt)
	 fifoCount -= packetSize;

	 // display Euler angles in degrees
	 mpu.dmpGetQuaternion(&q, fifoBuffer); // usable
	 mpu.dmpGetGravity(&gravity, &q);
	 mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
		
		
	*val = ypr[0] * 180/M_PI;
	
	return 0;

 }


}
