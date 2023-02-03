int potPin = A0;
float phSense = 0;
float refvoltage = 1.1;

void setup()
{
  Serial.begin(9600);
  Serial.println("PH sensor ");
  analogReference(INTERNAL);
}

void loop()
{
  int samples = 20;
  int aRead = 0;  

  aRead = analogRead(potPin);
 
  float voltage = aRead * (5.0 / 1023.0);  // assuming 5V reference 
  
  phSense = 14 - voltage/0.25;
  
  // convert voltage to pH
  Serial.print("Analog in reading: ");
  Serial.print(voltage);
  
  // print pH value on serial monitor
  Serial.print(" - Calculated pH Level: ");
  Serial.println(phSense, 2); // 1 = one decimal, 2 = two decimals (default),etc   // removed the /10
  delay(500);
 }
