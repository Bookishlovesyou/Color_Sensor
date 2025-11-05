#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define redpin 3
#define greenpin 5
#define bluepin 6
#define commonAnode false  // Common cathode RGB LED

byte gammatable[256];

// === Helper function ===



// Initialize TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

String lastColor = "";

void showColorOnLCD(String color) {
  if (color != lastColor) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Detected Color:");
    lcd.setCursor(4, 1);
    lcd.print(color);
    lastColor = color;
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.backlight();

  if (!tcs.begin()) {
    Serial.println("No TCS34725 found... check connections!");
    while (1);
  }

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  for (int i = 0; i < 256; i++) {
    float x = i / 255.0;
    x = pow(x, 2.5);
    x *= 255;
    gammatable[i] = commonAnode ? (byte)(255 - x) : (byte)x;
  }
}

void loop() {
  float red, green, blue;
  tcs.setInterrupt(false);
  delay(60);
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true);

  Serial.print("R: "); Serial.print(int(red));
  Serial.print("\tG: "); Serial.print(int(green));
  Serial.print("\tB: "); Serial.println(int(blue));

  float brightness = red + green + blue;

  // Detect no color or very dark (black)
  if (brightness < 40) {
    showColorOnLCD("None");
    analogWrite(redpin, 255);
    analogWrite(greenpin, 255);
    analogWrite(bluepin, 255);  // White light
    delay(300);
    return;
  }

  // Normalize colors
  float rRatio = red / brightness;
  float gRatio = green / brightness;
  float bRatio = blue / brightness;

  // --- Color detection logic (calibrated for TCS34725) ---
  String colorName = "Unknown";

  if (rRatio > 0.45 && gRatio < 0.35 && bRatio < 0.25)
  colorName = "Red";
else if (rRatio > 0.38 && gRatio > 0.38 && bRatio < 0.25)
  colorName = "Yellow";
else if (rRatio > 0.45 && bRatio > 0.35 && gRatio < 0.3)
  colorName = "Magenta";
else if (gRatio > 0.55 && rRatio < 0.35 && bRatio < 0.3)
  colorName = "Green";
else if (bRatio > 0.55 && rRatio < 0.3 && gRatio < 0.4)
  colorName = "Blue";  
else if (bRatio > 0.45 && gRatio > 0.35 && rRatio < 0.35)
  colorName = "Cyan";
else if (rRatio > 0.35 && gRatio > 0.35 && bRatio > 0.35)
  colorName = "White";
else if (brightness < 80)
  colorName = "Black";
else if (rRatio > 0.5 && gRatio > 0.3 && bRatio < 0.2)
  colorName = "Orange";

  // --- LED output ---
  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);

  // --- Update LCD only if color changes ---
  showColorOnLCD(colorName);

  delay(300);
}

