#include <Adafruit_NeoPixel.h>
#define PIN        5 
#define NUMPIXELS 10 

//declarations of variables, edges, pairs and colour
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

//all states of the state machines
enum State {
  INACTIVE,
  STEPPING,
  STEPPED,
  FADING,
  OFF
};

enum GameState {
  GAMESTEPPED,
  GAMECORRECT,
  GAMEOFF
};

enum LauraState {
  L_FIREFLY,
  L_OFF
};

int getNeighboursSize(int id); //prototype

//all variables changable over server
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

//all colours defined
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
int id = 9;                         //change per step
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

//goal: change current value of variableName to value of input
//input: variableName= name of variable to change. value= new value for the variable
//output: none
void changeVar(String variableName, int value) {
  for (int i = 0; i < (sizeof(vars) / sizeof(vars[0])); i++) {
    if (variableName == vars[i].varName) {
      vars[i].value = value;
    }
  }
}

//goal: get the variable requested
//input: variableName= variable to get
//output: the requested variable
Var getVar(String variableName) {
  for (int i = 0; i < (sizeof(vars) / sizeof(vars[0])); i++) {
    if (variableName == vars[i].varName) {
      return vars[i];
    }
  }
  sendMessage("all", "Invalide varname accessed");
  return {"", 0};

}

//goal: check if someone is stepping on a step
//input: none
//output: boolean true if someone is stepping, false if not.
bool checkStepping() {
  if (getRunningAvg() > getVar("threshold").value) {
    touched = millis();
    setState(STEPPING);
    return true;
  }
  return false;
}

//goal: switch the idle modus
//input: none
//output: none
void inactive() {
  switch (getVar("idle_setting").value) {
    case 1: lauraIdle(); break;
    case 2: marijnIdle(); break;
  }
}

//goal: turn the light of the step on and send to others that someone is stepping
//input: none
//output: none
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

//goal: wait for waittime seconds then start fading
//input: none
//output: none
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

//goal: start fading
//input: none
//output: none
void fading() {
  fade();
}

//goal: nothing happens because step is off
//input: none
//output: none
void off() {
  
}

//goal: changes the state and does the things that need to be done for the state change
//input: newState = new state to go to
//output: none
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

//goal: setup function, one time only
//input: none
//output: none
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


//goal: calls the correct function for every state
//input: none
//output: none
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
