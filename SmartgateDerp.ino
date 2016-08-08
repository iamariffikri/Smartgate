#include "TheThingsUno.h"
#include <LiquidCrystal.h>
#include <Servo.h> 

// Declare the pins 
int servoPin = 13; 
int pirPin = 8; 
int lightPin = 8;
int greenLEDPin = 9; 
int redLEDPin = 10; 
int calibrationTime = 10;
long unsigned int pause = 5000;  
long unsigned int lowIn;         

boolean lockLow = true;
boolean takeLowTime;  
boolean isProcessingSerial;
boolean isProcessingTTN;
boolean isProcessingDownlink;

// Create a servo object 
Servo servoEngine;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Before your start, make sure that in the Tools menu, your Board and your
// Port is set to Arduino Leonardo

// After you registered your ABP device, go to The Things Network Dashboard
// and copy the Device Addr, Network Session Key and App Session Key

// Set your Device Address, for example: { 0x02, 0xDE, 0xAE, 0x00 };
const byte devAddr[4] = { ... };

// Set your Network Session Key, for example: { 0x2B, 0x7E, 0x15, 0x16, ... };
// This is used by the network to identify your device
const byte nwkSKey[16] = { ... };

// Set your Application Session Key, for example: { 0x2B, 0x7E, 0x15, 0x16, ... };
// This is used by the network for encryption
const byte appSKey[16] = { ... };

#define debugSerial Serial
#define loraSerial Serial1

#define debugPrintLn(...) { if (debugSerial) debugSerial.println(__VA_ARGS__); }
#define debugPrint(...) { if (debugSerial) debugSerial.print(__VA_ARGS__); }

TheThingsUno ttu;
bool isLightsOut;
bool isGateOpen = false;

void setup() {  
  // Set up the serial interfaces for the debugging serial monitor and LoRa module
  debugSerial.begin(115200);
  loraSerial.begin(57600);
  delay(1000);

  // Initialize and reset The Things Uno
  ttu.init(loraSerial, debugSerial);
  ttu.reset();

  // Here we activate the device with your address and keys
  ttu.personalize(devAddr, nwkSKey, appSKey);

  // Show the status on the debugging serial monitor
  ttu.showStatus();
  debugPrintLn("Setup for The Things Network complete");

  // initialize pins for LED light
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(redLEDPin, HIGH);

  // initialize pins servo engine
  servoEngine.attach(servoPin);
  
  lcd.begin(16, 2);
  lcd.print("Setting up..");
}

uint16_t getLight() {
  return analogRead(lightPin);
}

void switchGreen(bool greenLight) {
  if(greenLight) {
     digitalWrite(redLEDPin, LOW);
     digitalWrite(greenLEDPin, HIGH);
     debugPrintLn("PLATE MATCH!");
  } else {
     digitalWrite(greenLEDPin, LOW);
     digitalWrite(redLEDPin, HIGH);
  }   
}

void writeFirstRow(String displayString) {
  lcd.setCursor(0, 0); 
  while (displayString.length() < 16) {
      displayString += " ";
  }

  lcd.print(displayString);
}

void writeSecondRow(String displayString) {
  lcd.setCursor(0, 1); 
  while (displayString.length() < 16) {
      displayString += " ";
  }
  
  lcd.print(displayString);
}

void openGate(bool gateOpen) {
  if(gateOpen) {
     servoEngine.write(90);
     debugPrintLn("GATE OPENNING..");
  } else {
     servoEngine.write(0);
  }   
  isGateOpen = gateOpen;
}

void sendPlateNo(String plateNoDetected) {
  int bytes = ttu.sendString(plateNoDetected);
}

void readMotionSensor() {
     if(digitalRead(pirPin) == HIGH){
       digitalWrite(redLEDPin, HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       digitalWrite(redLEDPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(50);
           }
       }
  }

void sendToSerial(String dataString) {
    // send to serial
    Serial.println(dataString);
}

String getPlateNumber() {
    // read from serial
    String plateNo = Serial.readString();
    debugPrintLn("Plate Number Received =" + plateNo);
    return plateNo;
}

bool getGrantAccess() {
    // todo // read from serial
    int isValidNumber = Serial.read();
    return isValidNumber;
}

void loop() {
  byte data[1];
  int bytes = ttu.sendBytes(data, sizeof(data));
  if (bytes > 0 && ttu.downlink[0] == 1) {
    switchGreen(true);
    openGate(true);
    writeFirstRow("Hello There!");
    writeSecondRow("*CAR MATCHED*");
    delay(4000);
    isProcessingSerial = isProcessingTTN = false;
  } else if(bytes > 0 && ttu.downlink[0] == 2) {
    writeFirstRow("NO ACCESS :(");
    writeSecondRow(" ");
    isProcessingSerial = isProcessingTTN = false;
  } else {
    switchGreen(false);
    openGate(false);
  }

  if(getLight() < 500 && !isProcessingSerial) {
      writeSecondRow("Car detected");
      sendToSerial("action:takePicture");
      isProcessingSerial = true;
      delay(1000);
  } else if(isProcessingSerial && !isProcessingTTN) {
      writeSecondRow("Reading..");
  } else if(!isProcessingSerial && !isProcessingTTN) {
      writeFirstRow("Smartgate active");
      writeSecondRow(" ");
  }
    
  // assuming pi only pass plate number
  if(Serial.available() > 0 && !isProcessingTTN) {
    isProcessingTTN = true;
    String plateNo = getPlateNumber();

    if(plateNo.length() < 13) {
        writeFirstRow("Reading...");
      writeSecondRow(plateNo);
      sendPlateNo(plateNo);
    } else {
        writeFirstRow("Sorry,");
          writeSecondRow("*NO ACCESS*");
          delay(4000);
          isProcessingSerial = isProcessingTTN = false;
     }
}
}
