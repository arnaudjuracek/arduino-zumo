#include <ZumoMotors.h>
#include <ZumoBuzzer.h>
#include <Pushbutton.h>
#include <Wire.h>
#include <LSM303.h>


#define SPEED          400
#define ANGLE_INTERVAL 20


#define CALIBRATION_SAMPLES 100  // Number of compass readings to take when calibrating
#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M value for magnetometer +/-2.5 gauss full scale
#define CRA_REG_M_220HZ    0x1C // CRA_REG_M value for magnetometer 220 Hz update rate

// Allowed deviation (in degrees) relative to target angle that must be achieved before driving straight
#define DEVIATION_THRESHOLD 10

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;
boolean is_running = false;
unsigned long time;

// Setup will calibrate our compass by finding maximum/minimum magnetic readings
void setup(){
	buzzer.play(">g32>>c32");

	// COMPASS_calibrate();

	buzzer.play(">g32>>c32");
	while(buzzer.isPlaying());

}

int s_left = 0;
int s_right = 0;
boolean invert = false;

void loop(){
	if(is_running){

		// for(int i=0; i<2; i++){
		// 	motors.setSpeeds(SPEED*.7, SPEED*.7);
		// 	delay(200);
		// 	motors.setSpeeds(-SPEED*.7, -SPEED*.7);
		// 	delay(200);
		// }

		// motors.setSpeeds(SPEED, -SPEED);
		// delay(100);

		if(!invert){
			s_left -= 2;
			s_right += 2;
		}else{
			s_left += 2;
			s_right -=2;
		}

		motors.setSpeeds(s_left, s_right);

		if(s_left<0 || s_right>SPEED) invert = true;
		if(s_right<0 || s_left>SPEED) invert = false;


		// is_running = false;
	}else{
		// arrête toi !
		motors.setSpeeds(0, 0);
		button.waitForButton();
		buzzer.play(">g32>>c32");
		is_running = true;

		s_left = SPEED;
		s_right = 0;
	}
	delay(5);
}