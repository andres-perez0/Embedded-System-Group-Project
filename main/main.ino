// Additional Resources https://forum.arduino.cc/t/arduino-i2c-with-ssd1306-oled-random-pixels/870026 

// Task 
// - Design the Proper Circuit (aka sketch out a diagram)
// - Think about the logic 
// - Divide the Task between us
// - think about optimizations to ensure a proper fps (using the libary or trying to get real into depth with the memory to increase fps)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen Dimensions (pixels)
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
// I2C address found from the I2C scanner
#define OLED_ADDR   0x3C 

#define OLED_RESET  -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // -1 for no reset pin

void setup() {
  Serial.begin(9600);

  if(display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR == 0)) {
    // Checks if the address is approiate
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }

  delay(1000); 
}

const int x = 2;
const int paddleSize = 16;
int y = 2;
char c; 

void loop() {
  display.clearDisplay();
  display.drawFastVLine(x, y, paddleSize, WHITE);
  display.display();

  if(Serial.available()) {
    c = Serial.read();
    if (c == 's') {
      y += 2; 
      display.clearDisplay();
      display.drawFastVLine(x, y, 8, WHITE);
      display.display();
    } else if (c == 'w') {
      y -= 2; 
      display.clearDisplay();
      display.drawFastVLine(x, y, 8, WHITE);
      display.display();
    }
  }
  delay(100);
}
