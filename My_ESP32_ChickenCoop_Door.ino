// This script is designed for the ESP32 and channel A of an L298N motor controller.
// Its purpose is to control a chicken coop door.
// It uses a photoresistor to measure the light level and two reed swithces on the door.
// If light detected = LIGHT and door is closed, it will open the door.
// If dark detected = DARK and door is open, it will close the door.
// If door is not detected as open or close, it will flash an LED in error.


// pins assignments
const int photocellPin = 4;                  // photocell connected to analog 4
const int enableDoorMotorA = 14;          // enable motor A - pin 14 - this is the PWM pin.
const int directionCloseDoorMotorA = 26;  // direction close motor A - pin 26
const int directionOpenDoorMotorA = 27;   // direction open motor A - pin 27
const int bottomSwitchPin = 25;               // bottom switch is connected to pin 25
const int topSwitchPin = 13;                  // top switch is connected to pin 13
const int DoorOpenLed = 12;               // led set to digital pin 12
const int DoorClosedLed = 32;             // led set to digital pin 32

// Setting PWM properties
//Generate a signal of 30000 Hz on channel 0 with a 8-bit resolution. 
//Start with a duty cycle of 200 (duty cycle value from 0 to 255).
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
const int dutyCycle = 200;

// variables
// photocell
int photocellReading;                            // analog reading of the photocell
int photocellReadingLevel;                       // photocel reading levels (dark, twilight, light)

// reed switches top and bottom of coop door
int topSwitchPinVal;                   // top switch var for reading the pin status
int topSwitchPinVal2;                  // top switch var for reading the pin delay/debounce status
int topSwitchState;                    // top switch var for to hold the switch state
int bottomSwitchPinVal;                // bottom switch var for reading the pin status
int bottomSwitchPinVal2;               // bottom switch var for reading the pin delay/debounce status
int bottomSwitchState;                 // bottom switch var for to hold the switch state

// debounce delay
long lastDebounceTime = 0;
long debounceDelay = 100;

// ************************************** the setup **************************************

void setup() {

Serial.begin(115200); // initialize serial port hardware

  // welcome message
    Serial.println(" Processes running:");
  
  // coop door motor
  pinMode (directionCloseDoorMotorA, OUTPUT);   // motor close direction pin = output
  pinMode (directionOpenDoorMotorA, OUTPUT);    // motor open direction pin = output
  pinMode (enableDoorMotorA, OUTPUT);

 // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  
  // attach the pwm channel to the enableCoopDoorMotorA on GPIO14 to be controlled
  ledcAttachPin(enableDoorMotorA, pwmChannel);

  // coop door leds
  pinMode (DoorOpenLed, OUTPUT);                // enable coopDoorOpenLed = output
  pinMode (DoorClosedLed, OUTPUT);              // enable coopDoorClosedLed = output

  // coop door switches
  pinMode(bottomSwitchPin, INPUT);                  // set bottom switch pin as input
  digitalWrite(bottomSwitchPin, HIGH);              // activate bottom switch resistor
  pinMode(topSwitchPin, INPUT);                     // set top switch pin as input
  digitalWrite(topSwitchPin, HIGH);                 // activate top switch resistor  
}

// ************************************** functions **************************************

// photocel to read levels of exterior light
// function to be called repeatedly
void readPhotoCell() { 
Serial.println(" readPhotoCell function ");
  photocellReading = analogRead(photocellPin);
  Serial.print(" Photocel Analog Reading = ");
      
  //  set photocel threshholds      These need to be adjusted for real work.  Current values are for testing.
  if (photocellReading >= 0 && photocellReading <= 1000) {
    photocellReadingLevel = 1;
      Serial.print(" Photocel Reading Level:");
      Serial.println(" Dark");
    }  
  else if (photocellReading  >= 1001 && photocellReading <= 1020){
    photocellReadingLevel = 2;
      Serial.print(" Photocel Reading Level:");
      Serial.println(" Twilight");
    }  
  else if (photocellReading  >= 1021 ) {
    photocellReadingLevel = 3;
      Serial.println(photocellReadingLevel);      
      Serial.print(" Photocel Reading Level:");
      Serial.println(" Light");
    }
delay(10000);
}

// debounce top reed switch
void debounceTopReedSwitch() {
  Serial.println("debounceTopReedSwitch function");
  topSwitchPinVal = digitalRead(topSwitchPin);             // read input value and store it in val
  delay(1000);
  if ((millis() - lastDebounceTime) > debounceDelay) {     // delay 10ms for consistent readings
    topSwitchPinVal2 = digitalRead(topSwitchPin);          // read input value again to check or bounce
    if (topSwitchPinVal == topSwitchPinVal2) {             // make sure we have 2 consistant readings
      if (topSwitchPinVal != topSwitchState) {             // the button state has changed!
        topSwitchState = topSwitchPinVal;
      }
        Serial.print(" Top Switch Value: ");              // display "Bottom Switch Value:" 
        Serial.println(digitalRead(topSwitchPin));         // display current value of bottom switch;
      }
  }
}

//debounce bottom reed switch
void debounceBottomReedSwitch() {
  Serial.println("debounceBottomReedSwitch function"); 
  bottomSwitchPinVal = digitalRead(bottomSwitchPin);       // read input value and store it in bottomSwitchPinVal
  delay(1000);                                             // delay(10);
  if ((millis() - lastDebounceTime) > debounceDelay) {    // delay 10ms for consistent readings
    bottomSwitchPinVal2 = digitalRead(bottomSwitchPin);    // read input value again to check or bounce
    if (bottomSwitchPinVal == bottomSwitchPinVal2) {       // make sure we have 2 consistant readings
      if (bottomSwitchPinVal != bottomSwitchState) {       // the switch state has changed!
        bottomSwitchState = bottomSwitchPinVal;
      }
        Serial.print(" Bottom Switch Value: ");           // display "Bottom Switch Value:" 
        Serial.println(digitalRead(bottomSwitchPin));      // display current value of bottom switch;
      }
  }
}

// stop the door motor
void stopDoorMotorA(){
  Serial.println("stopDoorMotorA function");
  digitalWrite (directionCloseDoorMotorA, LOW);      // turn off motor close direction
  digitalWrite (directionOpenDoorMotorA, LOW);       // turn off motor open direction
}

// close the door motor (motor dir close = clockwise) 
void closeDoorMotorA() {  
  Serial.println("closeDoorMotorA function");
  while (bottomSwitchPinVal != 1) {
  digitalWrite (directionCloseDoorMotorA, HIGH);     // turn on motor close direction
  digitalWrite (directionOpenDoorMotorA, LOW);       // turn off motor open direction
  Serial.println(" Closing Door");
  debounceTopReedSwitch();                          // read and debounce the switches
  debounceBottomReedSwitch();
  }
  if (bottomSwitchPinVal == 1) {                         // if bottom reed switch circuit is closed
    stopDoorMotorA();
    Serial.println(" Door Closed");
  }
}

// open the door (motor dir open = counter-clockwise)
void openDoorMotorA() { 
  Serial.println("openDoorMotorA function");
  while (topSwitchPinVal != 1) {
  digitalWrite(directionCloseDoorMotorA, LOW);       // turn off motor close direction
  digitalWrite(directionOpenDoorMotorA, HIGH);       // turn on motor open direction
  Serial.println(" Opening door");
  debounceTopReedSwitch();                          // read and debounce the switches
  debounceBottomReedSwitch();
  }
  if (topSwitchPinVal == 1) {                            // if top reed switch circuit is closed
    stopDoorMotorA();
    Serial.println(" Door Open");
  }
}

// blink door red led if door is stuck 
// blink  DoorLedOpen (red)
void doDoorLedError(){
  while ((topSwitchPinVal == 0) && (bottomSwitchPinVal == 0)) { 
  Serial.println("do DoorLedError function");
  digitalWrite (DoorClosedLed, HIGH);
  digitalWrite (DoorOpenLed, HIGH);                  // blinks coopDoorOpenLed
  delay(500);
  digitalWrite (DoorOpenLed, LOW);
  delay(500);
  debounceBottomReedSwitch();                           // Check if door has moved
    if (bottomSwitchPinVal == 1) {
    doDoorLed();                                    // If bottomswitch = 1, door is closed.
    }
  debounceTopReedSwitch();
  if (topSwitchPinVal == 1) {                           //If topswitch = 1, door is open.
  doDoorLed();
}
}
}

void doDoorLed() {
  if ((bottomSwitchPinVal == 0) && (topSwitchPinVal == 1)) { // if bottom reed switch circuit is closed (door closed)
    Serial.println("do DoorLED function");
    digitalWrite (DoorClosedLed, HIGH);              // turns on coopDoorClosedLed (green)
    digitalWrite (DoorOpenLed, LOW);                 // turns off coopDoorOpenLed (red)
  }
  else if ((topSwitchPinVal == 0) && (bottomSwitchPinVal == 1)) { // if top reed switch circuit is closed (door open)
    Serial.println("do DoorLED function");
    digitalWrite (DoorClosedLed, LOW);               // turns off coopDoorClosedLed (green)
    digitalWrite (DoorOpenLed, HIGH);                // turns on coopDoorOpenLed (red)
  }  
  else if ((topSwitchPinVal == 0) && (bottomSwitchPinVal == 0)) {   // if bottom and top reed switch circuits are open 
  doDoorLedError();                                 // blink the coopDoorOpenLed
  }
}  
  
// do the door
void doDoor(){
Serial.println ("doDoor function");  
 debounceTopReedSwitch();                    // read and debounce the switches
 debounceBottomReedSwitch();  
  if ((photocellReadingLevel  == 1) && (topSwitchPinVal == 1)) {              // if it's dark and door is open
    Serial.println("call closeDoorMotorA function");      
    closeDoorMotorA();                                                       // close the door
    }
  else if ((photocellReadingLevel  == 3) && (bottomSwitchPinVal == 1)) {    // if it's light and door is closed
    Serial.println("Call openDoorMotorA function");
    openDoorMotorA();                                                       // Open the door
    }
  }
// ************************************** the loop **************************************

void loop() {
  readPhotoCell();
  delay(10000);
  doDoor();
  delay(10000);
  doDoorLed();
}
