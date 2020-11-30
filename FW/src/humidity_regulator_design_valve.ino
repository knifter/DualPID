#include <Arduino.h>
#include <Wire.h>

#include <PID_v1.h>
#include <LiquidCrystal.h>

#define PIN_VALVE           11
#define ANALOG_SENSOR_RH    1
#define ANALOG_HUM_ADJUST   2
#define PID_LOOPTIME_MS     2000            // PID delta-T: 2 seconds as it was
#define DISPLAY_LOOPTIME_MS 500             // Display update rate: every half a second
#define WINDOWSIZE          5000			// time-windows size in ms

// Define Variables for the PID controler
double Setpoint, Input, Output;
unsigned long windowStartTime;

// Define PID constants
double Kp = 2;
double Ki = 5;
double Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Initialize liquid crystal
LiquidCrystal lcd(/*RS*/ 8, /*enb*/ 9, /*D4*/ 4, /*D5*/ 5, /*D6*/ 6, /*D7*/ 7);

void setup()
{
    lcd.begin(16, 2);

    digitalWrite(PIN_VALVE, HIGH);
    pinMode(PIN_VALVE, OUTPUT);

    windowStartTime = millis();
    Output = WINDOWSIZE / 2;

    // Set the range between 0 and the full window size
    myPID.SetOutputLimits(0, WINDOWSIZE);

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

    // Read the humidity sensor value
    float measVoltageSensor = analogRead(ANALOG_SENSOR_RH) / 1023. * 5;
    float measRHSensor = (measVoltageSensor - 0.826) / 0.0315;

    unsigned long now = millis();

    /************************************************
     * PID controler
     ************************************************/
    static unsigned long lastPID = 0;
    if(now - lastPID > PID_LOOPTIME_MS)
    {
        // Setpoint for the PID
        Setpoint = measRHSetPoint;

        // Input for the PID
        Input = measRHSensor;
        myPID.Compute();

        lastPID = now;
    };

    if(now - windowStartTime > WINDOWSIZE)
    { //time to shift the Relay Window
        windowStartTime += WINDOWSIZE;
    };
    if(Output > (now - windowStartTime))
    {
        // Increase
        digitalWrite(PIN_VALVE, HIGH);
    }else{
        // Decrease
        digitalWrite(PIN_VALVE, LOW);
    };

    /************************************************
     * DISPLAY
     ************************************************/
    static unsigned long lastDisplay = 0;
    if(now - lastDisplay > DISPLAY_LOOPTIME_MS)
    {
        float MeasRH  = analogRead(ANALOG_SENSOR_RH) / 1023. * 100;

        lcd.setCursor(0, 0);
        lcd.print("RH=    % (Meas.)");
        lcd.setCursor(3, 0);
        lcd.print(MeasRH, 1);
        lcd.setCursor(0, 1);
        lcd.print("RH=    % (SetPt)");
        lcd.setCursor(3, 1);
        lcd.print(measRHSetPoint, 1);
        // lcd.print(Output, 1);

        lastDisplay = now;
    };

    delay(10);
};
