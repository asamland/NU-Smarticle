#include <Servo.h>
#include <SoftwareSerial.h>

Servo myservo;
int target = 0;
String inputString;
bool stringComplete;
const int mic = A3;
const int photo = A0;
const int sampleTime = 100; 

SoftwareSerial mySerial(2,3); // RX, TX
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  myservo.attach(4, 600, 2500);
  myservo.writeMicroseconds(1500);
Serial.println("I hope it works Alex :(");
}

void loop() {
  int micOutput = findPTPAmp();
  if (micOutput>700){
    myservo.writeMicroseconds(600);
    Serial.println(micOutput);
  }
  int light = 0;
  unsigned long startTime= millis();  // Start of sample window
  while(millis() - startTime < sampleTime){
    light = max(light,analogRead(photo));
  }
  char buffer[50];
  sprintf(buffer, "light: %d\nmic: %d\n",light, micOutput);
//  Serial.print(buffer);
  if (light>900){
    Serial.println("photo!");
    myservo.writeMicroseconds(1800);
  }
  
  while (mySerial.available()) {
    // get the new byte:
    char inChar = (char)mySerial.read();
    Serial.print(inChar);
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      mySerial.print("received: ");
      mySerial.println(inputString);
      Serial.println("Xbee Command");
      myservo.writeMicroseconds(inputString.toInt());
      inputString = "";
    }
  }
  

}



// Find the Peak-to-Peak Amplitude Function
int findPTPAmp(){
// Time variables to find the peak-to-peak amplitude
   unsigned long startTime= millis();  // Start of sample window
   unsigned int PTPAmp = 0; 

// Signal variables to find the peak-to-peak amplitude
   unsigned int maxAmp = 0;
   unsigned int minAmp = 1023;

// Find the max and min of the mic output within the 50 ms timeframe
   while(millis() - startTime < sampleTime) 
   {
      int micOut = analogRead(mic);
      if( micOut < 1023) //prevent erroneous readings
      {
        if (micOut > maxAmp)
        {
          maxAmp = micOut; //save only the max reading
        }
        else if (micOut < minAmp)
        {
          minAmp = micOut; //save only the min reading
        }
      }
   }

  PTPAmp = maxAmp - minAmp; // (max amp) - (min amp) = peak-to-peak amplitude
  double micOut_Volts = (PTPAmp * 3.3) / 1024; // Convert ADC into voltage

  //Uncomment this line for help debugging (be sure to also comment out the VUMeter function)
  //Serial.println(PTPAmp); 

  //Return the PTP amplitude to use in the soundLevel function. 
  // You can also return the micOut_Volts if you prefer to use the voltage level.
  return PTPAmp;   
}
