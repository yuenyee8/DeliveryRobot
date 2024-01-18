#include <ezButton.h>                   // Include "ez Button Library"
#include <CytronMotorDriver.h>          // Include "Cytron Motor Library"
#include <LiquidCrystal_I2C.h>          // Include "Liquid Crystal Library"
#include <DIYables_IRcontroller.h>      // Include "DIYables_IRcontroller" Library
#include <Keypad.h>                     // Include "handleKeypad" Library

// Constants won't change
const int BUTTON_PIN      = 22;         // The number of the pushbutton pin
const int SWITCHL_PIN     = 28;         // The number of the pin for the left limit switch
const int SWITCHR_PIN     = 29;         // The number of the pin for the right limit switch
const int RED_PIN         = 23;         // The number of the red LED pin
const int GREEN_PIN       = 25;         // The number of the green LED pin
const int BLUE_PIN        = 27;         // The number of the blue LED pin
const int IR_RECEIVER_PIN = 26;         // The number of the IR pin
const int ROW_NUM         =  4;         // The number of four rows for the keypad
const int COLUMN_NUM      =  3;         // The number of three columns for the keypad

// Maker Line Sensor Pin Connection
const int LINE_D1 =  9;
const int LINE_D2 = 10;
const int LINE_D3 = 11;
const int LINE_D4 = 12;
const int LINE_D5 = 13;

// Configure the motor driver
CytronMD motorL(PWM_DIR, 3, 2);         // motorL = Left motor, PWM1 = Pin 3, DIR1 = Pin 2.
CytronMD motorR(PWM_DIR, 5, 4);         // motorR = Right motor, PWM2 = Pin 5, DIR2 = Pin 4.

// Define the pins for the ultrasonic sensor
const int trigPinL = 46;                // Trigger pin for the left ultrasonic sensor
const int echoPinL = 47;                // Echo pin for the left ultrasonic sensor
const int trigPinR = 44;                // Trigger pin for the right ultrasonic sensor
const int echoPinR = 45;                // Echo pin for the right ultrasonic sensor

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

LiquidCrystal_I2C lcd(0x27, 20, 4);                             // Set the LCD address to 0x27 for a 20 chars and 4 line display
ezButton button(BUTTON_PIN);                                    // Create ezButton object that attaches to pin 22;
ezButton limitSwitchL(SWITCHL_PIN);                             // Create ezButton object(limit switch) that attaches to pin 28;
ezButton limitSwitchR(SWITCHR_PIN);                             // Create ezButton object(limit switch) that attaches to pin 29;
DIYables_IRcontroller_21 irController(IR_RECEIVER_PIN, 200);    // Debounce time is 200ms

byte pin_rows[ROW_NUM]      = {A0, A1, A2, A3};                 // Connect to the row pinouts of the Keypad
byte pin_column[COLUMN_NUM] = {A4, A5, A6};                     // Connect to the column pinouts of the Keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Declare variables at the beginning of your code
int distanceL = 0;
int distanceR = 0;
int stateL    = 0;
int stateR    = 0;

// New global variable to track the current mode
int currentMode = 0;                    // 0: IR Controller, 1: Line Follower, 2: Keypad
int ledState = LOW;                     // The current state of LED

// Declare a variable to keep track of the button state
int buttonState = HIGH;                 // Assume the button is not pressed initially
bool buttonPressed = false;             // Variable to track if the button is pressed

// Variable to store the state of the startup display
bool displayStartup = true;
bool lineDetected = false;
bool secondLeftLineDetected = false;
bool secondRightLineDetected = false;
bool exitCondition = false;

// Variable to store the previous switch state
long duration;
int distance;

// New RGB LED color codes
const int BLINK_COLOR[3]        = {250, 0, 50};  // Red
const int LINE_FOLLOW_COLOR[3]  = {250, 200, 1}; // Purple
const int KEYPAD_COLOR[3]       = {1, 1, 128};   // Blue
const int IRCONTROLLER_COLOR[3] = {100, 150, 1}; // Green

// Function prototypes
void setColor(int R, int G, int B);
void handleIRController();
void handleLineFollower();
void handleKeypad();
void displayStartupInfo();
void toggleModes();
void Blink();
void ObstacleBlink();
void ObstacleAvoid();
void ObstacleDetect();
void avoidObstacleLeft();
void avoidObstacleRight();
void avoidObstacleFront();
void moveForward();
void moveBackward();
void moveBackwardslow();
void slightLeft();
void gentleLeft();
void gradualLeft();
void hardLeft();
void turnLeft();
void slightRight();
void gentleRight();
void gradualRight();
void hardRight();
void turnRight();
void gradualReverseLeft();
void gradualReverseRight();
void fastmoveForward();
void normalmoveBackward();
void fastslightLeft();
void fastgentleLeft();
void fastgradualLeft();
void fasthardLeft();
void fastslightRight();
void fastgentleRight();
void fastgradualRight();
void fasthardRight();
void fastturnLeft();
void fastturnRight();
void fastgradualReverseLeft();
void fastgradualReverseRight();
void Stop();
void Route1();
void Route2();
void Route3();
void Route4();
void Route6();
void Route7();
void Route9();
void uturnRoute();

// Enum for different robot modes
enum RobotMode {
  IR_CONTROL,
  LINE_FOLLOW,
  KEYPAD_CONTROL
};


// The setup routine run once
void setup() {
  Serial.begin(9600);                       // Initialize serial
  irController.begin();
  pinMode(RED_PIN, OUTPUT);                 // Set red LED pin to output mode
  pinMode(GREEN_PIN, OUTPUT);               // Set green LED pin to output mode
  pinMode(BLUE_PIN, OUTPUT);                // Set blue LED pin to output mode
  setColor(255, 255, 255);                  // Set the RGB LED to the initial color

  pinMode(LINE_D1, INPUT);
  pinMode(LINE_D2, INPUT);
  pinMode(LINE_D3, INPUT);
  pinMode(LINE_D4, INPUT);
  pinMode(LINE_D5, INPUT);

  pinMode(echoPinL, INPUT);                 // Sets the echoPin as an Input
  pinMode(echoPinR, INPUT);
  pinMode(trigPinL, OUTPUT);                // Sets the trigPin as an Output
  pinMode(trigPinR, OUTPUT);

  buttonState = digitalRead(BUTTON_PIN);
  button.setDebounceTime(50);               // Set debounce time to 50 milliseconds
  limitSwitchL.setDebounceTime(50);         // Set debounce time to 50 milliseconds
  limitSwitchR.setDebounceTime(50);         // Set debounce time to 50 milliseconds
  noTone(8);

  digitalWrite(PWM_DIR, LOW);
  motorL.setSpeed(0);                       // Set motor speeds to zero to prevent rotation
  motorR.setSpeed(0);

  lcd.init();                               // Initialize the 16x2 lcd module
  lcd.backlight();                          // Enable backlight for the LCD module
  lcd.clear();
  lcd.begin(20, 4);                         // Initialize the LCD with 20 columns and 4 rows
  displayStartupInfo();                     // Display startup information
}


// The loop routine runs over and over again
void loop() {
  int newButtonState = digitalRead(BUTTON_PIN);   // Read the current state of the button
  static bool buttonStateChanged = false;         // Flag to track if the button state has changed

  // Check if the button state has changed
  if (newButtonState != buttonState) {
    buttonState = newButtonState;                 // Update the button state

    // Check if the button is pressed and the state change is not already registered
    if (buttonState == LOW && !buttonStateChanged) {
      Stop();
      toggleModes();                              // Toggle between modes when the button is pressed
      buttonStateChanged = true;                  // Mark that the state change is registered
    } 
    else if (buttonState == HIGH) {
      buttonStateChanged = false;                 // Reset the state change flag when the button is released
    }
  }
  // Perform actions based on the current mode only if the button state hasn't changed
  if (!buttonStateChanged) {
    switch (currentMode) {
      case 0:
        handleIRController();                      // Handle IR Controller mode
        break;

      case 1:
        handleLineFollower();                      // Handle Line Follower mode
        break;

      case 2:
        handleKeypad();                            // Handle Keypad mode
        break;

      default:
        // Handle unknown mode or no mode...
        break;
    }
  }
}

/////////////////////
///StartUp Display///
/////////////////////
void displayStartupInfo() {
  Serial.println("Delivery Robot");
  Serial.println("Press 1 for Line Follow Mode");
  Serial.println("Press 2 for Keypad Mode");
  Serial.println("Press 3 for Controller Mode");

  lcd.setCursor(0, 0);
  lcd.print("Delivery Robot");
  lcd.setCursor(0, 1);
  lcd.print("1: Line Follower");
  lcd.setCursor(0, 2);
  lcd.print("2: Keypad");
  lcd.setCursor(0, 3);
  lcd.print("3: Controller");
}


/////////////////
///Push Button///
/////////////////
void toggleModes() {
  // Increment the current mode
  currentMode = (currentMode + 1) % 3;

  // Set the LED color based on the current mode
  switch (currentMode) {

    case 0:
      setColor(IRCONTROLLER_COLOR[0], IRCONTROLLER_COLOR[1], IRCONTROLLER_COLOR[2]);
      Serial.println("Controller Mode");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Controller Mode");
      break;

    case 1:
      setColor(LINE_FOLLOW_COLOR[0], LINE_FOLLOW_COLOR[1], LINE_FOLLOW_COLOR[2]);
      Serial.println("Line Follow Mode");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Line Follow Mode");
      break;

    case 2:
      setColor(KEYPAD_COLOR[0], KEYPAD_COLOR[1], KEYPAD_COLOR[2]);
      Serial.println("Keypad Mode");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Keypad Mode");
      break;

    default:
      // Handle unknown mode...
      break;
  }
}


///////////////////
///4 pin RBG LED///
///////////////////
void setColor(int R, int G, int B) {
  analogWrite(RED_PIN,   R);
  analogWrite(GREEN_PIN, G);
  analogWrite(BLUE_PIN,  B);
}


///////////////
///long data///
///////////////
long data(int trigPin, int echoPin) {       // Function to measure distance using ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  long duration = pulseIn(echoPin, HIGH);
  return duration / 58;
}


///////////////////
///IR Controller///
///////////////////
void handleIRController() {
  Key21 command = irController.getKey();
  if (command != Key21::NONE) {
    switch (command) {
      case Key21::KEY_CH_MINUS:
        Serial.println("CH-");
        fastgradualLeft();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_CH:
        Serial.println("CH");
        fastmoveForward();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_CH_PLUS:
        Serial.println("CH+");
        fastgradualRight();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_PREV:
        Serial.println("<<");
        fastturnLeft();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_NEXT:
        Serial.println(">>");
        Stop();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_PLAY_PAUSE:
        Serial.println(">||");
        fastturnRight();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_VOL_MINUS:
        Serial.println("–");
        fastgradualReverseLeft();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_VOL_PLUS:
        Serial.println("+");
        normalmoveBackward();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_EQ:
        Serial.println("EQ");
        fastgradualReverseRight();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_0:
        Serial.println("0");
        Stop();
        delay(10);
        break;

      case Key21::KEY_100_PLUS:
        Serial.println("100+");
        Stop();
        delay(10);
        break;

      case Key21::KEY_200_PLUS:
        Serial.println("200+");
        Stop();
        delay(10);
        break;

      case Key21::KEY_1:
        Serial.println("1");
        gradualLeft();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_2:
        Serial.println("2");
        moveForward();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_3:
        Serial.println("3");
        gradualRight();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_4:
        Serial.println("4");
        turnLeft();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_5:
        Serial.println("5");
        Stop();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_6:
        Serial.println("6");
        turnRight();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_7:
        Serial.println("7");
        gradualReverseLeft();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_8:
        Serial.println("8");
        moveBackwardnormal();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case Key21::KEY_9:
        Serial.println("9");
        gradualReverseRight();
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      default:
        Blink();
        break;
    }
  }
}


/////////////////////////////////////
///LineFollower with ObstacleAvoid///
/////////////////////////////////////
void handleLineFollower() {
  limitSwitchL.loop();                                // MUST call the loop() function first
  limitSwitchR.loop();                                // MUST call the loop() function first

  int distanceL = data(trigPinL, echoPinL);           // Measure distance for the Left ultrasonic sensor
  int distanceR = data(trigPinR, echoPinR);           // Measure distance for the Right ultrasonic sensor
  int stateL = limitSwitchL.getState();               // Measure distance for the Left ultrasonic sensor
  int stateR = limitSwitchR.getState();               // Measure distance for the Right ultrasonic sensor

  // Check if all line sensors detect the line
  int allLineSensors = digitalRead(LINE_D1) && digitalRead(LINE_D2) && digitalRead(LINE_D3) && digitalRead(LINE_D4) && digitalRead(LINE_D5);
  // Set the lineDetected flag
  lineDetected = allLineSensors;

  // Check if the second left line is detected
  if (digitalRead(LINE_D2) == 1 && digitalRead(LINE_D3) == 1) {
    secondLeftLineDetected = true;
  }

  // Check if the second right line is detected
  if (digitalRead(LINE_D3) == 1 && digitalRead(LINE_D4) == 1) {
    secondRightLineDetected = true;
  }

  // Print the distances to the serial monitor
  Serial.print("Distance Left: ");
  Serial.print(distanceL);
  Serial.print(" cm, Distance Right: ");
  Serial.print(distanceR);
  Serial.println(" cm");

  lcd.setCursor(0, 2);
  if (allLineSensors == 1) {
    // All line sensors detect the line, stop and ignore ultrasonic sensors
    Serial.println("All line sensors detect the line - Stopping");
    lcd.println("Stop - Line Detected");
    Stop();
    return;  // Exit the function to ignore ultrasonic sensors
  }

  if (distanceL > 0 && distanceL <= 20) {
    Serial.println("Start Avoid - Obstacle detected on the left side");
    lcd.println("Avoid Left, Turn Right");
    avoidObstacleLeft();
    noTone(8);
  }

  if (distanceR > 0 && distanceR <= 20) {
    Serial.println("Start Avoid - Obstacle detected on the right side");
    lcd.println("Avoid Right, Turn Left");
    avoidObstacleRight();
    noTone(8);
  }

  if (distanceL > 0 && distanceL <= 20 && distanceR > 0 && distanceR <= 20) {
    Serial.println("The Front have object");
    Serial.println("Start Avoid - Obstacle detected on the front");
    lcd.println("Avoid Front, move Backward");
    avoidObstacleFront();
    noTone(8);
  }

  if (stateL == LOW && stateR == HIGH) {             // HIGH when untouched, LOW when touched (1)
    Serial.println("The Left limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the left side");
    lcd.println("Avoid Left, Turn Right");
    avoidObstacleLeft();
    noTone(8);
  }

  if (stateL == HIGH && stateR == LOW) {             // HIGH when untouched, LOW when touched (1)
    Serial.println("The Right limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the right side");
    lcd.println("Avoid Right, Turn Left");
    avoidObstacleRight();
    noTone(8);
  }

  if (stateL == LOW && stateR == LOW) {             // HIGH when untouched, LOW when touched (1)
    Serial.println("The BOTH limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the front");
    lcd.println("Avoid Front, move Backward");
    avoidObstacleFront();
    noTone(8);
  }

  if (distanceL >= 20 && distanceR >= 20 && stateL == HIGH && stateR == HIGH) {
    noTone(8);
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);

    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackward();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
  }
}


////////////////////
///Obstacle Avoid///
////////////////////
void ObstacleAvoid() {                                // For LineFollower and Keypad
  limitSwitchL.loop();                                // MUST call the loop() function first
  limitSwitchR.loop();                                // MUST call the loop() function first

  int distanceL = data(trigPinL, echoPinL);           // Measure distance for the Left ultrasonic sensor
  int distanceR = data(trigPinR, echoPinR);           // Measure distance for the Right ultrasonic sensor
  int stateL = limitSwitchL.getState();               // Measure distance for the Left ultrasonic sensor
  int stateR = limitSwitchR.getState();               // Measure distance for the Right ultrasonic sensor

  // Check if all line sensors detect the line
  int allLineSensors = digitalRead(LINE_D1) && digitalRead(LINE_D2) && digitalRead(LINE_D3) && digitalRead(LINE_D4) && digitalRead(LINE_D5);

  // Print the distances to the serial monitor
  Serial.print("Distance Left: ");
  Serial.print(distanceL);
  Serial.print(" cm, Distance Right: ");
  Serial.print(distanceR);
  Serial.println(" cm");

  lcd.setCursor(0, 2);
  if (allLineSensors == 1) {
    // All line sensors detect the line, stop and ignore ultrasonic sensors
    Serial.println("All line sensors detect the line - Stopping");
    lcd.println("Stop - Line Detected");
    Stop();
    return;  // Exit the function to ignore ultrasonic sensors
  }

  if (distanceL > 0 && distanceL <= 20) {
    Serial.println("Start Avoid - Obstacle detected on the left side");
    lcd.println("Avoid Left, Turn Right");
    avoidObstacleLeft();
    noTone(8);
  }

  if (distanceR > 0 && distanceR <= 20) {
    Serial.println("Start Avoid - Obstacle detected on the right side");
    lcd.println("Avoid Right, Turn Left");
    avoidObstacleRight();
    noTone(8);
  }

  if (distanceL > 0 && distanceL <= 20 && distanceR > 0 && distanceR <= 20) {
    Serial.println("The Front have object");
    Serial.println("Start Avoid - Obstacle detected on the front");
    lcd.println("Avoid Front, move Backward");
    avoidObstacleFront();
    noTone(8);
  }

  if (stateL == LOW && stateR == HIGH) {             // HIGH when untouched, LOW when touched (1)
    Serial.println("The Left limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the left side");
    lcd.println("Avoid Left, Turn Right");
    avoidObstacleLeft();
    noTone(8);
  }

  if (stateL == HIGH && stateR == LOW) {             // HIGH when untouched, LOW when touched (1)
    Serial.println("The Right limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the right side");
    lcd.println("Avoid Right, Turn Left");
    avoidObstacleRight();
    noTone(8);
  }

  if (stateL == LOW && stateR == LOW) {             // HIGH when untouched, LOW when touched (1)
    Serial.println("The BOTH limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the front");
    lcd.println("Avoid Front, move Backward");
    avoidObstacleFront();
    noTone(8);
  }
}


////////////
///Keypad///
////////////
void handleKeypad() {
  char command = keypad.getKey();
  if (command != NO_KEY) {
    switch (command) {
      case '1':
        Serial.println("1");
        Route1();                                 // Follow the route 1
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '2':
        Serial.println("2");
        Route2();                                 // Follow the route 2
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '3':
        Serial.println("3");
        Route3();                                 // Follow the route 3
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '4':
        Serial.println("4");
        Route4();                                 // Follow the route 4
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '5':
        Serial.println("5");
        Stop();                                   // Stop
        delay(10);
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '6':
        Serial.println("6");
        Route6();                                 // Follow the route 6
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '7':
        Serial.println("7");
        Route7();                                 // Follow the route 7
        ObstacleDetect();                         // Call ObstacleDetect
        break;
        
      case '8':
        Serial.println("8");
        uturnRoute();                             // Robot U-turn
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '9':
        Serial.println("9");
        Route9();                                 // Follow the route 9
        ObstacleDetect();                         // Call ObstacleDetect
        break;

      case '*':
        Serial.println("*");
        handleLineFollower();
        break;

      case '0':
        Serial.println("0");
        uturnRoute();                                // Robot Stop
        break;

      case '#':
        Serial.println("#");
        handleIRController();
        break;

      default:
        Blink();
        break;
    }
  }
}


/////////////////////////////
///Multiple Route Follower///
/////////////////////////////
void Route1() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);
    
    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      turnLeft();
      delay(1000);
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 0) {
      turnLeft();
      delay(1000);
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
      exitCondition = true;  // Set the exit condition to true
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}

void Route2() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);
    
    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
      exitCondition = true;  // Set the exit condition to true
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}

void Route3() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);

    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
      turnRight();
      delay(800);
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      turnRight();
      delay(800);
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
      exitCondition = true;  // Set the exit condition to true
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}

void Route4() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);

    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 0) {
      turnLeft();
      delay(1000);
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
      exitCondition = true;  // Set the exit condition to true
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}

void Route6() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);

    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      turnRight();
      delay(900);
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
      exitCondition = true;  // Set the exit condition to true
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}

void Route7() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);

    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
      fastmoveForward();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 1) {
      turnLeft();
      delay(1000);
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
      exitCondition = true;  // Set the exit condition to true
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}

void Route9() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);

    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      fasthardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastgentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      fastslightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      fasthardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      fastgradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      fastgentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      fastslightRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
      fastmoveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      fastmoveForward();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      turnRight();
      delay(900);
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
      exitCondition = true;  // Set the exit condition to true
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}

void uturnRoute() {
  noTone(8);
  while (!exitCondition) {  // Loop indefinitely until a condition to exit is met
    //Perform line following
    int D1 = digitalRead(LINE_D1);
    int D2 = digitalRead(LINE_D2);
    int D3 = digitalRead(LINE_D3);
    int D4 = digitalRead(LINE_D4);
    int D5 = digitalRead(LINE_D5);

    // Print the status Maker Line to the serial monitor
    Serial.print(D1);
    Serial.print(" ");
    Serial.print(D2);
    Serial.print(" ");
    Serial.print(D3);
    Serial.print(" ");
    Serial.print(D4);
    Serial.print(" ");
    Serial.println(D5);

    turnRight();
    delay(2500);

    if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
      moveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      hardLeft();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      gradualLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      gentleLeft();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      slightLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      hardRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      gradualRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      gentleRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      slightRight();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      delay(150);
      tone(8, 523, 500);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(200);
    }
    else {
      Blink();
    }
    delay(10);
  }
  handleKeypad();                         // Code after exiting the loop
  exitCondition = false;                    // Reset the exit condition for the next iteration
}


/////////////////////
///Obstacle Detect///
/////////////////////
void ObstacleDetect() {                               // For IR Controller
  limitSwitchL.loop();                                // MUST call the loop() function first
  limitSwitchR.loop();                                // MUST call the loop() function first

  int distanceL = data(trigPinL, echoPinL);           // Measure distance for the Left ultrasonic sensor
  int distanceR = data(trigPinR, echoPinR);           // Measure distance for the Right ultrasonic sensor
  int stateL = limitSwitchL.getState();               // Measure distance for the Left ultrasonic sensor
  int stateR = limitSwitchR.getState();               // Measure distance for the Right ultrasonic sensor

  // Print the distances to the serial monitor
  Serial.print("Distance Left: ");
  Serial.print(distanceL);
  Serial.print(" cm, Distance Right: ");
  Serial.print(distanceR);
  Serial.println(" cm");

  lcd.setCursor(0, 2);
  if (distanceL > 0 && distanceL <= 20) {
    Stop();
    ObstacleBlink();
  }

  if (distanceR > 0 && distanceR <= 20) {
    Stop();
    ObstacleBlink();
  }

  if (distanceL > 0 && distanceL <= 20 && distanceR > 0 && distanceR <= 20) {
    Stop();
    ObstacleBlink();
  }

  if (stateL == LOW && stateR == HIGH) {             // HIGH when untouched, LOW when touched (1)
    Stop();
    ObstacleBlink();
  }

  if (stateL == HIGH && stateR == LOW) {             // HIGH when untouched, LOW when touched (1)
    Stop();
    ObstacleBlink();
  }

  if (stateL == LOW && stateR == LOW) {              // HIGH when untouched, LOW when touched (1)
    Stop();
    ObstacleBlink();
  }
}


//////////////////////////////
///Direction Obstacle Avoid///
//////////////////////////////
void avoidObstacleLeft() {           // Robot turn Right to avoid obstacle at left
  Stop();
  delay(100);
  moveBackwardslow();
  delay(500);
  fastturnRight();                       // turn right
  delay(500);
  fastmoveForward();
  delay(900);
  fastturnLeft();                        // move straight
  delay(700);
  fastmoveForward();
  delay(800);
  fastturnLeft();                        // turn left
  delay(700);
  fastmoveForward();
  delay(900);
  fastturnLeft();                        // turn left
  delay(500);
  moveForwardslow();
  delay(2100);
}

void avoidObstacleRight() {          // Robot turn Left to avoid obstacle at right
  Stop();
  delay(100);
  moveBackwardslow();
  delay(600);
  Stop();
  delay(100);
  fastturnLeft();                        // turn left
  delay(600);
  fastmoveForward();
  delay(1000);
  fastturnRight();                       // move straight
  delay(700);
  fastmoveForward();
  delay(1000);
  fastturnRight();                       // turn right
  delay(500);
  moveForwardslow();
  delay(2200);
}

void avoidObstacleFront() {          // Robot turn Left to avoid obstacle at front
  Stop();
  delay(100);
  moveBackwardslow();
  delay(600);
  Stop();
  delay(100);
  fastturnLeft();                        // turn left
  delay(600);
  fastmoveForward();
  delay(1000);
  fastturnRight();                       // move straight
  delay(600);
  fastmoveForward();
  delay(1500);
  fastturnRight();                       // turn right
  delay(600);
  moveForwardslow();
  delay(1250);
}


///////////////
///Direction///
///////////////
void moveForward() {
  Serial.println("Move Forward");
  lcd.setCursor(0, 3);
  lcd.print("Move Forward");
  motorL.setSpeed(150);
  motorR.setSpeed(150);              // Robot move forward
}

void moveForwardslow() {
  Serial.println("Move Forward");
  lcd.setCursor(0, 3);
  lcd.print("Move Forward");
  motorL.setSpeed(130);
  motorR.setSpeed(130);              // Robot move forward
}

void moveBackward() {
  Serial.println("Move Backward");
  lcd.setCursor(0, 3);
  lcd.print("Move Backward");
  motorL.setSpeed(-130);
  motorR.setSpeed(-130);             // Robot move backward
}

void moveBackwardnormal() {
  Serial.println("Move Backward Slow");
  lcd.setCursor(0, 3);
  lcd.print("Move Backward");
  motorL.setSpeed(-150);
  motorR.setSpeed(-150);              // Robot move backward
}

void moveBackwardslow() {
  Serial.println("Move Backward Slow");
  lcd.setCursor(0, 3);
  lcd.print("Move Backward");
  motorL.setSpeed(-100);
  motorR.setSpeed(-100);              // Robot move backward
}

void slightLeft() {
  Serial.println("Slight Left");
  lcd.setCursor(0, 3);
  lcd.print("Slight Left");
  motorL.setSpeed(30);
  motorR.setSpeed(130);              // Robot move slight left
}

void gentleLeft() {
  Serial.println("Gentle Left");
  lcd.setCursor(0, 3);
  lcd.print("Gentle Left");
  motorL.setSpeed(60);
  motorR.setSpeed(130);              // Robot move gentle left
}

void gradualLeft() {
  Serial.println("Gradual Left");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Left");
  motorL.setSpeed(90);
  motorR.setSpeed(130);              // Robot move gradual left
}

void hardLeft() {
  Serial.println("Hard Left");
  lcd.setCursor(0, 3);
  lcd.print("Hard Left");
  motorL.setSpeed(130);
  motorR.setSpeed(150);              // Robot move hard left
}

void turnLeft() {
  Serial.println("Turn Left");
  lcd.setCursor(0, 3);
  lcd.print("Turn Left");
  motorL.setSpeed(-100);
  motorR.setSpeed(100);              // Robot turn left
}

void slightRight() {
  Serial.println("Slight Right");
  lcd.setCursor(0, 3);
  lcd.print("Slight Right");
  motorL.setSpeed(130);
  motorR.setSpeed(30);                // Robot move slight right
}

void gentleRight() {
  Serial.println("Gentle Right");
  lcd.setCursor(0, 3);
  lcd.print("Gentle Right");
  motorL.setSpeed(130);
  motorR.setSpeed(60);               // Robot move gentle right
}

void gradualRight() {
  Serial.println("Gradual Right");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Right");
  motorL.setSpeed(130);
  motorR.setSpeed(90);               // Robot move gradual right
}

void hardRight() {
  Serial.println("Hard Right");
  lcd.setCursor(0, 3);
  lcd.print("Hard Right");
  motorL.setSpeed(150);
  motorR.setSpeed(130);               // Robot move hard right
}

void turnRight() {
  Serial.println("Turn Right");
  lcd.setCursor(0, 3);
  lcd.print("Turn Right");
  motorL.setSpeed(100);
  motorR.setSpeed(-100);             // Robot turn right
}

void gradualReverseLeft() {
  Serial.println("Gradual Left");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Left");
  motorL.setSpeed(-90);
  motorR.setSpeed(-130);             // Robot move gradual left
}

void gradualReverseRight() {
  Serial.println("Gradual Right");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Right");
  motorL.setSpeed(-130);
  motorR.setSpeed(-90);               // Robot move gradual right
}

void Stop() {
  Serial.println("Stop");
  lcd.setCursor(0, 3);
  lcd.print("Stop");
  motorL.setSpeed(0);
  motorR.setSpeed(0);                // Robot completely stop
}


////////////////////
///Fast Direction///
////////////////////
void fastmoveForward() {
  Serial.println("Move Forward");
  lcd.setCursor(0, 3);
  lcd.print("Move Forward");
  motorL.setSpeed(200);
  motorR.setSpeed(200);              // Robot move forward
}

void normalmoveBackward() {
  Serial.println("Move Backward");
  lcd.setCursor(0, 3);
  lcd.print("Move Backward");
  motorL.setSpeed(-170);
  motorR.setSpeed(-170);             // Robot move backward
}

void fastslightLeft() {
  Serial.println("Slight Left");
  lcd.setCursor(0, 3);
  lcd.print("Slight Left");
  motorL.setSpeed(90);
  motorR.setSpeed(180);              // Robot move slight left
}

void fastgentleLeft() {
  Serial.println("Gentle Left");
  lcd.setCursor(0, 3);
  lcd.print("Gentle Left");
  motorL.setSpeed(130);
  motorR.setSpeed(180);              // Robot move gentle left
}

void fastgradualLeft() {
  Serial.println("Gradual Left");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Left");
  motorL.setSpeed(150);
  motorR.setSpeed(180);              // Robot move gradual left
}

void fasthardLeft() {
  Serial.println("Hard Left");
  lcd.setCursor(0, 3);
  lcd.print("Hard Left");
  motorL.setSpeed(170);
  motorR.setSpeed(190);              // Robot move hard left
}

void fastslightRight() {
  Serial.println("Slight Right");
  lcd.setCursor(0, 3);
  lcd.print("Slight Right");
  motorL.setSpeed(180);
  motorR.setSpeed(90);                // Robot move slight right
}

void fastgentleRight() {
  Serial.println("Gentle Right");
  lcd.setCursor(0, 3);
  lcd.print("Gentle Right");
  motorL.setSpeed(180);
  motorR.setSpeed(130);               // Robot move gentle right
}

void fastgradualRight() {
  Serial.println("Gradual Right");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Right");
  motorL.setSpeed(180);
  motorR.setSpeed(150);               // Robot move gradual right
}

void fasthardRight() {
  Serial.println("Hard Right");
  lcd.setCursor(0, 3);
  lcd.print("Hard Right");
  motorL.setSpeed(190);
  motorR.setSpeed(170);               // Robot move hard right
}

void fastturnLeft() {
  Serial.println("Turn Left");
  lcd.setCursor(0, 3);
  lcd.print("Turn Left");
  motorL.setSpeed(-190);
  motorR.setSpeed(190);             // Robot turn left
}

void fastturnRight() {
  Serial.println("Turn Right");
  lcd.setCursor(0, 3);
  lcd.print("Turn Right");
  motorL.setSpeed(190);
  motorR.setSpeed(-190);             // Robot turn right
}

void fastgradualReverseLeft() {
  Serial.println("Gradual Left");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Left");
  motorL.setSpeed(-150);
  motorR.setSpeed(-180);             // Robot move gradual left
}

void fastgradualReverseRight() {
  Serial.println("Gradual Right");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Right");
  motorL.setSpeed(-180);
  motorR.setSpeed(-150);               // Robot move gradual right
}

////////////////////////
///LED N Buzzer Blink///
////////////////////////
void Blink() {                       // RED LED and Buzzer blink when problem
  Serial.println("WARNING: undefined command:");
  lcd.setCursor(0, 3);
  lcd.print("WARNING: undefined command:");
  setColor(BLINK_COLOR[0], BLINK_COLOR[1], BLINK_COLOR[2]);
  tone(8, 440, 200);                 // play a note on pin 8 for 200 ms:
  delay(200);                        // wait for 200 ms
  setColor(0, 0, 0);                 // turn off led
  noTone(8);                         // turn off tone function for pin 8:
  delay(200);                        // wait for 200 ms
}

void ObstacleBlink() {               // RED LED and Buzzer blink when problem
  Serial.println("WARNING: Obstacle Detect");
  lcd.setCursor(0, 3);
  lcd.print("WARNING: Obstacle Detect");
  setColor(BLINK_COLOR[0], BLINK_COLOR[1], BLINK_COLOR[2]);
  tone(8, 440, 200);                 // play a note on pin 8 for 200 ms:
  delay(200);                        // wait for 200 ms
  setColor(0, 0, 0);                 // turn off led
  noTone(8);                         // turn off tone function for pin 8:
  delay(200);                        // wait for 200 ms
}
