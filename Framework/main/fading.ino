long startFade;

void startFading() {
  startFade = millis();
}

void fade() {
  double fadeVal = -1 * ((millis() - startFade) / getVar("fading_time").value - 1);
  if (fadeVal > 0) {
    showColor(fadeVal * currColor.red, currColor.green, currColor.blue);
  } else {
    setState(OFF);
    if (lastOn == id) {
      setState(INACTIVE);
    }

  }
}
