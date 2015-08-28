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

// Setup will calibrate our compass by finding maximum/minimum magnetic readings
void setup(){
	buzzer.play(">g32>>c32"); // Play a little welcome song

	COMPASS_calibrate();
	// ROTATION_calibrate();

	// Play music and wait for it to finish before we start driving.
	buzzer.play(">g32>>c32");
	while(buzzer.isPlaying());
}

void loop(){
	if(is_running){
		// Heading is given in degrees away from the magnetic vector, increasing clockwise
		float heading = averageHeading();

		int direction = (heading > 180) ? 1 : -1;
		motors.setSpeeds(direction*SPEED, -direction*SPEED);

		if(heading >= -ANGLE_INTERVAL*.5 && heading <= ANGLE_INTERVAL*.5){
			buzzer.play("L16 cdegreg4");
			motors.setSpeeds(400, 400);
			while(buzzer.isPlaying());

			// randomize function
			motors.setSpeeds(-400, 400);
			delay(random(200, 1000));
			motors.setSpeeds(400, 400);
			delay(300);

			is_running = false;
		}
	}else{
		// arrête toi !
		motors.setSpeeds(0, 0);
		// attendre button pour la reprise de la boucle
		button.waitForButton();
		buzzer.play(">g32>>c32");
		is_running = true;
	}

}


void ROTATION_calibrate(){

}