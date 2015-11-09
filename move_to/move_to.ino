#include <ZumoMotors.h>
#include <ZumoBuzzer.h>
#include <Pushbutton.h>
#include <Wire.h>
#include <LSM303.h>

#define SPEED          400

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

boolean is_running = false;
unsigned long time;

void setup(){
  // play buzzer when ready
	buzzer.play(">g32>>c32");
	while(buzzer.isPlaying());
}

int s_left = 0;
int s_right = 0;
boolean invert = false;

void loop(){
	if(is_running){

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

    // zumo stops when is_running is false
		// is_running = false;
    
	}else{
		// stop
		motors.setSpeeds(0, 0);
    // idle until button press
		button.waitForButton();
		buzzer.play(">g32>>c32");
		is_running = true;

		s_left = SPEED;
		s_right = 0;
	}
	delay(5);
}
