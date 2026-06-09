# Color Sensor

A simple Arduino Uno project that uses a TCS34725 color sensor to detect colors. The detected color is displayed on a 16x2 I2C LCD, and an RGB LED lights up with the corresponding color.

## Components

- Arduino Uno
- TCS34725 Color Sensor
- 16x2 I2C LCD
- RGB LED (Common Cathode)
- Jumper Wires
- 220 Ω Resistors (Optional)

## Wiring

| Component | Pin | Arduino Uno |
|------------|-----|-------------|
| TCS34725 | VIN | 5V |
| TCS34725 | GND | GND |
| TCS34725 | SDA | A4 |
| TCS34725 | SCL | A5 |
| TCS34725 | 3V3 | 3.3V |
| LCD I2C | VCC | 5V |
| LCD I2C | GND | GND |
| LCD I2C | SDA | A4 |
| LCD I2C | SCL | A5 |
| RGB LED (Red) | R | D3 |
| RGB LED (Green) | G | D5 |
| RGB LED (Blue) | B | D6 |
| RGB LED (Cathode) | Common | GND |

## Features

- Detects basic colors
- Displays detected color on LCD
- RGB LED shows the detected color
- Uses I2C communication for sensor and display

## How It Works

The TCS34725 reads the color of an object placed in front of the sensor. The Arduino processes the readings, displays the detected color on the LCD, and lights the RGB LED with the corresponding color.
