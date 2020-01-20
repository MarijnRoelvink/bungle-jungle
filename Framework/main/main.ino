#include <Adafruit_NeoPixel.h>
#define PIN        5 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 10 // Popular NeoPixel ring size

struct Var {
  String varName;
  int value;
};

struct Edge {
  int a, b;
};

struct Pair {
  int one, two;
};

struct Colour {
  int red, green, blue;
};


enum State {
  INACTIVE,
  STEPPING,
  STEPPED,
  FADING,
  OFF
};

enum GameState {
  GAMEFIREFLY,
  GAMESTEPPED,
  GAMECORRECT,
  GAMEOFF
};

enum LauraState {
  L_FIREFLY,
  L_OFF
};

int getNeighboursSize(int id); //prototype

Var vars[] = {
  //global vars
  {"threshold", 4},
  {"waittime", 5 * 1000},
  {"idle_setting", 2},
  {"setting", 1 },
  {"fading_time", 1000},
  
  //game vars
  {"goalscore", 10},
  {"remembertime", 3 * 1000},
  {"noofgamesteps", 20},
  
  //marijn idle vars
  {"m_firefly_period", 3000},
  {"m_timeout_period", 10000},
  {"m_fade_percentage", 90}

};

int colourSize = 7;
Colour colours[] = {
  {100, 0, 0}, //red
  {100, 50, 0}, //orange
  {100, 100, 0}, //yellow
  {0, 100, 0}, //green
  {0, 0, 100}, //blue
  {100, 0, 100}, //purple
  {100, 0, 50}, //pink
};

int currIndex = 0;
int id = 14;                         //change per step
int lastOn = 0;
const int NEIGHBOURSIZE = getNeighboursSize(id);
int* neighbours = new int[8];
unsigned long touched;
unsigned long failsafe;
unsigned long lastsend;
unsigned long lastsendstepped;
State state = INACTIVE;
Colour currColor = {0, 0, 0};
String stepstring = "step " + String(id);
String onstring = "on " + String(id);
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void changeVar(String variableName, int value) {
  for (int i = 0; i < (sizeof(vars) / sizeof(vars[0])); i++) {
    if (variableName == vars[i].varName) {
      vars[i].value = value;
    }
  }
}

Var getVar(String variableName) {
  for (int i = 0; i < (sizeof(vars) / sizeof(vars[0])); i++) {
    if (variableName == vars[i].varName) {
      return vars[i];
    }
  }
  sendMessage("all", "Invalide varname accessed");
  return {"", 0};

}


bool checkStepping() {
  if (getRunningAvg() > getVar("threshold").value) {
    touched = millis();
    setState(STEPPING);
    return true;
  }
  return false;
}

void inactive() {
  switch (getVar("idle_setting").value) {
    case 1: lauraIdle(); break;
    case 2: marijnIdle(); break;
  }
}

void stepping() {
  showColor(currColor);
  float pressureValue = getRunningAvg();
  if (millis() - lastsend > 10000) {
    sendMessage("all", stepstring);
    lastsend = millis();
  }
  if (pressureValue > getVar("threshold").value) {
    touched = millis();
  } else {
    setState(STEPPED);
  }
}

void stepped() {
  showColor(currColor);
  if (millis() - touched > getVar("waittime").value) {
    setState(FADING);
  }
  if (millis() - lastsendstepped > getVar("waittime").value / 3) {
    sendMessage("all", stepstring);
    lastsendstepped = millis();
  }
}

void fading() {
  fade();
}

void off() {
  
}

void setState(State newState) {
  switch (newState) {
    case INACTIVE:
      sendMessage("all", "idle-mode");
      switch(getVar("idle_setting").value) {
        case 1: initLauraIdle(); break;
        case 2: initMarijnIdle(); break;
      }
      break;
    case STEPPING:
      {
        sendMessage("all", onstring);
        sendMessage("all", "color-index " + String(currIndex));
        currColor = colours[currIndex];
        break;
      }
    case STEPPED:
      {
        sendMessage("all", "up " + String(id));
        break;
      }
    case FADING: startFading(); break;
    case OFF: break;
  }
  state = newState;
}

void setup()
{
  Serial.begin(9600);
  neighbours = getNeighbours(neighbours, id);
  initPressureSensor();
  initMqtt();
  initColour();
  bang();
  lastsendstepped = 0; //check if this goes wrong
  lastsend = 0;
  touched = 0;
  setState(INACTIVE);
  if (getVar("setting").value == 2) {
    settingup();
  }
  if(id == 20) {
    sendToAll();
  }
}

void loop()
{
  loopPressureSensor();
  loopMqtt();
  switch (getVar("setting").value) {
    case 1: {
        pixels.clear();
        if (state != STEPPING) {
          checkStepping();
        }
        switch (state) {
          case INACTIVE: inactive(); break;
          case STEPPING: stepping(); break;
          case STEPPED: stepped(); break;
          case FADING: fading(); break;
          case OFF: off(); break;
        }
        pixels.show();
      } break;
    case 2: gamemain(); break;
  }
}
