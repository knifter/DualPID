#include <PID_v1.h>
#include <LiquidCrystal.h>

#define PIN_TIP120 7
#define ANALOG_SENSOR 0
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

// Initialize liquid crystal
//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);

  windowStartTime = millis();

  // Set the range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  // Turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  // Read the Setpoint on ANALOG_HUM_ADJUST:
  // Convert the analog reading (which goes from 0 - 1023) to RH (between 0 and 100%)
  float RH = analogRead(ANALOG_HUM_ADJUST) / 1023. * 100;
  // Apply the manufacturer's calibration
  // This has to be changed when the sensor is replaced
  float setpoint_volt = RH * 0.0307 + 0.958;
  // Compute the Setpoint
  Setpoint = setpoint_volt / 5 * 1023;

  // Read the humidity sensor value
  Input = analogRead(ANALOG_SENSOR);
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
    analogWrite(PIN_TIP120, 255);
  }
  else
  {
    // Decrease
    analogWrite(PIN_TIP120, 0);
  }

  /************************************************
   * DISPLAY
   ************************************************/
  float MeasRH  = analogRead(ANALOG_SENSOR) / 1023. * 100;

  Serial.println(MeasRH);

  //         ----------------
  lcd.setCursor(0, 0);
  lcd.print("RH=    % (Meas.)");
  lcd.setCursor(3, 0);
  lcd.print(MeasRH, 1);
  lcd.setCursor(0, 1);
  lcd.print("RH=    % (SetPt)");
  lcd.setCursor(3, 1);
  lcd.print(RH, 1);

  delay(2000);
}
