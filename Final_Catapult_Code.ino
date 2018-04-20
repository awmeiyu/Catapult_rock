
#include <Servo.h>
#include <Stepper.h>
#include <ADCTouch.h>

//int ref0, ref1;     //reference values to remove offset
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

String mode="auto"; 
int buttonDev1; 
int buttonDev2; 
int buttonrockA;
int buttonrockB;
int buttonrockC;
const int button_mnlnorock = 38;

int buttonState = 0;  
unsigned long lastUpdate = 0;
unsigned long updateInterval = 20;
unsigned long elapsedTime = 0;

bool loaded =false;
Servo latch;
// initialize the stepper library on pins 4 through 7:
Stepper myStepper(stepsPerRevolution, 4, 5, 6, 7);

#define STATE_IDLE 0
#define STATE_LOCKNLOAD 1
#define STATE_LOCKSERVO 2
#define STATE_PRIME 3
#define STATE_READY 4
#define STATE_LAUNCH 5

#define LIMIT_SWITCH_PIN 2
#define LATCH_PIN 9

#define REEL_PIN1 4
#define REEL_PIN2 5
#define REEL_PIN3 6
#define REEL_PIN4 7

#define PRIME_TIME 3800 // How many millis to release the reel?
#define LAUNCH_TIME 1000 // How many millis will launch phase last?

int state = STATE_IDLE;
int primeTimer = 0;
int launchTimer = 0;

// constants won't change. Used here to set a pin number:
const int ledPin =  10;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 3000;           // interval at which to blink (milliseconds)
int ref0, ref1, ref2;     //reference values to remove offse
int rockA=3;
int rockB=10;
int rockC=12;
int Button=13;
int currentrock=-1;
int SCORE = 0;
int RANDOMholder = 0;
int pos = 0;    // variable to store the servo position

int touchthreshold = 30;

int counter = 0;
//int counterValue; 
int switchValue = 0; 
int Blink = 0;
int randomiser;

const int buzzer = 3;


void changeState(int newState) {
  state = newState;
  String stateString = "Unknown";
  
  switch (state) {
    case STATE_IDLE: 
      stateString = "Idle";
      break;
    case STATE_LOCKNLOAD:
      launchTimer = 0; // Reset launch timer
      stateString = "Lock n Load";
      break;
    case STATE_LOCKSERVO:
      stateString = "Lock Servo";
      break;
    case STATE_PRIME:
      primeTimer = 0; // Reset prime timer
      stateString = "Priming";
      break;
    case STATE_READY: 
      stateString = "Ready";
      break;
    case STATE_LAUNCH: 
      stateString = "Launch";
      break;
  }

    Serial.println("STATE: " + stateString); 
}


void setup() {
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);


  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  // initialize the serial port:
  Serial.begin(9600);
  
  latch.attach(LATCH_PIN);
  latch.write(0);

  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  //pinMode(buzzer, OUTPUT);

    ref0 = ADCTouch.read(A0, 100); 
    ref1 = ADCTouch.read(A1, 100);
    ref2 = ADCTouch.read(A2, 100);
    pinMode(rockA, OUTPUT);
    pinMode(rockB, OUTPUT);
    pinMode(rockC, OUTPUT);

    pinMode(Button, INPUT_PULLUP); 
    randomSeed(analogRead(A5));
    Blink = random(3,4);
    chooseRandomRock();
    Serial.print("number of times to press ");
    Serial.println(Blink);

    
  Serial.begin(9600);
  while (!Serial);
  // put your setup code here, to run once:
  //state buttons setup here
  //MODE
    pinMode(8,INPUT_PULLUP); //btnAuto
    pinMode(11,INPUT_PULLUP); //btnManual
    pinMode(13,INPUT_PULLUP); //btnDebug
    pinMode(38,INPUT_PULLUP); //btnMnlNoRock
    
  //device buttons setup here
    pinMode(A3,INPUT_PULLUP); // servo
    pinMode(A4,INPUT_PULLUP); // motor
    pinMode(A8,INPUT_PULLUP); // rockA
    pinMode(A9,INPUT_PULLUP); // rockB
    pinMode(A10,INPUT_PULLUP); // rockC

  //LED setup here
  //MODE
    pinMode(30,OUTPUT); //AUTO
    pinMode(32,OUTPUT); //MANUAL
    pinMode(34,OUTPUT); //DEBUG 
    pinMode(36,OUTPUT); //MNLNOROCK

digitalWrite(30,HIGH);
}

void loop() {
  
int button_auto=digitalRead(8);
int button_manual=digitalRead(11);
int button_debug=digitalRead(13);
int button_mnlnorock=digitalRead(38);


int buttonDev1 = digitalRead(A3);
int buttonDev2 = digitalRead(A4);
int buttonrockA = digitalRead(A8);
int buttonrockB = digitalRead(A9);
int buttonrockC = digitalRead(A10);


if (button_auto==LOW){
  mode="auto";

    //reset counter for rocks
     Blink = random(3,4);
     counter = 0; 
     
    //handle led
    Serial.println("enters A");
        killAllLights();
        //setManualLight on
        digitalWrite(30,HIGH);
        loaded=false;
  }

if(button_manual==LOW){
  mode="manual";
  
    //reset counter for rocks
      Blink = random(3,4);
      counter = 0; 
      
    //led
    Serial.println("enters M");
        killAllLights();
        //setManualLight on
        digitalWrite(32,HIGH);
        loaded=false;
    
    }

if(button_mnlnorock==LOW){
  mode="manualnorock";
      //handle led
      Serial.println("enters MWR");
          killAllLights();
          //setManualLight on
          digitalWrite(36,HIGH);
          loaded=false;
         
    }

if(button_debug==LOW){
  mode="debug";
      Serial.println("enters D");
        killAllLights();
        //setManualLight on
        digitalWrite(34 ,HIGH);
        loaded=false;
      
      }
  
  
  // put your main code here, to run repeatedly:
 if(mode.equals("auto")){
  
      // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval && Blink>counter && state == STATE_IDLE) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    //if (ledState == LOW) {
      //ledState = HIGH;
      chirp(currentrock);
    //} else {
    //  ledState = LOW;
    //}

    // set the LED with the ledState of the variable:
    //digitalWrite(ledPin, ledState);
  }
  
   int tA = ADCTouch.read(A0);
    tA -= ref0; 
    
   int tB = ADCTouch.read(A1);
    tB -= ref1; 
    
   int tC = ADCTouch.read(A2);
    tC -= ref2;

    Serial.println (tA);
    Serial.println (tB);
    Serial.println (tC);

    if (currentrock == rockA && tA > touchthreshold && counter < Blink)  {
      digitalWrite(rockA, LOW);
      noTone(rockA);
      Serial.println("0 TOUCH");
      SCORE += 1;
      Serial.println(SCORE);
      
      delay(1000);
      chooseRandomRock();
      counter++;
      
    } else if (currentrock == rockB && tB > touchthreshold && counter < Blink) {
      digitalWrite(rockB, LOW);
      noTone(rockB);
      Serial.println("1 TOUCH");
      SCORE += 1;
      Serial.println(SCORE);
      
      delay(1000);
      chooseRandomRock();
      counter++;
    } else if (currentrock == rockC && tC > touchthreshold && counter < Blink) {
      digitalWrite(rockC, LOW);
      noTone(rockC);
      Serial.println("2 TOUCH");
      SCORE += 1;
      Serial.println(SCORE);
      
      delay(1000);
      chooseRandomRock();
     counter++;
    }
  
  if (counter >= Blink || state != STATE_IDLE){
      digitalWrite(rockA, LOW);
      digitalWrite(rockB, LOW);
      digitalWrite(rockC, LOW); 
           
      noTone(rockA);
      noTone(rockB);
      noTone(rockC);
  }

  if(counter == Blink) {
    counter = 0;
    changeState(STATE_LOCKNLOAD);
  }
  
  int ButtonState=digitalRead(Button);
  if (ButtonState == LOW){
      Blink = random(1,2);
     chooseRandomRock();
      counter=0;
  }

  if(!isNextFrame()) return; 

  readInput();
  switch (state) {
    case STATE_LOCKNLOAD: lockAndLoad(); break;
    case STATE_LOCKSERVO: lockServo(); break;
    case STATE_PRIME: prime(); break;
    case STATE_LAUNCH: launch(); break;
    //case STATE_READY:
     // int value0 = ADCTouch.read(A0);   //no second parameter
    //  value0 -= ref0;       //remove offset

     // if(value0 > 40) {
       // state = STATE_LAUNCH;
      //}
      break;
  }
  }


if(mode.equals("manual")){ 
  
      // here is where you'd put code that needs to be running all the time.

      //check button values
     //button 1 pressed
        if(buttonDev1==LOW){
     //insert servo code
        latch.write(90);              // tell servo to go to position in variable 'pos'
        delay(1500);                       // waits 15ms for the servo to reach the position
        latch.write(0);              // tell servo to go to position in variable 'pos'
        delay(1500);
       changeState(STATE_IDLE);
  }
    



  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval && Blink>counter && state == STATE_IDLE) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
      chirp(currentrock);
    //} else {
    //  ledState = LOW;
    //}

    // set the LED with the ledState of the variable:
    //digitalWrite(ledPin, ledState);
  }
  
   int tA = ADCTouch.read(A0);
    tA -= ref0; 
    
   int tB = ADCTouch.read(A1);
    tB -= ref1; 
    
   int tC = ADCTouch.read(A2);
    tC -= ref2;

    Serial.println (tA);
    Serial.println (tB);
    Serial.println (tC);

    if (currentrock == rockA && tA > touchthreshold && counter < Blink)  {
      digitalWrite(rockA, LOW);
      noTone(rockA);
      Serial.println("0 TOUCH");
      SCORE += 1;
      Serial.println(SCORE);
      
      delay(1000);
      chooseRandomRock();
      counter++;
    } else if (currentrock == rockB && tB > touchthreshold && counter < Blink) {
      digitalWrite(rockB, LOW);
      noTone(rockB);
      Serial.println("1 TOUCH");
      SCORE += 1;
      Serial.println(SCORE);
      
      delay(1000);
      chooseRandomRock();
      counter++;
    } else if (currentrock == rockC && tC > touchthreshold && counter < Blink) {
      digitalWrite(rockC, LOW);
      noTone(rockC);
      Serial.println("2 TOUCH");
      SCORE += 1;
      Serial.println(SCORE);
      
      delay(1000);
      chooseRandomRock();
     counter++;
    }
  
  if (counter >= Blink || state != STATE_IDLE){
      digitalWrite(rockA, LOW);
      digitalWrite(rockB, LOW);
      digitalWrite(rockC, LOW); 
           
      noTone(rockA);
      noTone(rockB);
      noTone(rockC);
  }

  if(counter == Blink) {
    counter = 0;
    changeState(STATE_LOCKNLOAD);
  }
  
  int ButtonState=digitalRead(Button);
  if (ButtonState == LOW){
      Blink = random(1,2);
     chooseRandomRock();
      counter=0;
  }

  if(!isNextFrame()) return; 

  readInput();
  switch (state) {
    case STATE_LOCKNLOAD: lockAndLoad(); break;
    case STATE_LOCKSERVO: lockServo(); break;
    case STATE_PRIME: prime(); break;
    case STATE_LAUNCH: launch(); break;
    case STATE_READY:
       buttonState = digitalRead(buttonDev1);
        // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
       if (buttonState == LOW) {
          state = STATE_LAUNCH;
          //}
      break;
     }
  }
}

  if(mode.equals("manualnorock")){

    if(buttonDev1==LOW){
     //insert servo code
        latch.write(90);              // tell servo to go to position in variable 'pos'
        delay(1500);                       // waits 15ms for the servo to reach the position
        latch.write(0);              // tell servo to go to position in variable 'pos'
        delay(1500);
       changeState(STATE_IDLE);

     //  loaded=false;
  }
    
  //  buttonState = digitalRead(button_mnlnorock);
//    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
//    if (buttonState == LOW){
      if(loaded==false){
      changeState(STATE_LOCKNLOAD);
      }
   // }
     
     if(!isNextFrame()) return; 

  readInput();
  switch (state) {
    case STATE_LOCKNLOAD: lockAndLoad(); break;
    case STATE_LOCKSERVO: lockServo(); break;
    case STATE_PRIME: prime(); break;
    case STATE_LAUNCH: launch(); break;
    case STATE_READY:
       buttonState = digitalRead(buttonDev1);
        // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
       if (buttonState == LOW) {
        
          state = STATE_LAUNCH;
          //}
      break;
     }
  }
  

    
  }
  
  if(mode.equals("debug")){ 
      if(buttonDev2==LOW){
        Serial.println("debug Stepper");
        killRocksLights ();
       nochirp ();
        myStepper.step(-stepsPerRevolution); // Unreel the catapult rope (CCW)
        delay(1500);
    
        myStepper.step(stepsPerRevolution); // Reel in the catapult rope (CW)
        delay(1500);
      changeState(STATE_IDLE);
   
   }
   
     
     if(buttonDev1==LOW){
       Serial.println("debug Servo");
       killRocksLights ();
       nochirp ();
       //insert servo code
       latch.write(90);              // tell servo to go to position in variable 'pos'
       delay(1500);                       // waits 15ms for the servo to reach the position
       latch.write(0);              // tell servo to go to position in variable 'pos'
       delay(1500);
      changeState(STATE_IDLE);
  }

     if(buttonrockA==LOW){
        Serial.println("debug rockA");
        chirp(rockA);
        nochirp();
      
      killRocksLights();
      digitalWrite(rockA, HIGH);  
       changeState(STATE_IDLE);
       killRocksLights();       
   }

     if(buttonrockB==LOW){
       Serial.println("debug rockB");
        //nochirp();
        chirp(rockB);
        nochirp(); 
        
      killRocksLights();
      digitalWrite(rockB, HIGH);
       changeState(STATE_IDLE);
     //  killRocksLights();
   }

     if(buttonrockC==LOW){
        Serial.println("debug rockC");
       // nochirp();
        chirp(rockC);
        nochirp();

      killRocksLights();
      digitalWrite(rockC, HIGH);
       changeState(STATE_IDLE);
       killRocksLights();
    }
  }

}


void nochirp(){
      noTone(rockA);
      noTone(rockB);
      noTone(rockC);
}

void killAllLights(){
      digitalWrite(30,LOW);
      digitalWrite(32,LOW);
      digitalWrite(34,LOW);
      digitalWrite(36,LOW);
    
    }

void killRocksLights () {
      digitalWrite(rockA,LOW);
      digitalWrite(rockB,LOW);
      digitalWrite(rockC,LOW);
}

void chirp(int therock){
    tone(therock, 100);
    delay(250);
        noTone(therock);
        
   delay(100);
    tone(therock, 80);
    delay(250);
    noTone(therock);
}

void chooseRandomRock() {
  randomiser = random(0,3);
 Serial.println("R: " + randomiser);
    
  if(randomiser == 0) {
    currentrock = rockA;
    Serial.println("rockA");
  } else if (randomiser == 1) {
    currentrock = rockB;
    Serial.println("rockB");
  } else if (randomiser == 2) {
    currentrock = rockC;
    Serial.println("rockC");

  }

}
    // read the state of the pushbutton value:
void readInput() {
  if (Serial.available() > 0) {
      int input = Serial.read();

      switch (input) {
        case 48: state = STATE_IDLE; break;
        case 49: state = STATE_LOCKNLOAD; break;
        case 50: state = STATE_LOCKSERVO; break;
        case 51: state = STATE_PRIME; break;
        case 52: state = STATE_READY; break;
        case 53: state = STATE_LAUNCH; break;
        default:
          state = STATE_IDLE; break;
      }
  }
}

void launch() {  
  latch.write(0); // Set latch servo position to 0
  launchTimer += elapsedTime; // Update lauch timer
  
  if(launchTimer >= LAUNCH_TIME) { // Are we done launching?
    changeState(STATE_IDLE);
  }
}

void lockServo() {
  Serial.println("LOCKING SERVO");
  latch.write(90); // Set latch servo position to 90 to prime catapult
  delay(1000);
  changeState(STATE_PRIME);
}

void prime() {
  primeTimer += elapsedTime; // Update prime timer
  if(primeTimer >= PRIME_TIME) { // Are we done priming?
    myStepper.step(0); // Unreel the catapult rope (CCW)
    delay(1500);
    if(mode.equals("manual")||mode.equals("manualnorock")){
      changeState(STATE_READY); // Ready to launch!
    }
    else if(mode.equals("auto")){
      changeState(STATE_LAUNCH); // Time to launch!
    }
    
    return;
  }
  
  myStepper.step(-stepsPerRevolution); // Unreel the catapult rope (CCW)
  //delay(0);
}

void lockAndLoad() {
  loaded=true;
  latch.write(0); // Set latch servo position to 0
  myStepper.step(stepsPerRevolution/2); // Reel in the catapult rope (CW)
  checkLimitSwitch(); // Check limit switch
}
 
void checkLimitSwitch() {
  if(state != STATE_LOCKNLOAD) return; // Only allow check for Lock n Load state
  if(digitalRead(LIMIT_SWITCH_PIN) == LOW) { // When the reel limit is reached...
   changeState(STATE_LOCKSERVO); // Time to prime the catapult!
  }
}

bool isNextFrame() {
  elapsedTime = millis() - lastUpdate;
  if(elapsedTime < updateInterval) return false;
    
  lastUpdate = millis();
  return true;
}
