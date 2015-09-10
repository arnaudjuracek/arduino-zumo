#include <ZumoMotors.h>
#include <ZumoBuzzer.h>
#include <Pushbutton.h>

#define SPEED 400

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

int DELAY, STATE = 0;
long PREVIOUS_MILLIS;

void setup(){
	pinMode(2, INPUT); // set tilt antenna
}


void loop(){
	switch(STATE){
		// TILTED CASE
		case -1 :
			motors.setSpeeds(0,0);
			buzzer.play(">g32>>c32");
			button.waitForButton();
			PREVIOUS_MILLIS = millis();
			STATE = 0;
			break;

		// QUEUED CASES
		case 0 :
			motors.setSpeeds(SPEED, SPEED);
			DELAY = 2000;
			break;
		case 1 :
			motors.setSpeeds(SPEED, -SPEED);
			DELAY = 3000;
			break;

		// LOOP WHEN STATE++ > DEFINED CASES
		default:
			STATE = 0;
			break;
	}

	update_timer();
}

void update_timer(){
	if(millis() - PREVIOUS_MILLIS > DELAY){
		PREVIOUS_MILLIS = millis();
		STATE++;
	}

	if(tilted()) STATE = -1;
}


boolean tilted(){
	return (digitalRead(2) == LOW);
}