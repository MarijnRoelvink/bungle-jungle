
void initColour() {
  pixels.begin(); // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.setBrightness(150);
  pixels.clear(); // Set all pixel colors to 'off'
}

void clearPixels() {
  pixels.clear();
  pixels.show();
}

void showColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void showColor(Colour c) {
  showColor(c.red, c.green, c.blue);
}

void bang(){  
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(250, 0, 250));
  }
  pixels.show();
  delay(300);
  clearPixels();
}
