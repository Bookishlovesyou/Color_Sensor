#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define redpin 3
#define greenpin 5
#define bluepin 6
#define commonAnode false  // Common cathode RGB LED

byte gammatable[256];

Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

String lastColor = "";
int colorStableCount = 0;
String pendingColor = "";

// === LCD Update Function with stability check ===
void showColorOnLCD(String color) {
  // Require color to be stable for 2 readings to reduce flickering
  if (color == pendingColor) {
    colorStableCount++;
  } else {
    pendingColor = color;
    colorStableCount = 0;
  }

  if (colorStableCount >= 2 && color != lastColor) {
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

  Serial.println("=== Color Sensor Started ===");
}

void loop() {
  float red, green, blue;
  tcs.setInterrupt(false);
  delay(60);
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true);

  float brightness = red + green + blue;

  // Print raw readings
  Serial.print("R: "); Serial.print((int)red);
  Serial.print("\tG: "); Serial.print((int)green);
  Serial.print("\tB: "); Serial.print((int)blue);
  Serial.print("\tBright: "); Serial.print((int)brightness);

  // Normalize color ratios
  float rRatio = red / brightness;
  float gRatio = green / brightness;
  float bRatio = blue / brightness;

  // Calculate color differences for better discrimination
  float rgDiff = rRatio - gRatio;
  float rbDiff = rRatio - bRatio;
  float gbDiff = gRatio - bRatio;

  // Print ratios to Serial
  Serial.print("\trR: "); Serial.print(rRatio, 3);
  Serial.print("\tgR: "); Serial.print(gRatio, 3);
  Serial.print("\tbR: "); Serial.print(bRatio, 3);
  Serial.print("\tR-G: "); Serial.print(rgDiff, 3);

  // --- Main Color Detection Logic ---
  String colorName = "Unknown";

  // BLUE: Blue dominant, low red - CHECK FIRST!
  // Key: Blue should be clearly highest
  if (bRatio > 0.38 && rRatio < 0.32 && bRatio > rRatio && bRatio > gRatio) {
    colorName = "Blue";
  }
  // GREEN: Green dominant
  else if (gRatio > 0.48 && rRatio < 0.35 && bRatio < 0.35) {
    colorName = "Green";
  }
  // YELLOW: Red and Green similar and high, blue clearly lower
  // Key: R and G are close together, both high
  else if (rRatio > 0.35 && gRatio > 0.35 && bRatio < 0.26 &&
           abs(rgDiff) < 0.12) {
    colorName = "Yellow";
  }
  // ORANGE: Red higher than green (but not too much), blue low
  // Key: R > G with small-to-moderate difference
  else if (rRatio > 0.36 && gRatio > 0.24 && bRatio < 0.26 &&
           rgDiff > 0.04 && rgDiff < 0.16) {
    colorName = "Orange";
  }
  // RED: Strong red dominance, large R-G difference
  else if (rRatio > 0.42 && rgDiff > 0.17) {
    colorName = "Red";
  }

  Serial.print("\t→ "); Serial.println(colorName);

  // --- LED Color Output ---
  analogWrite(redpin, gammatable[(int)red]);
  analogWrite(greenpin, gammatable[(int)green]);
  analogWrite(bluepin, gammatable[(int)blue]);

  // --- LCD Update (with stability filter) ---
  showColorOnLCD(colorName);

  delay(300);
}