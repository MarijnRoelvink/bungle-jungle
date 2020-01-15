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
    double fadeTime = mWaitingTime * getVar("m_fade_percentage").value / (100 * 2.0);
    double fadeValue = 1;
    if (millis() - mLastSwitch < fadeTime) {
      //fade in
      fadeValue = (millis() - mLastSwitch) / fadeTime;
    } 
    else if (millis() - mLastSwitch > mWaitingTime - fadeTime) {
      //fade out
      fadeValue = -1 * ((millis() - (mLastSwitch + mWaitingTime - fadeTime)) / fadeTime - 1);
    }
    //show colors
    showColor(fadeValue*125, fadeValue*125, fadeValue*255);
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
