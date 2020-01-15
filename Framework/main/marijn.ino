int mLastSwitch = 0;
int mWaitingTime = 0;
bool on = false;

void initMarijnIdle() {
  on = false;
  mWaitingTime = random(getVar("m_timeout_period").value / 2, getVar("m_timeout_period").value);
  mLastSwitch = millis();
}

void marijnIdle() {
  if (on) {
    showColor(125, 125, 255);
  }
  if (millis() - mLastSwitch > mWaitingTime) {
    on = !on;
    mLastSwitch = millis();
    if (on) {
      mWaitingTime = random(getVar("m_firefly_period").value / 2, getVar("m_firefly_period").value);
    } else {
      mWaitingTime = random(getVar("m_timeout_period").value / 2, getVar("m_timeout_period").value);
    }
  }
}
