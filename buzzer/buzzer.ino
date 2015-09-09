#include <ZumoBuzzer.h>
#include <Pushbutton.h>

ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);

void setup(){
	button.waitForButton();
}

void loop(){
	// for(int i=0; i<440; i++){
	// 	buzzer.playFrequency(i, 1000, 15);
	// 	delay(1);
 //  	}

 //  	for(int i=440; i>0; i--){
	// 	buzzer.playFrequency(i, 1000, 15);
	// 	delay(1);
 //  	}

	buzzer.play("4,d5,4,p,4,p,4,a5,4,p,4,p,4,a4,4,b4,4,cx5,4,d5,4,p,4,f5,4,d5,4,p,4,f5,4,e5,4,d5,4,cx5,4,d5,4,p,4,f5,4,d5,4,p,4,f5,4,e5,4,d5,4,cx5,4,d5,4,p,4,f5,4,d5,4,f5,4,e5,4,d5,4,e5,4,f5,4,e5,4,g5,4,f5,4,e5,4,p,4,gx5,4,d5,4,p,4,f5,4,e5,4,g5,4,f5,4,e5,4,p,4,gx5,4,d5,4,p,4,f5,4,e5,4,f5,4,g5,3,a5,1,p,3,a5,1,p,3,a5,1,p,3,a5,1,p,3,a5,1,p,3,a5,1,p,4,a5,4,g5,4,f5,4,e5,4,g5,4,f5,4,e5,4,d5,4,c5,1,d5,3,c5,4,b4,4,a4,4,g5,4,p,4,p,10,g5,2,p,2,a5,0,0");
	// buzzer.playFrequency(440, 1000, 15);
  	while(buzzer.isPlaying());
  	button.waitForButton();
}