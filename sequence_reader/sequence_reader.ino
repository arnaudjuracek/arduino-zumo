#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <ZumoMotors.h>
#include <ZumoBuzzer.h>
#include <Pushbutton.h>
#include <Wire.h>
#include <LSM303.h>

ZumoBuzzer buzzer;
	#define PLAY_SOUND true
ZumoMotors motors;
	#define FORWARD_DURATION 30 // ms
	#define ROTATION_DURATION 230 // ms
	#define SPEED_RECORDING 100
	#define SPEED_READING_FORWARD 400
	#define SPEED_READING_ROTATION 400
	#define SPEED_CALIBRATION 200
ZumoReflectanceSensorArray reflectanceSensors;
	#define NUM_SENSORS 6
	#define THRESHOLD .8 // %
	unsigned int
		sensorValues[NUM_SENSORS],
		reflectance_position,
		reflectance_left,
		reflectance_right,
		reflectance_n,
		reflectance_avg,
		reflectance_sum;
	String
		reflectance_sequence = "",
		reflectance_sequence_sanitized = "",
		reflectance_sequence_sanitization_buffer = "";

Pushbutton button(ZUMO_BUTTON);

LSM303 compass;
	#define CALIBRATION_SAMPLES 50  // Number of compass readings to take when calibrating
	#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M value for magnetometer +/-2.5 gauss full scale
	#define CRA_REG_M_220HZ    0x1C // CRA_REG_M value for magnetometer 220 Hz update rate
	unsigned int
		last_heading = 0,
		one_turn_duration = 0;

int STATE = 0;


void setup(){
	Serial.begin(9600);
	pinMode(2, INPUT); // set tilt antenna
	pinMode(13, OUTPUT); // on board LED
	button.waitForButton();

	Serial.println("Reflectance sensors calibration...");
		reflectanceSensors.init();
		for(int i=0; i<130; i++){
			if((i > 10 && i <= 30) || (i > 50 && i <= 70) || (i > 90 && i <= 110)) motors.setSpeeds(-SPEED_CALIBRATION, -SPEED_CALIBRATION);
			else motors.setSpeeds(SPEED_CALIBRATION, SPEED_CALIBRATION);

			reflectanceSensors.calibrate();
			delay(10);
		}
		motors.setSpeeds(0,0);
	Serial.println("Calibration completed.");


	// Serial.println("Compass calibration...");
	// 	LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32767, -32767, -32767};
	// 	Wire.begin();
	// 	compass.init();
	// 	compass.enableDefault();
	// 	compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // +/- 2.5 gauss sensitivity to hopefully avoid overflow problems
	// 	compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ);    // 220 Hz compass update rate

	// 	motors.setSpeeds(SPEED_READING_FORWARD, -SPEED_READING_FORWARD);
	// 	for(int i=0; i<CALIBRATION_SAMPLES; i++){
	// 		compass.read();
	// 		running_min.x = min(running_min.x, compass.m.x);
	// 		running_min.y = min(running_min.y, compass.m.y);
	// 		running_max.x = max(running_max.x, compass.m.x);
	// 		running_max.y = max(running_max.y, compass.m.y);
	// 		delay(50);
	// 	}
	// 	compass.m_max.x = running_max.x;
	// 	compass.m_max.y = running_max.y;
	// 	compass.m_min.x = running_min.x;
	// 	compass.m_min.y = running_min.y;
	// 	motors.setSpeeds(0, 0);
	// Serial.println("Calibration completed.");


	// Serial.println("Rotation duration calibration...");
	// 	long start = millis();
	// 	long duration = 0;
	// 	int i = 0;
	// 	motors.setSpeeds(SPEED_READING_ROTATION, -SPEED_READING_ROTATION);
	// 	while(i<6){
	// 		float heading = averageHeading();
	// 		if(heading > last_heading){
	// 			// buzzer.play(">a32");
	// 			digitalWrite(13, HIGH);
	// 			duration += millis() - start;
	// 			start = millis();
	// 			last_heading = heading;
	// 			delay(800);
	// 			digitalWrite(13, LOW);
	// 			i++;
	// 		}
	// 	}
	// 	one_turn_duration = duration / 6;
	// 	motors.setSpeeds(0, 0);
	// Serial.println("Calibration completed.");


	button.waitForButton();
	STATE = -1;
}



void loop(){
	switch(STATE){

		case -10:{
			// DEBUG 90° ROTATION
			motors.setSpeeds(SPEED_READING_ROTATION, -SPEED_READING_ROTATION);
			delay(ROTATION_DURATION);
			motors.setSpeeds(0,0);
			button.waitForButton();
		}

		case -1 :{
			Serial.println("Sequence scan :");

			reflectance_left = 0;
			reflectance_right = 0;
			reflectance_sum = 0;
			reflectance_n = 0;

			reflectanceSensors.readCalibrated(sensorValues, QTR_EMITTERS_ON);
			motors.setSpeeds(SPEED_RECORDING + SPEED_RECORDING*.08, SPEED_RECORDING);

			// GET POSITION
			for(byte i=0; i<NUM_SENSORS; i++){
				if(sensorValues[i] >= 1000*THRESHOLD){
					if(i<NUM_SENSORS*.5) reflectance_left++;
					else reflectance_right++;

					reflectance_n++;
				}
				reflectance_sum += sensorValues[i];
			}

			reflectance_avg = int(reflectance_sum / NUM_SENSORS);

			// ADD CURENT POSITION TO THE LIST
			if(reflectance_n >= 5){ // ALL BLACK

				if(reflectance_sequence.startsWith("#") && !reflectance_sequence.endsWith("#")){ // LAST LINE
					if(PLAY_SOUND) buzzer.play(">a32");
					reflectance_sequence = reflectance_sequence + "#";
					STATE = 0;
				}else{ // FIRST LINE
					if(PLAY_SOUND) buzzer.play(">a32");
					reflectance_sequence = "#";
				}

			}else if(reflectance_sum < 6000 - 6000*THRESHOLD){ // ALL WHITE

				reflectance_sequence = reflectance_sequence + "-";

			}else{ // SOME BLACK, SOME WHITE

				if(reflectance_left>reflectance_right){ // LEFT
					if(PLAY_SOUND) buzzer.play(">d32");
					reflectance_sequence = reflectance_sequence + "<";
				}else{ 									// RIGHT
					if(PLAY_SOUND) buzzer.play(">g32");
					reflectance_sequence = reflectance_sequence + ">";
				}

			}

			Serial.println(reflectance_sequence);

			break;
		}


		case 0 :{
			Serial.println("Sequence conversion :");
			char c;
			boolean reflectance_sequence_started = false;
			motors.setSpeeds(0, 0);

			for(int i=0; i<reflectance_sequence.length(); i++){
				char t = reflectance_sequence[i];

				if(t == '-'){
					reflectance_sequence_started = true;

					if(reflectance_sequence_sanitization_buffer.length()>0){
						int left = 0,
							right = 0;

						for(int j=0; j<reflectance_sequence_sanitization_buffer.length(); j++){
							char s = reflectance_sequence_sanitization_buffer[j];
							if(s == '<') left++;
							if(s == '>') right++;
						}

						reflectance_sequence_sanitization_buffer = "";

						// reflectance_sequence_sanitized = reflectance_sequence_sanitized + ((left>=right)? '<' : '>');

						// longueur d'angle variable
						int	iteration = (left>=right) ? left : right;
							iteration = map(min(iteration, 8), 0, 10, 0, 4);
						for(int i=0; i<iteration; i++){
							reflectance_sequence_sanitized = reflectance_sequence_sanitized + ((left>=right)? '<' : '>');
						}
					}

					reflectance_sequence_sanitized = reflectance_sequence_sanitized + t;
					c = t;
				}else{
					if(reflectance_sequence_started){
						reflectance_sequence_sanitization_buffer = reflectance_sequence_sanitization_buffer + t;
					}
				}
			}

			Serial.println(reflectance_sequence);
			Serial.println(reflectance_sequence_sanitized);

			STATE = 2;
			break;
		}


		case 1 :{
			Serial.println("Sequence reading :");
			// playSequence(reflectance_sequence_sanitized);

			Serial.print("GO");
			for(int i=0; i<reflectance_sequence_sanitized.length(); i++){
				switch(reflectance_sequence_sanitized[i]){
					case '-' :
						Serial.print("-");
						motors.setSpeeds(SPEED_READING_FORWARD, SPEED_READING_FORWARD);
						delay(FORWARD_DURATION);
						break;
					case '<' :
						// left
						Serial.print("left");
						motors.setSpeeds(-SPEED_READING_ROTATION, SPEED_READING_ROTATION);
						// delay(one_turn_duration*.1);
						delay(ROTATION_DURATION);
						break;
					case '>' :
						// right
						Serial.print("right");
						motors.setSpeeds(SPEED_READING_ROTATION, -SPEED_READING_ROTATION);
						// delay(one_turn_duration*.1);
						delay(ROTATION_DURATION);
						break;
				}
			}
			motors.setSpeeds(0, 0);
			Serial.println("STOP");

			STATE = 2;
			break;
		}

		case 2:{
			Serial.println("End of the program.");
			Serial.println("Push button to replay, or tilt to rescan.");

			// button.waitForButton();
			// STATE = -1;
			// reflectance_sequence = "";
			// reflectance_sequence_sanitized = "";
			// reflectance_sequence_sanitization_buffer = "";

			boolean WAIT = true;
			while(WAIT == true){
				if(button.isPressed()){
					// RE-PLAY
					STATE = 1;

					WAIT = false;
				}
				if(digitalRead(2) == LOW){
					// NEW SCAN
					STATE = -1;
					reflectance_sequence = "";
					reflectance_sequence_sanitized = "";
					reflectance_sequence_sanitization_buffer = "";

					WAIT = false;
				}
			}
			break;
		}

		default :{
			STATE = 0;
			break;
		}
	}
}


void playSequence(String seq){
	for(int i=0; i<seq.length(); i++){
		switch(seq[i]){
			case '#' : buzzer.play(">a32"); break;
			case '<' : buzzer.play(">d32>g32"); break;
			case '>' : buzzer.play(">g32>d32"); break;
		}
		delay(100);
	}
}