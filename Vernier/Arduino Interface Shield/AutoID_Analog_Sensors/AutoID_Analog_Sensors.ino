    /*
VernierAnalogAutoID (v 2013.12)
Reads the information to AutoID a Vernier BTA sensor with digital AutoID,
and resistor ID sensors including Vernier Stainless Steel Temperature Probe (TMP-BTA). 
It should read the +/-10 volt Voltage Probe correctly also.
This version does all tests for resistorID sensors first, then
turns on the I2C clock and tests for digital autoID sensors.)

Note that this sketch handles multiple pages of sensor calibrations.

When used with the SparkFun Vernier Interface Shield, this program will AutoID
two different sensors on BTA1 and BTA2. With homemade, breadboard
connections, it will work with only one sensor.

After the AutoID:
Assuming Vernier analog (BTA) Sensors are connected to the BTA connectors,
this sketch displays the time and sensor readings on the Serial Monitor.
As written, the readings will be displayed every second. 
Change the variable TimeBetweenReadings to change the rate.
 See www.vernier.com/arduino for more information.
*/

//#define PLX_DAQ         // uncomment this line if using PLX_DAQ for data collection.
#include <Wire.h>
#include "vernierShield.h"


unsigned int BAUD_RATE = 9600;  // set data rate for Serial monitor to be the fastest possible.

int dataRate = 60;        // set # of samples per second.
int duration = 15000;      // set the data collection duration in milliseconds
                          // default value is set to 5 seconds or 5000 milliseconds
char delimiter = '\t';

// Variables used in the code for calculations
unsigned long timeRef;    // reference for starting time
unsigned long timeInterval;

unsigned long ReadingNumber; // index for data counter


void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(muxlsb, OUTPUT);  // multiplexer pins for AutoID
  pinMode(muxmsb, OUTPUT);  // multiplexer pins for AutoID

  timeInterval = 1000 / dataRate;

#ifdef PLX_DAQ
  {
    delimiter = ',';
    BAUD_RATE = 56000;  // max speed for PLX_DAQ that matches with Arduino's data rates
  }
#endif
  Serial.begin(BAUD_RATE);
  digitalWrite(ledPin, LOW);

  device = 0x50;     // I2C Address for sensors - used for calibration data
  Serial.println("");

  // Read BTA1 Sensor
  digitalWrite(muxlsb, LOW); //set multiplexer for BTA1
  digitalWrite(muxmsb, LOW);
  BTAResistorSensorID(0);

  // Read BTA2 Sensor
  digitalWrite(muxlsb, HIGH); //set multiplexer for BTA2
  digitalWrite(muxmsb, LOW);
  BTAResistorSensorID(1);

  Wire.begin(); //start I2C communication

  // Read BTA1 Sensor
  digitalWrite(muxlsb, LOW); //set multiplexer for BTA1
  digitalWrite(muxmsb, LOW);
  if (SensorNumber[0] == 0) DigitalSensorID(0); // if no resistorID, check for digital ID

  // Read BTA2 Sensor
  digitalWrite(muxlsb, HIGH); //set multiplexer for BTA2
  digitalWrite(muxmsb, LOW);
  if (SensorNumber[1] == 0) DigitalSensorID(1); // if no resistorID, check for digital ID


  PrintSensorInfo(0);// this line can be commented out if you do not need all this info!!!
  PrintSensorInfo(1);// this line can be commented out if you do not need all this info

  PrintHeaderInfo();
  ReadingNumber = 0;
  timeRef = millis();

}

void loop()
{
  int Count[2]; //reading from 0 to 5 volt input
  int AltCount[2] ;//reading from -10 to +10V input
  float Voltage[2];
  float SensorReading[2];
  unsigned long currTime;

  currTime = millis();
  if ((currTime - timeRef) <= (duration))     // controls the duration of the data collection
  {
    if (currTime >= ReadingNumber * timeInterval + timeRef) // controls so only runs once per timeInterval
    {
      digitalWrite(ledPin, HIGH); //turn on LED
#ifdef PLX_DAQ
      {
        Serial.print("DATA,");
      }
#endif

      Serial.print((currTime - timeRef) / 1000.0, 3);
      for (int Channel = 0; Channel <= 1; Channel++)
      {
        Serial.print(delimiter); //tab character
        if (Name[Channel] == "Voltage +/- 10V")
        {
          AltCount[0] = analogRead(A1); //read both +/- 110 volt lines
          AltCount[1] = analogRead(A3);
          // convert from count to raw voltage if using 10 volt range:
          Voltage[Channel] = AltCount[Channel] / 1023.0 * 5.0 ;
        }
        else
        {
          Count[0] = analogRead(A0); //read both analog lines
          Count[1] = analogRead(A2);
          // convert from count to raw voltage on 0 to 5 range:
          Voltage[Channel] = Count[Channel] / 1023.0 * 5.0;
        }
        SensorReading[Channel] = Intercept[Channel] + Voltage[Channel] * Slope[Channel];
        //special calibration for thermistor temperture probe:
        if (SensorNumber[Channel] == 10) SensorReading[Channel] = Thermistor(Count[Channel]);
        Serial.print(SensorReading[Channel], 3);
      } // end of going through the channels

      Serial.println(" ");
      digitalWrite(ledPin, LOW);// LED on D13 flashes once per readng
      ReadingNumber++;
    }
  }
  else
  {
    digitalWrite(ledPin, LOW);

    PrintHeaderInfo();
    // reset counters and timeRef
    digitalWrite(ledPin, HIGH);
    ReadingNumber = 0;
    timeRef = millis();
  }
} // end

void BTAResistorSensorID(int Channel)
{
  Name[Channel] = ""; // clear name string
  ShortName[Channel] = ""; // clear name string
  SensorNumber[Channel] = 0;
  delay (10);
  int CountID = analogRead(A5);
  VoltageID[Channel] = CountID / 1023.0 * VCC;// convert from count to voltage
  if (VoltageID[Channel] > 0.86 & VoltageID[Channel] < 0.95) SensorNumber[Channel] = 1;
  if (VoltageID[Channel] > 3.80 & VoltageID[Channel] < 3.88) SensorNumber[Channel] = 2; // Voltage +/-10 V
  if (VoltageID[Channel] > 1.92 & VoltageID[Channel] < 2.13) SensorNumber[Channel] = 3;
  if (VoltageID[Channel] > 1.18 & VoltageID[Channel] < 1.30) SensorNumber[Channel] = 4;
  if (VoltageID[Channel] > 3.27 & VoltageID[Channel] < 3.68) SensorNumber[Channel] = 5;
  if (VoltageID[Channel] > 4.64 & VoltageID[Channel] < 4.73) SensorNumber[Channel] = 8; //Differential Voltage
  if (VoltageID[Channel] > 4.73 & VoltageID[Channel] < 4.83) SensorNumber[Channel] = 9; //Current
  if (VoltageID[Channel] > 2.38 & VoltageID[Channel] < 2.63) SensorNumber[Channel] = 10; //Stainless Steel or Surface Temperature Probe
  if (VoltageID[Channel] > 2.85 & VoltageID[Channel] < 3.15) SensorNumber[Channel] = 11;
  if (VoltageID[Channel] > 1.52 & VoltageID[Channel] < 1.68) SensorNumber[Channel] = 12; //TILT
  if (VoltageID[Channel] > 0.43 & VoltageID[Channel] < 0.48) SensorNumber[Channel] = 13;
  if (VoltageID[Channel] > 4.08 & VoltageID[Channel] < 4.16) SensorNumber[Channel] = 14;
  if (VoltageID[Channel] > 0.62 & VoltageID[Channel] < 0.68) SensorNumber[Channel] = 15;
  if (VoltageID[Channel] > 4.32 & VoltageID[Channel] < 4.40) SensorNumber[Channel] = 17;
  if (VoltageID[Channel] > 4.50 & VoltageID[Channel] < 4.59) SensorNumber[Channel] = 18; //Oxygen

  switch (SensorNumber[Channel])
  {
    case 1:
      Name[Channel] = "Thermocouple" ;
      Units[Channel] = "Deg C " ;
      ShortName[Channel] = "TC";
      Slope[Channel] = -2.45455;
      Intercept[Channel] = 6.2115;
      Page[Channel] = 1;; //calibration storage p (1,2, or 3)
      CalEquationType[Channel] = 1;
      break;
    case 2:
      Name[Channel] = "Voltage +/- 10V" ;  //!!! do not change this name or you will mess up the code of the loop
      Units[Channel] = "V" ;
      ShortName[Channel] = "Voltage10";
      Slope[Channel] = 4; //note correction for Sparkfun circuit done in calculation of Voltage!!
      Intercept[Channel] = -10;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 3:
      Name[Channel] = "Current" ;
      Units[Channel] = "Amps" ;
      ShortName[Channel] = "Current";
      Slope[Channel] = -2.665;
      Intercept[Channel] = 6.325;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 4:
      Name[Channel] = "Resistance" ;
      Units[Channel] = "Ohms" ;
      ShortName[Channel] = "Diff V";
      Slope[Channel] = -2.5;
      Intercept[Channel] = 6.25;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 8:
      Name[Channel] = "Diff Voltage" ;
      Units[Channel] = "V" ;
      ShortName[Channel] = "Diff V";
      Slope[Channel] = -2.5;
      Intercept[Channel] = 6.25;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 9:
      Name[Channel] = "Current" ;
      Units[Channel] = "Amp" ;
      ShortName[Channel] = "I";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 10:
      Name[Channel] = "Temperature" ;
      Units[Channel] = "Deg C" ;
      ShortName[Channel] = "Temp";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 11:
      Name[Channel] = "Temperature" ;
      Units[Channel] = "Deg C" ;
      ShortName[Channel] = "Temp";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 12:
      Name[Channel] = "TI Light" ;
      Units[Channel] = "relative" ;
      ShortName[Channel] = "TI Light";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 13:
      Name[Channel] = "Exercise Heart Rate" ;
      Units[Channel] = "V" ;
      ShortName[Channel] = "Ex HR";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 14:
      Name[Channel] = "Voltage" ;
      Units[Channel] = "V" ;
      ShortName[Channel] = "Volts";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 15:
      Name[Channel] = "EKG" ;
      Units[Channel] = "V" ;
      ShortName[Channel] = "EKG";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 17:
      Name[Channel] = "Carbon Dioxide" ;
      Units[Channel] = "ppm" ;
      ShortName[Channel] = "CO2";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    case 18:
      Name[Channel] = "Oxygen" ;
      Units[Channel] = "%" ;
      ShortName[Channel] = "O2";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
    default:
      Name[Channel] = "nothing on BTA" ;
      SensorNumber[Channel] = 0; //
      Units[Channel] = "" ;
      ShortName[Channel] = "";
      Slope[Channel] = 1;
      Intercept[Channel] = 0;
      Page[Channel] = 1;; //calibration storage page
      CalEquationType[Channel] = 1;
      break;
  } // end of switch case
} //end of BTA resistor check

void DigitalSensorID(int Channel)
{
  int i;
  int x;
  // check for digital ID sensor:
  for (i = 0; i < 128; i++) // clear our digital ID sensor data
  {
    sensordata[i] = 0;
  }
  //Now check for Digital AutoID sensor:
  Wire.begin(); // join i2c bus (address optional for master) !!!
  //Reading device first time... ;
  Wire.beginTransmission(device);  // Now we're going to read it back
  Wire.write(0x0);                 // Sending address 0, so it knows where we'll want
  Wire.endTransmission();          // to read from
  x = Wire.requestFrom(device, 32); // Start new transmission and keep reading for 128 bytes
  i = 1;
  Serial.print("Wire Request In: ");
  Serial.println(x);
  while (x > 1)
  {
    x = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i] = c;
    i++;
  }
  //Reading device second time... ;
  Wire.beginTransmission(device); // Now we're going to read it back
  Wire.write(0x20);               // Sending address 0, so it knows where we'll want
  Wire.endTransmission();       // to read from
  x = Wire.requestFrom(device, 32);  // Start new transmission and keep reading for 128 bytes
  i = 1;
  while (x > 1)
  {
    x = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i + 32] = c;
    i++;
  }
  //Reading device third time... ;
  Wire.beginTransmission(device); // Now we're going to read it back
  Wire.write(0x40);               // Sending address 0, so it knows where we'll want
  Wire.endTransmission();         // to read from
  x = Wire.requestFrom(device, 32); // Start new transmission and keep reading for 128 bytes
  i = 1;
  while (x > 1)
  {
    x = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i + 64] = c;
    i++;
  }
  //Reading device a forth time... ;
  Wire.beginTransmission(device); // Now we're going to read it back
  Wire.write(0x60);               // Sending address 0, so it knows where we'll want
  Wire.endTransmission();       // to read from
  x = Wire.requestFrom(device, 32);  // Start new transmission and keep reading for 128 bytes
  i = 1;
  while (x > 1)
  {
    x = Wire.available();
    char c = Wire.read();    // Read a byte and write it out to the Serial port
    sensordata [i + 96] = c;
    i++;
  }
  //      Print out array:  // remove *'s to get this display for diagnostics

#ifdef DEBUG

  Serial.println("array: ");
  for (i = 1; i <= 128; i++)
  {
    Serial.print (i);
    Serial.print ("\t ");
    Serial.print(sensordata[i], DEC);
    Serial.print ("\t ");
    Serial.println(char(sensordata[i]));
  }
#endif

  //******************************************************************
  //Determine sensor number:
  //  VoltageID[Channel]=-1;// indicated resistor ID not used
  SensorNumber[Channel] = sensordata[2];

  //Determine the sensor name:
  Name[Channel] = "";
  for (i = 0; i < 20; i++)
  {
    char c =  sensordata[i + 9]; // 9 byte offset
    Name[Channel] += c;
  }
  Name[Channel] += '\0';    //add terminating character

  //Determine the short name:
  ShortName[Channel] = "";
  for (i = 0; i < 12; i++)
  {
    char c =  sensordata[i + 29]; // 29 byte offset
    ShortName[Channel] += c;
  }
  ShortName[Channel] += '\0';    //add terminating character

  //Determine the calibration equation type:
  CalEquationType[Channel] = sensordata[57];

  //Determines the  calibration page:
  Page[Channel] = sensordata[70];

  // the code below uses the calibration page set:
  // Intercept starts at 71 for page 1, 90 for p2, and 109 for p3

  //Determines intercept:
  for (i = 0; i < 4; i++)
  {
    floatbyte[i] = sensordata[i + 71 + (Page[Channel]) * 19];
  }
  float j = *(float*) &floatbyte;
  Intercept[Channel] = j;

  //Determines slope:
  // slope starts at 75 for page 1, 94 for p2, and 113 for p3
  for (i = 0; i < 4; i++)
  {
    floatbyte[i] = sensordata[i + 75 + (Page[Channel]) * 19];
  }
  float y = *(float*) &floatbyte;
  Slope[Channel] = y;

  //Determines the units:
  // the cryptic code in the next line just uses the
  // correct bytes, depending on the page selected.
  // units start at 83 for page 1, 102 for p2, and 121 for p3
  for (i = 0; i < 7; i++)
  {
    char c =  sensordata[83 + i + (Page[Channel]) * 19];
    Units[Channel] += c;
  }
  Units[Channel] += '\0';    //add terminating character
} //end of checking for digital ID sensor

void PrintHeaderInfo()
{
  Serial.println();
  Serial.println("**************************************************");
  Serial.println("*   Push the D12 button to start collection.     *");
  Serial.println("*      Use reset button to reset / stop          *");
  Serial.println("**************************************************");
  while (digitalRead(buttonPin) == HIGH)
  {
    // holding loop until the button goes low.
  }
  Serial.println("Vernier Format 2");
  Serial.println("Sensor Readings taken using Ardunio");
  Serial.println("Data Set");

#ifdef PLX_DAQ
  {
    Serial.println("CLEARDATA");
    Serial.print("LABEL,");
  }
#endif

  Serial.print("Time");
  for (int chan = 0; chan <= 1; chan++)
  {
    Serial.print(delimiter); //tab character
    //print sensor name:
    Serial.print (ShortName[chan]);
  }
  Serial.println("");

  Serial.print("seconds");

  for (int chan = 0; chan <= 1; chan++)
  {
    Serial.print("\t"); //tab character
    //print sensor name:
    Serial.print (Units[chan]);
  }
  Serial.println ("");
}


void PrintSensorInfo(int Channel)
{ // print out information about sensor:
  //(This code is commented out, but add it for more feedback)
  Serial.println(" ");
  Serial.print("BTA Connector ");
  Serial.println(Channel);
  Serial.print("sensor ID number: ");
  Serial.println(SensorNumber[Channel]);
  Serial.print("ID voltage level: ");
  Serial.println(VoltageID[Channel]);
  Serial.print("sensor name: ");
  Serial.println (Name[Channel]);
  Serial.print("sensor short name: ");
  Serial.println (ShortName[Channel]);
  Serial.print("calibration page: ");
  Serial.println(Page[Channel]);
  Serial.print("calibration equation type: ");
  Serial.println(CalEquationType[Channel]);
  Serial.print("intercept: ");
  Serial.println (Intercept[Channel]);
  Serial.print("slope ");
  Serial. println(Slope[Channel]);
  Serial.print("units: ");
  Serial.println (Units[Channel]);
}// end of PrintSensorInfo

float Thermistor(int Raw) //This function calculates temperature from ADC count
{
  /* Inputs ADC count from Thermistor and outputs Temperature in Celsius
  *  requires: include <math.h>
  * There is a huge amount of information on the web about using thermistors with the Arduino.
  * Here we are concerned about using the Vernier Stainless Steel Temperature Probe TMP-BTA and the
  * Vernier Surface Temperature Probe STS-BTA, but the general principles are easy to extend to other
  * thermistors.
  * This version utilizes the Steinhart-Hart Thermistor Equation:
  *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]3}
  *   for the themistor in the Vernier TMP-BTA probe:
  *    A =0.00102119 , B = 0.000222468 and C = 1.33342E-7
  *    Using these values should get agreement within 1 degree C to the same probe used with one
  *    of the Vernier interfaces
  *
  * Schematic:
  *   [Ground] -- [thermistor] -------- | -- [15,000 ohm bridge resistor] --[Vcc (5v)]
  *                                     |
  *                                Analog Pin 0
  *

  For the circuit above:
  * Resistance = ( Count*RawADC /(1024-Count))
   */
  long Resistance;
  float Resistor = 15000; //bridge resistor
  // the measured resistance of your particular bridge resistor in
  //the Vernier BTA-ELV this is a precision 15K resisitor
  float Temp;  // Dual-Purpose variable to save space.
  Resistance = ( Resistor * Raw / (1024 - Raw));
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.00102119 + (0.000222468 * Temp) + (0.000000133342 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius
  return Temp;                                      // Return the Temperature
}
