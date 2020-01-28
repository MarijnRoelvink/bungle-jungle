int analogPin = 0;
int raw = 0;
int Vin = 5;
float Vout = 0;
float R1 = 410;
float R2 = 0;
float buffer = 0;
const int windowSize = 10;
float values[windowSize];

//goal: initialize the pressure sensor
//input: none
//output: none
void initPressureSensor() {
  for (int i = 0; i < windowSize; i++) {
    values[i] = 0;
  }
}

//goal: update the window of the values
//input: none
//output: none
void updateWindow() {
  for (int i = 0; i < windowSize - 1; i++) {
    values[i] = values[i + 1];
  }
  values[windowSize - 1] = Vout;
}

//goal: check the value the steps gives
//input: none
//output: float of the value of the sensor
float getRunningAvg() {
  float sum = 0;
  for (int i = 0; i < windowSize; i++) {
    sum += values[i];
  }
  //Serial.println(sum/windowSize);
  return sum / windowSize;
}

//goal: check if someone is standing on the step
//input: none
//output: boolean, true if someone is standing on the step
bool checkStillStanding() {
  loopPressureSensor();
  if (getRunningAvg() < getVar("threshold").value) {
    return false;
  }
  else {
    touched = millis();
    return true;
  }
}

//goal: check the values of the sensor
//input: none
//output: none
void loopPressureSensor()
{
  raw = analogRead(analogPin);
  if (raw)
  {
    //analogread maps incoming voltage (going in a range between zero and Vin to values between 0 and 1023)
    //calculate Vin
    buffer = raw * Vin;
    Vout = (buffer) / 1024.0;
    buffer = (Vin / Vout) - 1;
    R2 = R1 / buffer;

    updateWindow();
  }
}
