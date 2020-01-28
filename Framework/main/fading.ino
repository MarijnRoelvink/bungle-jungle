long startFade;

//goal: adjust the startFade to current time
//input: none
//output: none
void startFading() {
  startFade = millis();
}

//goal: fade in or out, depending on the startFade
//input: none
//output: none
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
