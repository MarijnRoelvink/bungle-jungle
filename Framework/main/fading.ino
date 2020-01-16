long startFade;

void startFading() {
  startFade = millis();
}

void fade() {
  double fadeVal = -1 * ((millis() - startFade) / double(getVar("fading_time").value) - 1);

  if (fadeVal > 0) {
    showColor(fadeVal * currColor.red, fadeVal * currColor.green, fadeVal * currColor.blue);
  } else {
    setState(OFF);
    if (lastOn == id) {
      setState(INACTIVE);
    }

  }
}
