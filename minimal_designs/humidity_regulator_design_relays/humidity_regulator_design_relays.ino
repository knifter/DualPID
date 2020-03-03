#include <PID_v1.h>
#include <LiquidCrystal.h>

#define RELAY_INCREASE 5
#define RELAY_DECREASE 4
#define ANALOG_SENSOR_RH 0
#define ANALOG_HUM_ADJUST 1

// Define Variables for the PID controler
double Setpoint, Input, Output;
int WindowSize = 5000;
unsigned long windowStartTime;

// Define PID constants
double Kp = 2;
double Ki = 5;
double Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


void setup()
{
  windowStartTime = millis();

  // Set the range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  // Turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  // Read the Setpoint on ANALOG_HUM_ADJUST:
  // Convert the analog reading (which goes from 0 - 1023) to voltage (between 0 and 5V)
  float measVoltageSetPoint = analogRead(ANALOG_HUM_ADJUST) / 1023. * 5;
  // Apply the manufacturer's calibration
  // This has to be changed when the sensor is replaced
  float measRHSetPoint = (measVoltageSetPoint - 0.826) / 0.0315;

  // Setpoint for the PID
  Setpoint = measRHSetPoint;


  // Read the humidity sensor value
  float measVoltageSensor = analogRead(ANALOG_SENSOR_RH) / 1023. * 5;
  float measRHSensor = (measVoltageSensor - 0.826) / 0.0315;

  // Input for the PID
  Input = measRHSensor;
  myPID.Compute();

  /************************************************
   * PID controler
   ************************************************/
  unsigned long now = millis();
  if(now - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if(Output > now - windowStartTime)
  {
    // Increase
    digitalWrite(RELAY_INCREASE, HIGH);
    digitalWrite(RELAY_DECREASE, LOW);
  }
  else
  {
    // Decrease
    digitalWrite(RELAY_INCREASE, LOW);
    digitalWrite(RELAY_DECREASE, HIGH);
  }

  delay(2000);
}
