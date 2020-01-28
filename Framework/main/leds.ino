//goal: initialize the led strip
//input: none
//output: none
void initColour() {
  pixels.begin(); // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.setBrightness(150);
  pixels.clear(); // Set all pixel colors to 'off'
}

//goal: turn all the lights off
//input: none
//output: none
void clearPixels() {
  pixels.clear();
  pixels.show();
}

//goal: turn the light on
//input: r = red value, g = green value, b = blue value
//output: none
void showColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

//goal: turn the light on
//input: c = colour of the colour list
//output: none
void showColor(Colour c) {
  showColor(c.red, c.green, c.blue);
}

//goal: turn the light on for 300 ms
//input: none
//output: none
void bang(){  
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(250, 0, 250));
  }
  pixels.show();
  delay(300);
  clearPixels();
}
