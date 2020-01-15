LauraState ls = L_OFF;

void setLauraState(LauraState s) {
  ls = s;
}

void lOff() {
  if ((millis() - failsafe) > 5000 && id == 1) {
    setLauraState(L_FIREFLY);
  }
}

bool delayAndCheck(int t) {
  LauraState startstate = ls;
  unsigned long starttime = millis();
  while (millis() < starttime + t) {
    loopPressureSensor();
    loopMqtt();
    checkStepping();
    if (ls != startstate || state != INACTIVE) {
      return true;
    }
    if (getVar("setting").value != 1) {
      return true;
    }
  }
  return false;
}

void lFirefly() {
  for (int i = 0, delta = 1; i > -1; i += delta) {
    if (i == 150) delta = -1;
    for (int j = 0; j < NUMPIXELS; j++) {
      pixels.setPixelColor(j, pixels.Color(i / 3, i / 3 * 2, i / 3));
    }
    pixels.show();
    
    if (delayAndCheck(5)) {
      return;
    }
    if (delta == -1 && i == 100) {
      sendMessage(String(neighbours[random(NEIGHBOURSIZE)]), "firefly");
    }
  }
  setLauraState(L_OFF);
  sendMessage("all", "failsafe");
}

void initLauraIdle() {
  if (lastOn == id) {
    setLauraState(L_FIREFLY);
  } else if (lastOn == 0 && id == 1) {
    setLauraState(L_FIREFLY);
  } else {
    setLauraState(OFF);
  }
}

void lauraIdle() {
  switch (ls) {
    case L_OFF: lOff();
    case L_FIREFLY: lFirefly();
  }
}
