#include <M5Stack.h>
#include <PID_v1.h>
//#include <EEPROM.h>

#define KEY_SLOW 500
#define KEY_FAST 100
#define KEY_VERY_FAST 50
#define KEY_STEP_SLOW .1
#define KEY_STEP_FAST .5

float key_step = KEY_STEP_SLOW;
int key_delay = KEY_SLOW;

#define PIN_VALVE           11
#define ANALOG_SENSOR_RH    35
//#define ANALOG_HUM_ADJUST   2
#define PID_LOOPTIME_MS     2000            // PID delta-T: 2 seconds as it was
#define DISPLAY_LOOPTIME_MS 500             // Display update rate: every half a second
#define WINDOWSIZE          5000      // time-windows size in ms

// Modi of display
#define SELECT_P  0
#define  CHANGE_P  1
#define  MEASURE  2

// Status of display. Note: variable name status or stat gives a lot of strange errors in compiling
int stts = MEASURE;
// Current parameter to change

// Define Variables for the PID controler
double Setpoint, Input, Output;
unsigned long windowStartTime;

#define KSETPOINT  0
// Define PID constants
#define Kp  1
#define Ki  2
#define Kd  3

// Current parameter to change
int Selected_Parameter = KSETPOINT ;

// P I D parameters
double para[] = {50.0, 2., 5., 1.};

// Menuitems
String item[5] = {"S", "P", "I", "D", "X"};

// Memorize place of the numbers to changes
uint16_t Para_Cusor_X[5];
uint16_t Para_Cusor_Y[5];

//define PID
PID myPID(&Input, &Output, &Setpoint, para[Kp], para[Ki], para[Kd], DIRECT);

// Sohow an menu item with current value
void DisplayMenuItem(String txt, int place, int selected)
{
  M5.Lcd.setTextSize(3);

  int y = place * 40 + 20;

  M5.Lcd.setCursor (10, y);
  if ( selected == place )
  {
    m5.Lcd.setTextColor(BLUE, WHITE);
  }
  else
  {
    m5.Lcd.setTextColor(WHITE, BLACK);
  }

  M5.Lcd.print(txt);
  if (txt != "X")
  {
    M5.Lcd.setTextSize(3);
    M5.Lcd.print(" = ");
    Para_Cusor_X[place] = M5.Lcd.getCursorX();
    Para_Cusor_Y[place] = M5.Lcd.getCursorY();
    M5.Lcd.print(para[place]);
    M5.Lcd.print("      ");
  }
}

// show every menu item with current values
void ShowMenu(int it )
{ int i;
  ShowHelp();
  M5.Lcd.setTextSize(3);
  Selected_Parameter = it;
  for (i = 0; i < 5; i++)
    DisplayMenuItem(item[i], i, Selected_Parameter);
}

void ShowHelp()
{
  m5.Lcd.setTextColor(WHITE, BLUE);
  M5.Lcd.setCursor(40,225);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("<<<    Select    >>>");
 
}

// Parameter to change will be red colored
void Change_Parameter(int item)
{
  M5.Lcd.setCursor(Para_Cusor_X[item], Para_Cusor_Y[item]);
  M5.Lcd.setTextColor(RED, WHITE);
  M5.Lcd.print(para[item]);
}

void PID_Setup()
{
  digitalWrite(PIN_VALVE, HIGH);
  
  windowStartTime = millis();
  Output = WINDOWSIZE / 2;

  // Set the range between 0 and the full window size
  myPID.SetOutputLimits(0, WINDOWSIZE);

  // Turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void setup()
{
  M5.begin();
  M5.Lcd.fillScreen(BLACK);

  pinMode(PIN_VALVE, OUTPUT);
  pinMode(ANALOG_SENSOR_RH,INPUT);

  dacWrite (25,0); // Voorkomt knullige tyfus herrie uit de m5stack.... Werkelijk waardeloos.

  analogSetWidth(10);

  PID_Setup();

}

void parameter_change()
{
  if (M5.BtnA.pressedFor(5000) or M5.BtnC.pressedFor(5000))
  {
    key_delay = KEY_VERY_FAST;
    key_step = KEY_STEP_FAST;
  }
  else if (M5.BtnA.pressedFor(2000) or M5.BtnC.pressedFor(2000))
  {
    key_delay = KEY_FAST;
  }
  else
  {
    key_delay = KEY_SLOW;
    key_step = KEY_STEP_SLOW;
  }

  if (M5.BtnA.isPressed())
  {
    para[Selected_Parameter] = para[Selected_Parameter] - key_step;
    Change_Parameter(Selected_Parameter);
    delay(key_delay);
  }
  if (M5.BtnB.wasPressed())
  {
    stts = SELECT_P;
    M5.Lcd.fillScreen(BLACK);
    ShowMenu(Selected_Parameter);
  }

  if (M5.BtnC.isPressed())
  {
    para[Selected_Parameter] = para[Selected_Parameter] + key_step;
    Change_Parameter(Selected_Parameter);
    delay(key_delay);
  }
}

void select_parameter()
{
  if (M5.BtnA.wasPressed())
  {
    Selected_Parameter = Selected_Parameter - 1;
    if ( Selected_Parameter < 0) Selected_Parameter = 4;
    ShowMenu(Selected_Parameter);
  }
  if (M5.BtnB.wasPressed())
  {
    if (item[Selected_Parameter] != "X")
    {
      stts = CHANGE_P;
      Change_Parameter(Selected_Parameter);
    }
    else
    {
      M5.Lcd.fillScreen(BLACK);
      myPID.SetTunings( para[Kp], para[Ki], para[Kd]);
      stts = MEASURE;
    }
  }
  if (M5.BtnC.wasPressed())
  {
    Selected_Parameter = Selected_Parameter + 1;
    if ( Selected_Parameter > 4) Selected_Parameter = 0;
    ShowMenu(Selected_Parameter);
  }
}

void PID_Controller()
{
  // Read the Setpoint on ANALOG_HUM_ADJUST:
  // Convert the analog reading (which goes from 0 - 1023) to voltage (between 0 and 5V)
  // float measVoltageSetPoint = analogRead(ANALOG_HUM_ADJUST) / 1023. * 5;
  float measVoltageSetPoint = para[0] / 100. * 5;
  // Apply the manufacturer's calibration
  // This has to be changed when the sensor is replaced
  float measRHSetPoint = (measVoltageSetPoint - 0.826) / 0.0315;

  // Read the humidity sensor value
  float measVoltageSensor = analogRead(ANALOG_SENSOR_RH) / 1023. * 5;
 
  float measRHSensor = (measVoltageSensor - 0.826) / 0.0315;

  unsigned long now = millis();

  /************************************************
     PID controler
   ************************************************/
  static unsigned long lastPID = 0;
  if (now - lastPID > PID_LOOPTIME_MS)
  {
    // Setpoint for the PID
    Setpoint = measRHSetPoint;

    // Input for the PID
    Input = measRHSensor;
    myPID.Compute();

    lastPID = now;
  };

  if (now - windowStartTime > WINDOWSIZE)
  { //time to shift the Relay Window
    windowStartTime += WINDOWSIZE;
  };
  if (Output > (now - windowStartTime))
  {
    // Increase
    digitalWrite(PIN_VALVE, HIGH);
  } else {
    // Decrease
    digitalWrite(PIN_VALVE, LOW);
  };

  /************************************************
     DISPLAY
   ************************************************/
  static unsigned long lastDisplay = 0;
  if (stts == MEASURE && now - lastDisplay > DISPLAY_LOOPTIME_MS)
  {
    float MeasRH  = analogRead(ANALOG_SENSOR_RH) / 1023. * 100;
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(10, 50);
    M5.Lcd.print("RH =  ");
    M5.Lcd.print(MeasRH, 1);
    M5.Lcd.print(" %  ");
    M5.Lcd.setCursor(10, 150);
    M5.Lcd.print("Set = ");
    M5.Lcd.print(para[0], 1);
    M5.Lcd.print(" %  ");
    // lcd.print(Output, 1);

    lastDisplay = now;
  };

  delay(10);

}

// stts is status...
void loop()
{
  M5.update();

  if (stts == CHANGE_P )
  {
    parameter_change();
  }
  else if (stts == SELECT_P)
  {
    select_parameter();
  }
  else if (stts == MEASURE)
  {
    if (M5.BtnA.wasPressed()|| M5.BtnB.wasPressed() ||M5.BtnC.wasPressed())
    {
      stts = SELECT_P;
      M5.Lcd.fillScreen(BLACK);
      ShowMenu(0);
    }
  }
  PID_Controller();
}
