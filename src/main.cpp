
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "pinout.h"

//----------------------------------------------------------------------------------

#define BAUD_RATE       115200
#define RESOLUTION      12
#define SCALE           ((1 << RESOLUTION) - 1)
#define RADIUS          (tft.height()/15)

//----------------------------------------------------------------------------------

TFT_eSPI tft;
TFT_eSprite sprite = TFT_eSprite(&tft);

int red = 0;
int green = 0;
int blue = 0;

//----------------------------------------------------------------------------------

// Function to convert HSL color to RGB color
void hslToRgb(float h, float s, float l, uint8_t &r, uint8_t &g, uint8_t &b) {
  // Convert hue from degrees to a 0-1 range
  h /= 360.0;

  // Check for saturation being 0 to handle grayscale conversion
  if (s == 0.0) {
    r = g = b = static_cast<uint8_t>(l * 255.0);
    return;
  }

  // Helper functions for HSL to RGB conversion
  auto hue2rgb = [](float p, float q, float t) {
    if (t < 0.0) t += 1.0f;
    if (t > 1.0) t -= 1.0f;
    if (t < 1.0 / 6.0) return p + (q - p) * 6.0f * t;
    if (t < 1.0 / 2.0) return q;
    if (t < 2.0 / 3.0) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
  };

  // Calculate intermediate values for HSL to RGB conversion
  float q = (l < 0.5) ? l * (1.0 + s) : l + s - (l * s);
  float p = 2.0 * l - q;

  // Convert HSL to RGB
  r = static_cast<uint8_t>(hue2rgb(p, q, h + 1.0 / 3.0) * 255.0);
  g = static_cast<uint8_t>(hue2rgb(p, q, h) * 255.0);
  b = static_cast<uint8_t>(hue2rgb(p, q, h - 1.0 / 3.0) * 255.0);
}

void drawColorMixer() {
    uint8_t r, g, b;
    uint16_t color;

    // RED circle
    hslToRgb(0.0, 1.0, 0.5f*red/SCALE, r, g, b);
    color = tft.color565(r, g, b);
    sprite.fillCircle(tft.width()/2, tft.height()*2/20, RADIUS, TFT_BLACK);    
    sprite.fillCircle(tft.width()/2, tft.height()*2/20, RADIUS, color);
    sprite.drawCircle(tft.width()/2, tft.height()*2/20, RADIUS, TFT_RED);
    
    // GREEN circle
    hslToRgb(120.0, 1.0, 0.5f*green/SCALE, r, g, b);
    color = tft.color565(r, g, b);
    sprite.fillCircle(tft.width()/2, tft.height()*7/20, RADIUS, TFT_BLACK);
    sprite.fillCircle(tft.width()/2, tft.height()*7/20, RADIUS, color);
    sprite.drawCircle(tft.width()/2, tft.height()*7/20, RADIUS, TFT_GREEN);

    // BLUE circle
    hslToRgb(240.0, 1.0, 0.5f*blue/SCALE, r, g, b);
    color = tft.color565(r, g, b);
    sprite.fillCircle(tft.width()/2, tft.height()*12/20, RADIUS, TFT_BLACK);
    sprite.fillCircle(tft.width()/2, tft.height()*12/20, RADIUS, color);
    sprite.drawCircle(tft.width()/2, tft.height()*12/20, RADIUS, TFT_BLUE);

    // The adding of RED, GREEN and BLUE circle    
    sprite.fillCircle(tft.width()/2, tft.height()*17/20, RADIUS, TFT_BLACK);
    color = tft.color565((uint8_t)(255.0 * red/SCALE), (uint8_t)(255.0 * green/SCALE), (uint8_t)(255.0 * blue/SCALE));
    sprite.fillCircle(tft.width()/2, tft.height()*17/20, RADIUS, color);

    // Actually draw the screen
    sprite.pushSprite(0, 0);
}

// Setup routine for the ESP32S3
void setup() {    
    // Initialize TFT
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);

    // Initialize sprite for representing the TFT screen
    sprite.setColorDepth(16);                                   // Set the color depth to 16-bit (RGB565)
    sprite.createSprite(tft.width(), tft.height());
    sprite.setRotation(0);
    sprite.setTextDatum(CC_DATUM);
    sprite.fillScreen(TFT_BLACK);
    sprite.drawString("+", tft.width()/2, tft.height()*4.5/20, 4);
    sprite.drawString("+", tft.width()/2, tft.height()*9.5/20, 4);
    sprite.drawString("=", tft.width()/2, tft.height()*14.5/20, 4);

    // Initialize serial port
    Serial.begin(BAUD_RATE);

    // Configure I/O pins
    pinMode(RED_OUT, OUTPUT);
    pinMode(GREEN_OUT, OUTPUT);
    pinMode(BLUE_OUT, OUTPUT);
    analogWriteResolution(RESOLUTION);

    pinMode(RED_IN, INPUT);
    pinMode(GREEN_IN, INPUT);
    pinMode(BLUE_IN, INPUT);
    analogReadResolution(RESOLUTION);
}

// Forever do this
void loop() {
    // Read the potenciometers 
    red = analogRead(RED_IN);
    green = analogRead(GREEN_IN);
    blue = analogRead(BLUE_IN);
    
    // Output the light intencity to LEDs using PWM
    analogWrite(RED_OUT, red);
    analogWrite(GREEN_OUT, green);
    analogWrite(BLUE_OUT, blue);

    // Draw the corresponding colors on the TFT screen
    drawColorMixer();
}
