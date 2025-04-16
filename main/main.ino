/*
  Task 1 (explained below) 
  Task 2 The button task, to create a 'pause' and start to the game
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen Dimensions (pixels)
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

// I2C address found from the I2C scanner
#define OLED_ADDR 0x3C 
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // -1 for no reset pin

// define the pins:
#define potentiometer A0
#define life_1 12
#define life_2 8
#define life_3 4
#define button 2

void setup() {
  Serial.begin(9600);
  
  pinMode(potentiometer, INPUT);
  pinMode(life_1, OUTPUT);
  pinMode(life_2, OUTPUT);
  pinMode(life_3, OUTPUT);
  pinMode(button, INPUT);

  if(display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR) == 0) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }

  delay(1000);   
}

int px = 7, py = 3;
int bx=SCREEN_WIDTH/2, by=SCREEN_HEIGHT/2;
float dx=5, dy=5;
const int r = 7;
const int paddleSize = 16;

void drawBoundaries();
void drawBall(int, int);

void loop() {
  int reading = analogRead(potentiometer);
  py = map(reading, 0, 1023, 3, 45);

  // Increase ball's velocity
  bx += dx;
  by += dy;

  // Border Dectection
  if (bx + r >= SCREEN_WIDTH) {
    dx = -dx;
    bx = SCREEN_WIDTH - r;
  }
  if (by + r >= SCREEN_HEIGHT) {
    dy = -dy;
    by = SCREEN_HEIGHT - r;
  }
  if (by - r <= 0) {
    dy = -dy;
    by = r;
  }

  // Task 1
  if (bx - r <= 0) {
    // Code the Lights to make it go down when it passes this point;
    // Rest start the ball's position 
    // Make sure the ball's velocity is positive (headed towards the left)
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
  
  display.clearDisplay();
  drawBoundaries();
  drawBall(bx, by);

  // Draws Paddle
  display.drawFastVLine(px, py, paddleSize, WHITE);
  display.display(); 

  delay(75);
}

void drawBall(int centerX, int centerY) {
  display.drawFastHLine(centerX-2, centerY-3, 3, WHITE);
  display.drawFastHLine(centerX-3, centerY-2, 5, WHITE);
  display.drawFastHLine(centerX-4, centerY-1, 7, WHITE);
  display.drawFastHLine(centerX-4, centerY, 7, WHITE);
  display.drawFastHLine(centerX-4, centerY+1, 7, WHITE);
  display.drawFastHLine(centerX-3, centerY+2, 5, WHITE);
  display.drawFastHLine(centerX-2, centerY+3, 3, WHITE); 
}

void drawBoundaries() {
  display.drawFastVLine(0, 0, 64, WHITE);
  display.drawFastHLine(0, 0, 128, WHITE);
  display.drawFastVLine(127, 0, 64, WHITE);
  display.drawFastHLine(0, 63, 128, WHITE);
}
