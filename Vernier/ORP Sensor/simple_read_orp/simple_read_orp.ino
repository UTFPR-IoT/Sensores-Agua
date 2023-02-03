/*
 Analog input, serial output
 Reads an analog input pin and prints the results to the serial monitor.
 The circuit:
 Vernier probe pin 2: Ground (GND)
 Vernier probe pin 5: +5 V (VCC)
 Vernier probe pin 6: Sensor output (A0)

 created 2011-09-08 by David Hay (misterhay)
 Some code borrowed from example by Tom Igoe
  http://arduino.cc/en/Tutorial/AnalogInOutSerial

 This code is Creative Commons Attribution (http://creativecommons.org/licenses/by/3.0/)
*/

// Constants, used to give names to the pins used
const int analogInPin = A0; // Analog input pin that the probe is attached to
const int ledPin = 13; // The Teensy on-board LED is on pin 11

// Variables
int sensorValue = 0; // value read from the probe

// The setup, which runs once when the sketch starts
void setup()
{
  Serial.begin(9600); // initialize serial communications at 38400 bps,
                           // not that this matters since it runs at USB speed
  pinMode(ledPin, OUTPUT); // set the digital pin as an output
  digitalWrite(ledPin, HIGH);
}

// The actual loop that does the sampling and output to the serial monitor
// This will continue to run as long as the Teensy is plugged in
// Use the Arduino Serial Monitor or some fancy GUI to see the output
void loop()
{
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
// print the results to the serial monitor:
Serial.println(sensorValue / (5 * 1023));

// wait 10 milliseconds for the analog-to-digital converter to settle
delay(500);
}          
