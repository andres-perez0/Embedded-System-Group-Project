// Included Libaries 
#include <Wire.h>             // For I2C communication
#include <Adafruit_GFX.h>     // Following two libaries are for the OLED display
#include <Adafruit_SSD1306.h>

// Screen Dimensions (pixels)
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

// I2C address found from the I2C scanner
#define OLED_ADDR 0x3C 
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // -1 for no reset pin

// define the pins:
#define potPin A0
#define butPin 2
#define life_1 5
#define life_2 9
#define life_3 11


// Prototype Functions
void drawBoundaries();
void drawBall(int, int);
void gameOverScreen();
void resetGame();

const int r = 7;                          // Ball's Radius 
const int paddleSize = 16;                // Paddle Height
int led_brightness = 20;                  // LED Brightness (PWM)
int px = 7, py = 3;                       // Paddle's position
int bx=SCREEN_WIDTH/2, by=SCREEN_HEIGHT/2;// Ball's Position

volatile int pause = 0;                   // Pause state (1 = paused, 0 = running)
volatile float dx=5, dy=5;                // Ball's Velocity
volatile float last_dx = 5, last_dy = 5;  // Previous Velocity
int gameOn = 1;                           // 1 = game active, 0 = game over
int numLives = 3;

void setup() {
  Serial.begin(9600);
  
  pinMode(potPin, INPUT);
  pinMode(butPin, INPUT);
  pinMode(life_1, OUTPUT);
  pinMode(life_2, OUTPUT);
  pinMode(life_3, OUTPUT);
 
  // turn on the lives
  analogWrite(life_1, led_brightness);
  analogWrite(life_2, led_brightness);
  analogWrite(life_3, led_brightness);

	// Allocate RAM for image buffer, initialize peripherals and pins.
  if(display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR) == 0) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }

  attachInterrupt(digitalPinToInterrupt(butPin), butPin_clicked, RISING);

  delay(1000);   
}

void loop() {
  // Checks if the movemnt on the screen is paused(pause==1) and you ran out of lives (!gameOn)
  if (pause == 1 && !gameOn) {
    gameOverScreen();

    // Until the butPin is butPin is selected again the user is in a infinity loop;
    while(pause == 1) delay(100);

    // When the butPin is pressed; Stop the scrolling effect & reset the lives back to full;
    if (pause == 0) resetGame();
  }  

  // The Normal game loop; not paused 
  if (pause == 0){
    int reading = analogRead(potPin);
    py = map(reading, 0, 1023, 3, 45);

    // Increase ball's velocity
    bx += dx;
    by += dy;

    // Border Dectection
    if (bx + r >= SCREEN_WIDTH) {
      // Right Wall
      dx = -dx;
      bx = SCREEN_WIDTH - r;
    }
    if (by + r >= SCREEN_HEIGHT) {
      // Floor
      dy = -dy;
      by = SCREEN_HEIGHT - r;
    }
    if (by - r <= 0) {
      // Roof
      dy = -dy;
      by = r;
    }

    // If the ball goes out of bounds behind the paddle
    if (bx - r <= -20) {
      // Code the Lights to make it go down when it passes this point;
      if (numLives == 1){
        // Ends the Game after ball goes past the paddle the 3rd time
        analogWrite(life_3, 0);
        pause=1;
        gameOn=0;
      } else if (numLives == 2){
        analogWrite(life_2, 0);
        numLives -= 1;
      } else if (numLives == 3){
        analogWrite(life_1,0);
        numLives -= 1;
      }

      // Reset start the ball's position 
      bx = SCREEN_WIDTH/2;
      by = SCREEN_HEIGHT/2;

      // Make sure the ball's velocity is positive (headed towards the left)
      dx = abs(dx);
      dy = abs(dy);
    }

    // Ball Collision;
    if (bx - r <= px) {
      for (int i = 0; i < paddleSize; i++) {
        if ((py + i) == by + r) {
          dx = -dx;
        }
        if ((py + i) == by - r) {
          dx = -dx;
        }

      }
    }
  }
  
  // Clear previous frame
  display.clearDisplay();
  drawBoundaries();
  drawBall(bx, by);

  // Draws Paddle at current position
  display.drawFastVLine(px, py, paddleSize, WHITE);
  display.display();

  delay(75);
}

void drawBall(int centerX, int centerY) {
  // Draws the ball, radius 7, at a given center (centerX, centerY)
  display.drawFastHLine(centerX-2, centerY-3, 3, WHITE);
  display.drawFastHLine(centerX-3, centerY-2, 5, WHITE);
  display.drawFastHLine(centerX-4, centerY-1, 7, WHITE);
  display.drawFastHLine(centerX-4, centerY, 7, WHITE);
  display.drawFastHLine(centerX-4, centerY+1, 7, WHITE);
  display.drawFastHLine(centerX-3, centerY+2, 5, WHITE);
  display.drawFastHLine(centerX-2, centerY+3, 3, WHITE); 
}

void drawBoundaries() {
  // Draws the screen boundaries
  display.drawFastVLine(0, 0, 64, WHITE);
  display.drawFastHLine(0, 0, 128, WHITE);
  display.drawFastVLine(127, 0, 64, WHITE);
  display.drawFastHLine(0, 63, 128, WHITE);
}

void gameOverScreen() {
  // Displays the "Game Over" screen and scrolls the text
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 20);
  display.println(F("GAME OVER"));
  display.setTextSize(1);
  display.println(F(" "));
  display.println(F("Press butPin to Start"));
  display.display();
  display.startscrollleft(0x00, 0x0F);
}

void resetGame(){
  // Resets the game parameters and lights
  display.stopscroll();

  gameOn = 1;
  numLives = 3;

  // Turn on the lives
  analogWrite(life_1, led_brightness);
  analogWrite(life_2, led_brightness);
  analogWrite(life_3, led_brightness);
}

void butPin_clicked(){
  // Handles button press interrupt to pause/unpause the game
  static unsigned long last_processed_interrupt_time = 0;
  unsigned long current_interrupt_time = millis();

  if (current_interrupt_time - last_processed_interrupt_time > 250){
    if (pause == 0){
      pause = 1;
      // Saves the previous velocity
      last_dx = dx;
      last_dy = dy;
      dx = 0;
      dy = 0;
    } else{
      pause = 0;
      // Reinitalized the velocity of the ball
      dx = last_dx;
      dy = last_dy;
    }
  }
  last_processed_interrupt_time = current_interrupt_time;
}
