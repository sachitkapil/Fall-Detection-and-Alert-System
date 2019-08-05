#include <SoftwareSerial.h>

//GSM variables 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 //SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 //Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

//Ultrasonic Sensor variables
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance;
int count=0;

//PIR Sensor variables
int calibrationTime = 30;        
long unsigned int lowIn;         
long unsigned int pause = 10000;  //10 secs for demonstration 
boolean lockLow = true;
boolean takeLowTime;  
int pirPin = 2;    //the digital pin connected to the PIR 1 sensor's output
int ledPin = 13;

void setup() 
{
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600); // Starts the serial communication

  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("Calibrating Sensor");
    for(int i = 0; i < calibrationTime; i++)
    {
      delay(1000);
      }
    Serial.println(" ");
    Serial.println("Sensor Active");
    delay(50);
}

void loop() 
{
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(1000);
// Sets the trigPin on HIGH state for 1 second
digitalWrite(trigPin, HIGH);
delayMicroseconds(1000);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
//Prints the distance on the Serial Monitor
if (distance<10)
{
  count++;
Serial.println("---");
Serial.print("Count:");
Serial.println(count);
if(count%2==0)
{
  Serial.println
  ("Person has exited.");
}
delay(3000);
}
 if(digitalRead(pirPin) == HIGH)
 {
       digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         
         Serial.print("Motion Detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW)
     {       
       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && (millis() - lowIn > pause) && count%2!=0)
       {  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.println("Fall Detected");      //output
           //Call SMS function
           SMSAlert();
           delay(50);
           }
          
       }
}

void SMSAlert()
{
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
   
  //Serial.println("Setup Complete");
  Serial.println("\nSending SMS to Caretaker 1");
   
  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 serialSIM800.write("AT+CSMP=17,167,0,0\r\n");
 delay(1000);
  //Send new SMS command and message number
  serialSIM800.write("AT+CMGS=\"9988400442\"\r\n");
  delay(1500);
   
  //Send SMS content
  serialSIM800.write("Fall Alert");
  serialSIM800.write((char)26);
  delay(1000);
 Serial.println("SMS Sent");
 
 //Second number
 //Serial.println("Setup Complete");
  Serial.println("\nSending SMS to Caretaker 2");
   
  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 serialSIM800.write("AT+CSMP=17,167,0,0\r\n");
 delay(1000);
  serialSIM800.write("AT+CMGS=\"9779372877\"\r\n");
  delay(1500);
   
  //Send SMS content
  serialSIM800.write("Fall Alert!");
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent");
}


