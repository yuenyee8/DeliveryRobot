#include <ezButton.h>                   // Include "ez Button Library"
#include <CytronMotorDriver.h>          // Include "Cytron Motor Library"
#include <LiquidCrystal_I2C.h>          // Include "Liquid Crystal Library"
#include <DIYables_IRcontroller.h>      // Include "DIYables_IRcontroller" Library
#include <Keypad.h>                     // Include "Keypad" Library

/// Constants won't change
const int BUTTON_PIN = 22;              // The number of the pushbutton pin
const int SWITCHL_PIN = 28;             // The number of the pushbutton pin
const int SWITCHR_PIN = 29;             // The number of the pushbutton pin
const int RED_PIN = 23;                 // The number of the red LED pin
const int YELLOW_PIN = 25;              // The number of the red LED pin
const int GREEN_PIN = 27;               // The number of the green LED pin
const int IR_RECEIVER_PIN = 24;         // The number of the IR pin
const int ROW_NUM = 4;                  // The number of the keypad four rows
const int COLUMN_NUM = 3;               // The number of the keypad three columns

// Maker Line Sensor Pin Connection
int LINE_D1 = 9;
int LINE_D2 = 10;
int LINE_D3 = 11;
int LINE_D4 = 12;
int LINE_D5 = 13;

// Configure the motor driver.
CytronMD motorL(PWM_DIR, 3, 2);         // motorL = Left motor, PWM1 = Pin 11, DIR1 = Pin 10.
CytronMD motorR(PWM_DIR, 5, 4);         // motorR = Right motor, PWM2 = Pin 13, DIR2 = Pin 12.

// Define the pins for the ultrasonic sensor
const int trigPinL = 46;                // Left
const int echoPinL = 47;
const int trigPinR = 44;                // Right
const int echoPinR = 45;

// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

ezButton button(BUTTON_PIN);                                    // Create ezButton object that attaches to pin 23;
ezButton limitSwitchL(SWITCHL_PIN);                             // Create ezButton object(limit switch) that attaches to pin 9;
ezButton limitSwitchR(SWITCHR_PIN);                             // Create ezButton object(limit switch) that attaches to pin 9;
DIYables_IRcontroller_21 irController(IR_RECEIVER_PIN, 200);    // Debounce time is 200ms
byte pin_rows[ROW_NUM] = {A0, A1, A2, A3};                      // Connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {A4, A5, A6};                     // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Variables will change:
int ledState = LOW;                            // The current state of LED
// int speed = 0;

// Add global variable to store the current line following mode
// int lineFollowMode = 1;                     // 0: Left, 1: Straight, 2: Right

// Variable to store the previous switch state
// int maximumRange = 100;                     // Maximum range needed 1115 (NEAR 100%)
// int minimumRange = 4;                       // Minimum range needed 4 (NEAR 0%)
long duration;
int distance;

// Function prototype
// long data(int trigPin, int echoPin);
// void LineFollower();
// void IR_Controller();
// void Keypad();

// enum RobotState {
//   LineFollower,
//   Keypad,
//   IR_Controller,
// };

// RobotState robotState = LineFollower;

// The setup routine run once
void setup() {
  Serial.begin(9600);                       // initialize serial
  irController.begin();
  pinMode(RED_PIN, OUTPUT);                 // set red LED pin to output mode
  pinMode(YELLOW_PIN, OUTPUT);              // set green LED pin to output mode
  pinMode(GREEN_PIN, OUTPUT);               // set green LED pin to output mode

  pinMode(LINE_D1, INPUT);
  pinMode(LINE_D2, INPUT);
  pinMode(LINE_D3, INPUT);
  pinMode(LINE_D4, INPUT);
  pinMode(LINE_D5, INPUT);

  pinMode(trigPinL, OUTPUT);                // Sets the trigPin as an Output
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinL, INPUT);                 // Sets the echoPin as an Input
  pinMode(echoPinR, INPUT);

  digitalWrite(RED_PIN, HIGH);              // Turn on red LED initially
  digitalWrite(YELLOW_PIN, LOW);            // Turn off yellow LED initially
  digitalWrite(GREEN_PIN, LOW);             // Turn off green LED initially
  
  button.setDebounceTime(50);               // Set debounce time to 50 milliseconds
  limitSwitchL.setDebounceTime(50);         // Set debounce time to 50 milliseconds
  limitSwitchR.setDebounceTime(50);         // Set debounce time to 50 milliseconds
  noTone(8);

  digitalWrite(PWM_DIR, LOW);
  // Set motor speeds to zero to prevent rotation
  motorL.setSpeed(0);
  motorR.setSpeed(0);

  lcd.init();                               // Initialize the 16x2 lcd module
  lcd.backlight();                          // Enable backlight for the LCD module
  lcd.clear();
  lcd.begin(20, 4);                         // Initialize the LCD with 20 columns and 4 rows

  // Display startup information
  displayStartupInfo();
}

// The loop routine runs over and over again forever
void loop() {
  button.loop();

  if (button.isPressed()) {
    toggleModes();
  }

    if (ledState == HIGH) {
      LineFollower();               // Call the LineFollower function only when the switch is ON
    } 
    
    else {
      int mode = getMode();
      switch (mode) {
        case 1:
          Keypad();
          break;

        case 2:
          IR_Controller();
          break;

        default:
          // Handle other modes if needed
          break;
      }
    }
}


///////////////
///long data///
///////////////
long data(int trigPin, int echoPin) {       // Function to measure distance using ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  // digitalWrite(trigPin, LOW);
  // delayMicroseconds(60);                    // Delay for the maximum range
  long duration = pulseIn(echoPin, HIGH);
  return duration / 58;
}


/////////////////////
///StartUp Display///
/////////////////////
void displayStartupInfo() {
  Serial.println("Delivery Robot");
  Serial.println("Press 1 time for Line Follow Mode");
  Serial.println("Press 2 times for Controller Mode");

  lcd.setCursor(0, 0);
  lcd.print("Delivery Robot");
  lcd.setCursor(0, 1);
  lcd.print("Switch once for line");
  lcd.setCursor(0, 2);
  lcd.print("follow, twice for");
  lcd.setCursor(0, 3);
  lcd.print("control mode");
}


/////////////////
///Push Button///
/////////////////
void toggleModes() {
    // toggle state of LEDs
    ledState = !ledState;

    // control LEDs according to the toggled state
    if (ledState == HIGH) {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      Serial.println("Line Follow Mode");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Line Follow Mode");
    }
    else {
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      Serial.println("Controller Mode");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Controller Mode");
    }
  }

int getMode() {
  // Implement logic to determine the current mode
  // For now, return a default mode
  return 1;
}

///////////////////
///IR Controller///
///////////////////
void IR_Controller() {
  Key21 command = irController.getKey();
  if (command != Key21::NONE) {
    switch (command) {
      case Key21::KEY_CH_MINUS:
        Serial.println("CH-");
        LineFollower();
        break;

      case Key21::KEY_CH:
        Serial.println("CH");
        Keypad();
        break;

      case Key21::KEY_CH_PLUS:
        Serial.println("CH+");
        IR_Controller();
        break;

      case Key21::KEY_PREV:
        Serial.println("<<");
        followRoute(1);                       // Follow the route 1
        delay(10);
        break;

      case Key21::KEY_NEXT:
        Serial.println(">>");
        followRoute(2);                       // Follow the route 2
        delay(10);
        break;

      case Key21::KEY_PLAY_PAUSE:
        Serial.println(">||");
        followRoute(3);                       // Follow the route 3
        delay(10);
        break;

      case Key21::KEY_VOL_MINUS:
        Serial.println("–");
        followRoute(4);                       // Follow the route 4
        delay(10);
        break;

      case Key21::KEY_VOL_PLUS:
        Serial.println("+");
        followRoute(5);                       // Follow the route 5
        delay(10);
        break;

      case Key21::KEY_EQ:
        Serial.println("EQ");
        followRoute(6);                       // Follow the route 6
        delay(10);
        break;

      case Key21::KEY_0:
        Serial.println("0");
        followRoute(7);                       // Follow the route 7
        delay(10);
        break;

      case Key21::KEY_100_PLUS:
        Serial.println("100+");
        followRoute(8);                       // Follow the route 8
        delay(10);
        break;

      case Key21::KEY_200_PLUS:
        Serial.println("200+");
        followRoute(9);                       // Follow the route 9
        delay(10);
        break;

      case Key21::KEY_1:
        Serial.println("1");
        gradualLeft();
        delay(10);
        break;

      case Key21::KEY_2:
        Serial.println("2");
        moveForward();
        delay(10);
        break;

      case Key21::KEY_3:
        Serial.println("3");
        gradualRight();
        delay(10);
        break;

      case Key21::KEY_4:
        Serial.println("4");
        turnLeft();
        delay(10);
        break;

      case Key21::KEY_5:
        Serial.println("5");
        Stop();
        delay(10);
        break;

      case Key21::KEY_6:
        Serial.println("6");
        turnRight();
        delay(10);
        break;

      case Key21::KEY_7:
        Serial.println("7");
        gradualReverseLeft();
        delay(10);
        break;

      case Key21::KEY_8:
        Serial.println("8");
        moveBackward();
        delay(10);
        break;

      case Key21::KEY_9:
        Serial.println("9");
        gradualReverseRight();
        delay(10);
        break;

      default:
        Serial.println("WARNING: undefined command:");
        lcd.setCursor(0, 3);
        lcd.print("WARNING: undefined command:");
        break;
    }
  }
}


////////////
///Keypad///
////////////
void Keypad() {
  char command = keypad.getKey();
  if (command != NO_KEY) {
    switch (command) {
      case '1':
        Serial.println("1");
        followRoute(1);                       // Follow the route 1
        delay(10);
        break;
        
      case '2':
        Serial.println("2");
        followRoute(2);                       // Follow the route 2
        delay(10);
        break;

      case '3':
        Serial.println("3");
        followRoute(3);                       // Follow the route 3
        delay(10);
        break;

      case '4':
        Serial.println("4");
        followRoute(4);                       // Follow the route 4
        delay(10);
        break;

      case '5':
        Serial.println("5");
        followRoute(5);                       // Follow the route 5
        delay(10);
        break;

      case '6':
        Serial.println("6");
        followRoute(6);                       // Follow the route 6
        delay(10);
        break;

      case '7':
        Serial.println("7");
        followRoute(7);                       // Follow the route 7
        delay(10);
        break;
        
      case '8':
        Serial.println("8");
        followRoute(8);                       // Follow the route 8
        delay(10);
        break;

      case '9':
        Serial.println("9");
        followRoute(9);                       // Follow the route 9
        delay(10);
        break;

      case '*':
        Serial.println("*");
        LineFollower();
        break;

      case '0':
        Serial.println("0");
        Stop();                                // Robot Stop
        break;

      case '#':
        Serial.println("#");
        LineFollower();
        break;

      default:
        Serial.println("WARNING: undefined command:");
        lcd.setCursor(0, 3);
        lcd.print("WARNING: undefined command:");
        break;
    }
  }
}


/////////////////////////////////////
///LineFollower with ObstacleAvoid///
/////////////////////////////////////
void LineFollower() {
  limitSwitchL.loop(); // MUST call the loop() function first
  limitSwitchR.loop(); // MUST call the loop() function first
  int stateL = limitSwitchL.getState();
  int stateR = limitSwitchR.getState();

  int distanceL = data(trigPinL, echoPinL);           // Measure distance for the Left ultrasonic sensor
  int distanceR = data(trigPinR, echoPinR);           // Measure distance for the Right ultrasonic sensor

  // Print the distances to the serial monitor
  Serial.print("Distance Left: ");
  Serial.print(distanceL);
  Serial.print(" cm, Distance Right: ");
  Serial.print(distanceR);
  Serial.println(" cm");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("L:");
  lcd.print(distanceL);
  lcd.print(" R:");
  lcd.print(distanceR);
  lcd.print(" cm");

  lcd.setCursor(0, 2);
  if (distanceL > 0 && distanceL <= 20) {
    Serial.println("Start Avoid - Obstacle detected on the left side");
    lcd.println("Avoid Left, Turn Right");
    delay(300);
    avoidObstacleLeft();
  }

  if (distanceR > 0 && distanceR <= 20) {
    Serial.println("Start Avoid - Obstacle detected on the right side");
    lcd.println("Avoid Right, Turn Left");
    delay(300);
    avoidObstacleRight();
  }

  if (stateL == HIGH && stateR == LOW) {
    Serial.println("The limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the left side");
    lcd.println("Avoid Left, Turn Right");
    delay(300);
    avoidObstacleLeft();
  }

  if (stateL == LOW && stateR == HIGH) {
    Serial.println("The limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the right side");
    lcd.println("Avoid Right, Turn Left");
    delay(300);
    avoidObstacleRight();
  }

  if (stateL == HIGH && stateR == HIGH) {
    Serial.println("The BOTH limit switch: TOUCHED");
    Serial.println("Start Avoid - Obstacle detected on the front");
    lcd.println("Avoid Front, move Backward");
    delay(300);
    avoidObstacleFront();
  }

  if (distanceL >= 20 && distanceR >= 20 && stateL == LOW && stateR == LOW) {
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
      moveForward();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
      hardRight();
    }
    else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      gradualRight();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
      gentleRight();
    }
    else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      slightRight();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
      hardLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
      gradualLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
      gentleLeft();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
      slightLeft();
    }
    else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
      Stop();
    }
    else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
      Serial.println("Line Loss Detect");
      lcd.setCursor(0, 3);
      lcd.print("Line Loss Detect");
      Stop();
      // delay(100);
      moveBackwardslow();                     // Robot reverse back to find the line
      delay(250);
    }
    else {
    }
    // // Perform line following based on the current line follow mode
    // switch (lineFollowMode) {
    //   case 0:  // Left
    //     // Implement logic for left line following
    //     break;

    //   case 1:  // Straight
    //     // Implement logic for straight line following
    //     break;

    //   case 2:  // Right
    //     // Implement logic for right line following
    //     break;

    //   default:
    //     Serial.println("WARNING: undefined command:");
    //     lcd.setCursor(0, 3);
    //     lcd.print("WARNING: undefined command:");
    //     break;
    // }
  }
}


/////////////////////////////
///Multiple Route Follower///
/////////////////////////////
void followRoute(int routeNumber) {
  switch (routeNumber) {
    case 1:                         // Implement movements for route 1
      moveForward();
      delay(1000);
      turnLeft();
      delay(500);
      moveForward();
      delay(1000);
      break;

    case 2:                         // Implement movements for route 2
      turnRight();
      delay(500);
      moveForward();
      delay(1000);
      turnLeft();
      delay(500);
      moveForward();
      delay(1000);
      break;

    case 3:                         // Implement movements for route 3
      turnRight();
      delay(500);
      moveForward();
      delay(1000);
      turnLeft();
      delay(500);
      moveForward();
      delay(1000);
      break;

    case 4:                         // Implement movements for route 4
      delay(1000);
      break;

    case 5:                         // Implement movements for route 5
      delay(1000);
      break;

    case 6:                         // Implement movements for route 6
      delay(1000);
      break;

    case 7:                         // Implement movements for route 7
      delay(1000);
      break;

    case 8:                         // Implement movements for route 8
      delay(1000);
      break;

    case 9:                         // Implement movements for route 9
      delay(1000);
      break;

    default:
      // Handle invalid routeNumber
      break;
  }
}


////////////////////
///Obstacle Avoid///
////////////////////
void avoidObstacleLeft() {           // Robot turn Right to avoid obstacle at left
  Stop();
  delay(100);
  moveBackwardslow();
  delay(500);
  turnRight();                       // turn right
  delay(400);
  moveForward();
  delay(1100);
  turnLeft();                        // move straight
  delay(600);
  moveForward();
  delay(1500);
  turnLeft();                        // turn left
  delay(600);
  moveForward();
  delay(1250);
}

void avoidObstacleRight() {          // Robot turn Left to avoid obstacle at right
  Stop();
  delay(100);
  moveBackwardslow();
  delay(500);
  turnLeft();                        // turn left
  delay(600);
  moveForward();
  delay(1000);
  turnRight();                       // move straight
  delay(600);
  moveForward();
  delay(1500);
  turnRight();                       // turn right
  delay(600);
  moveForward();
  delay(1250);
}

void avoidObstacleFront() {          // Robot turn Left to avoid obstacle at front
  Stop();
  delay(100);
  moveBackwardslow();
  delay(500);
  turnLeft();                        // turn left
  delay(600);
  moveForward();
  delay(1000);
  turnRight();                       // move straight
  delay(600);
  moveForward();
  delay(1500);
  turnRight();                       // turn right
  delay(600);
  moveForward();
  delay(1250);
}


///////////////
///Direction///
///////////////
void moveForward() {
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
  tone(8, 523, 300);
  motorL.setSpeed(-130);
  motorR.setSpeed(-130);             // Robot move backward
}

void moveBackwardslow() {
  Serial.println("Move Backward Slow");
  lcd.setCursor(0, 3);
  lcd.print("Move Backward");
  tone(8, 523, 300);
  motorL.setSpeed(-70);
  motorR.setSpeed(-70);              // Robot move backward
}

void slightLeft() {
  Serial.println("Slight Left");
  lcd.setCursor(0, 3);
  lcd.print("Slight Left");
  motorL.setSpeed(0);
  motorR.setSpeed(100);              // Robot move slight left
}

void gentleLeft() {
  Serial.println("Gentle Left");
  lcd.setCursor(0, 3);
  lcd.print("Gentle Left");
  motorL.setSpeed(30);
  motorR.setSpeed(100);              // Robot move gentle left
}

void gradualLeft() {
  Serial.println("Gradual Left");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Left");
  motorL.setSpeed(60);
  motorR.setSpeed(100);              // Robot move gradual left
}

void hardLeft() {
  Serial.println("Hard Left");
  lcd.setCursor(0, 3);
  lcd.print("Hard Left");
  motorL.setSpeed(80);
  motorR.setSpeed(100);              // Robot move hard left
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
  motorL.setSpeed(100);
  motorR.setSpeed(0);                // Robot move slight right
}

void gentleRight() {
  Serial.println("Gentle Right");
  lcd.setCursor(0, 3);
  lcd.print("Gentle Right");
  motorL.setSpeed(100);
  motorR.setSpeed(30);               // Robot move gentle right
}

void gradualRight() {
  Serial.println("Gradual Right");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Right");
  motorL.setSpeed(100);
  motorR.setSpeed(60);               // Robot move gradual right
}

void hardRight() {
  Serial.println("Hard Right");
  lcd.setCursor(0, 3);
  lcd.print("Hard Right");
  motorL.setSpeed(100);
  motorR.setSpeed(80);               // Robot move hard right
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
  motorL.setSpeed(60);
  motorR.setSpeed(-100);             // Robot move gradual left
}

void gradualReverseRight() {
  Serial.println("Gradual Right");
  lcd.setCursor(0, 3);
  lcd.print("Gradual Right");
  motorL.setSpeed(-100);
  motorR.setSpeed(60);               // Robot move gradual right
}

void Stop() {
  Serial.println("Stop");
  lcd.setCursor(0, 3);
  lcd.print("Stop");
  motorL.setSpeed(0);
  motorR.setSpeed(0);                // Robot completely stop
}
