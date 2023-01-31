    /*
VernierThermistor (v 2013.11)
 Reads the temperature from a Vernier Stainless Steel Temperature Probe (TMP-BTA)
 or Surface Temperature Sensor (STS-BTA) connected to the BTA connector. 
 As written, the readings will be displayed every half second. Change the variable 
 TimeBetweenReadings to change the rate.

 We use the Steinhart-Hart equation (in the function Thermistor) to determine temperature 
 from the raw A/D converter reading. Because of the use of log functions, in the Steinhart-Hart 
 equation, this sketch requires the math.h library. 

 See www.vernier.com/engineering/stem/sensors/temperature-sensor/
 for more information on how thermistors are read.

 Modifications by B. Huang (Feb 2014)
 -------------------------
 Removed the use of delays in the loop() function. Uses a timeRef variable
 Added a variable called dataRate. dataRate describes the # of samples per second.
 Added separate functions for calculating the resistance of the thermistor and then applying the
 Steinhart-hart equation.

 See www.vernier.com/arduino for more information.
 */

float dataRate = 2;             // set # of samples per second.
const char delimiter = '\t';  // delimitter character

const int ThermistorPIN = A0; // A0 for Analog1 and A2 for Analog 2
float Temp;
int rawAnalogReading;

// Variables used in the code for calculations
unsigned long timeRef;    // reference for starting time

unsigned long timeInterval;
unsigned long ndx;        // index for data counter
unsigned long thermistor;

void setup() 
{
  Serial.begin(9600);
  Serial.println("Vernier Format 2");
  Serial.println("Temperature Readings taken using Ardunio");
  Serial.println("Data Set");
  Serial.print("Time");
  Serial.print("\t"); //tab character
  Serial.println ("Temperature"); 
  Serial.print("seconds");
  Serial.print("\t"); // tab character
  Serial.println ("degrees C"); 
  timeInterval = 1000 / dataRate;  // calculates the timeInterval based on # of samples per second.
  timeRef = millis();
}

void loop() 
{
  if ((millis()) >= ndx*timeInterval + timeRef)  // controls so only runs once per timeInterval
  {
    ndx++;  
    //the print below does the division first to avoid overflows
    Serial.print((float)(millis() - timeRef) / 1000, 2); 
    rawAnalogReading = analogRead(ThermistorPIN);  // reads raw analog value from Arduino
    thermistor = resistance(rawAnalogReading);     // converts raw analog value to a resistance
    Temp = steinharthart(thermistor);              // Applies the Steinhart-hart equation

    Serial.print(delimiter); //tab character
    Serial.println(Temp,1);   // display temperature to one digit                                
  }
}
unsigned long resistance(unsigned long rawAnalogInput)
/* function to convert the raw Analog Input reading to a resistance value    
 * Schematic:
 *   [Ground] -- [thermistor] -------- | -- [15,000 ohm bridge resistor] --[Vcc (5v)]
 *                                     |
 *                                Analog Pin 0
 *
 * For the circuit above:
 * Resistance = ((rawAnalogInput*15000) /(1023 - rawAnalogInput))
 */
{
  unsigned long temp;  // temporary variable to store calculations in
  temp = (rawAnalogInput * 15000) / (1023 - rawAnalogInput);
  return temp; // returns the value calculated to the calling function.
}

float steinharthart(unsigned long resistance)
// function users steinhart-hart equation to return a temperature in degrees celsius. 
/* Inputs ADC count from Thermistor and outputs Temperature in Celsius
 * There is a huge amount of information on the web about using thermistors with the Arduino.
 * Here we are concerned about using the Vernier Stainless Steel Temperature Probe TMP-BTA and the 
 * Vernier Surface Temperature Probe STS-BTA, but the general principles are easy to extend to other
 * thermistors.
 * This version utilizes the Steinhart-Hart Thermistor Equation:
 *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}
 *   for the themistor in the Vernier TMP-BTA probe:
 *    A =0.00102119 , B = 0.000222468 and C = 1.33342E-7
 *    Using these values should get agreement within 1 degree C to the same probe used with one
 *    of the Vernier interfaces
 * 
 */
{
  float temp; // temporary variable to store calculations in
  float logRes = log(resistance); 
  // calculating logirithms is time consuming for a microcontroller - so we just
  // do this once and store it to a variable.
  float k0 = 0.00102119;
  float k1 = 0.000222468;
  float k2 = 0.000000133342; 

  temp = 1 / (k0 + k1 * logRes + k2 * logRes*logRes*logRes);
  temp = temp - 273.15;  // convert from Kelvin to Celsius 
  return temp;
}
