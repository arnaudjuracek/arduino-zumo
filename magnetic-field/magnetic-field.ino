#include <ZumoMotors.h>
#include <ZumoBuzzer.h>
#include <Pushbutton.h>
#include <Wire.h>
#include <LSM303.h>


#define SPEED          	400
#define ANGLE_INTERVAL 	20
#define TURN_BASE_SPEED 200


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

void setup() {
  Serial.begin(115200);
  buzzer.play(">g32>>c32");
  button.waitForButton();

  Serial.print("waiting for calibration");
  COMPASS_calibrate();
  Serial.print("calibration completed");
}

void loop() {
  if (is_running) {
    float heading, relative_heading;
    int speed;
    static float target_heading = averageHeading();

    // Heading is given in degrees away from the magnetic vector, increasing clockwise
    heading = averageHeading();
    // This gives us the relative heading with respect to the target angle
    relative_heading = relativeHeading(heading, target_heading);

    Serial.print(heading);
    Serial.print(";");

    if (abs(relative_heading) < DEVIATION_THRESHOLD) {
      motors.setSpeeds(SPEED, SPEED);
      delay(100);

      // Turn 90 degrees relative to the direction we are pointing.
      // This will help account for variable magnetic field, as opposed
      // to using fixed increments of 90 degrees from the initial
      // heading (which might have been measured in a different magnetic
      // field than the one the Zumo is experiencing now).
      // Note: fmod() is floating point modulo
      target_heading = fmod(averageHeading(), 360);
    }else{
      // To avoid overshooting, the closer the Zumo gets to the target
      // heading, the slower it should turn. Set the motor speeds to a
      // minimum base amount plus an additional variable amount based
      // on the heading difference.

      speed = SPEED * relative_heading / 180;

      if (speed < 0)
        speed -= TURN_BASE_SPEED;
      else
        speed += TURN_BASE_SPEED;

      motors.setSpeeds(speed, -speed);
    }
  } else {
    buzzer.play(">g32>>c32");
    button.waitForButton();
    buzzer.play(">c32>g32>");
    is_running = true;
  }
}
