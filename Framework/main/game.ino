Pair pairs[10];
GameState gameState = GAMEOFF;
String startstring = "start " + String(id);
String endstring = "end " + String(id);
int score;
int red = -1;
int green = -1;
int blue = -1;
bool firstsend;
int savestart;
int saveend;
int nosteps;

//all colours in the game
Colour gamecolours[] = {
  {250, 0, 0},
  {0, 250, 0},
  {0, 0, 250},
  {250, 0, 250},
  {250, 250, 0},
  {0, 250, 250},
  {250, 125, 250},
  {0, 250, 125},
  {125, 0, 250},
  {250, 125, 0},
};

//goal: switch the idle modus
//input: newState = new state to change to
//output: none
void setGameState(GameState newState) {
  String onstring = "on " + String(id);
  switch (newState) {
    case GAMESTEPPED: {
        if (gameState != GAMECORRECT) {
          if (!firstsend) {
            sendMessage("check", startstring);
          }
          else {
            sendMessage("check", endstring);
          }
        }
        sendMessage("all", onstring);
        break;
      }
    case GAMECORRECT: break;
    case GAMEOFF: {
        if (gameSettingChangeCheckWithDelay(1000)) {
          return;
        }
        break;
      }
  }
  gameState = newState;
}

//goal: check if someone is stepping on a step
//input: none
//output: none
bool checkGameStepping() {
  if (getRunningAvg() > getVar("threshold").value) {
    setGameState(GAMESTEPPED);
    return true;
  }
  return false;
}

//goal: read the colours from a message
//input: msg= message to read
//output: none
void colourmessage(String msg) {
  int space = msg.indexOf(" ");
  int first = msg.indexOf(",", space + 1);
  int second = msg.indexOf(",", first + 1);
  red = (msg.substring(space + 1, first)).toInt();
  green = (msg.substring(first + 1, second)).toInt();
  blue = (msg.substring(second + 1, msg.length())).toInt();
}


//goal: compare the start and end step to the made pairs 
//input: none
//output: Boolean if pair is correct then true else false.
bool checkifright() {
  for (int i = 0; i < nosteps / 2; i++) {
    if (pairs[i].one == savestart) {
      if (pairs[i].two == saveend) {
        return true;
      }
      return false;
    }
    else if (pairs[i].two == savestart) {
      if (pairs[i].one == saveend) {
        return true;
      }
      return false;
    }
  }
  return false;
}

//goal: send the message to the steps that were stepped on.
//input: none
//output: none
void sendifright() {
  if (checkifright()) {
    sendMessage(String(savestart), "correct");
    sendMessage(String(saveend), "correct");
    sendMessage("all", "addscore");
  }
  else {
    sendMessage(String(savestart), "wrong");
    sendMessage(String(saveend), "wrong");
  }
}

//goal: process the received messages
//input: msg = the received message
//output: none
void gameMsg(String msg) {
  if (msg.startsWith("correct")) {
    setGameState(GAMECORRECT);
  }
  if (msg.startsWith("addscore")) {
    score++;
    if (score >= getVar("goalscore").value) {
      party();
    }
  }
  if (msg.startsWith("wrong") && gameState != GAMECORRECT) {
    setGameState(GAMEOFF);
  }
  if (msg.startsWith("change")) {
    changemessage(msg);
  }
  if(msg.startsWith("reset")){
    settingup();
  }
  if (msg.startsWith("step")) {
    failsafe = millis();
  }
  if (msg.startsWith("colour")) {
    colourmessage(msg);
  }
  if (msg.startsWith("start")) {
    firstsend = true;
    if (id == 1) {
      int space = msg.indexOf(" ");
      savestart = (msg.substring(space + 1, msg.length())).toInt();
    }
  }
  if (msg.startsWith("end")) {
    firstsend = false;
    if (id == 1) {
      int space = msg.indexOf(" ");
      saveend = (msg.substring(space + 1, msg.length())).toInt();
      sendifright();
    }
  }
}

//goal: make pairs from all steps
//input: none
//output: none
void makepairs() {
  int pairsSize = 0;
  int leftoverSize = nosteps;
  int* leftovers = new int[nosteps];
  for (int i = 0; i < leftoverSize; i++) {
    leftovers[i] = i;
  }

  for (int i = 0; i < nosteps / 2; i++) {
    int indexA = random(leftoverSize);
    int a = leftovers[indexA];
    leftovers = pop(leftovers, leftoverSize, indexA);
    leftoverSize--;

    int indexB = random(leftoverSize);
    int b = leftovers[indexB];
    leftovers = pop(leftovers, leftoverSize, indexB);
    leftoverSize--;

    pairs[pairsSize] = {a + 1, b + 1};
    pairsSize++;
  }
  for (int i = 0; i < nosteps / 2; i++) {
    String sends = "colour " + String(gamecolours[i].red) + "," + String(gamecolours[i].green) + "," + String(gamecolours[i].blue);
    if (pairs[i].one == 1 || pairs[i].two == 1) {
      red = gamecolours[i].red;
      green = gamecolours[i].green;
      blue = gamecolours[i].blue;
    }
    sendMessage(String(pairs[i].one), sends);
    sendMessage(String(pairs[i].two), sends);
  }
  delete leftovers;
}


//goal: pop from an array
//input: arr = the array to pop from. arrSize = size of the array. index = current index.
//output: the array minus the first element
int* pop(int* arr, int arrSize, int index) {
  int currIndex = 0;
  for (int i = 0; i < arrSize; i++) {
    if (i != index) {
      arr[currIndex] = arr[i];
      currIndex++;
    }
  }
  return arr;
}

//goal: check if state changes, setting changes or someone stepping on step
//input: wait_in_millis = time it needs to wait
//output: none
bool gameStateChangeCheckWithDelay(int wait_in_millis) {
  GameState startstate = gameState;
  int cursetting = getVar("setting").value;
  unsigned long starttime = millis();
  while (millis() < starttime + wait_in_millis) {
    loopPressureSensor();
    loopMqtt();
    checkGameStepping();
    if (gameState != startstate) {
      return true;
    }
    if (getVar("setting").value != cursetting) {
      return true;
    }
  }
  return false;
}

//goal: check if the setting changes
//input: wait_in_millis = time it needs to wait
//output: none
bool gameSettingChangeCheckWithDelay(int wait_in_millis) {
  int cursetting = getVar("setting").value;
  unsigned long starttime = millis();
  while (millis() < starttime + wait_in_millis) {
    loopMqtt();
    if (getVar("setting").value != cursetting) {
      return true;
    }
  }
  return false;
}


//goal: if someone stepped on a step, stay on with colour
//input: none
//output: none
void gamestepped() {
  float pressureValue = getRunningAvg();

  for (int j = 0; j < NUMPIXELS; j++) {
    pixels.setPixelColor(j, pixels.Color(red, green, blue));
  }
  pixels.show();
  if (millis() - lastsend > 10000) {
    sendMessage("all", stepstring);
    lastsend = millis();
  }
}

//goal: if enough pairs are correct they will celebrate. then reset.
//input: none
//output: none
void party() {
  unsigned long startparty = millis();
  while (millis() - startparty < 10 * 1000) {
    if (getVar("setting").value != 2) {
      return;
    }
    int colourset = random(10);
    for (int j = 0; j < NUMPIXELS; j++) {
      pixels.setPixelColor(j, pixels.Color(gamecolours[colourset].red, gamecolours[colourset].green, gamecolours[colourset].blue));
    }
    pixels.show();
    if (gameSettingChangeCheckWithDelay(200)) {
      return;
    }
  }
  pixels.clear();
  pixels.show();
  settingup();
}
 
//goal: stay on, happens when correct has been received
//input: none
//output: none
void gamecorrect() {
  for (int j = 0; j < NUMPIXELS; j++) {
    pixels.setPixelColor(j, pixels.Color(red, green, blue));
  }
  pixels.show();
}

//goal: turn off
//input: none
//output: none
void gameoff() {
  clearPixels();
  checkGameStepping();
}

//goal: set up, make and send pairs, send and receive colours
//input: none
//output: none
void settingup() { //setup
  nosteps = getVar("noofgamesteps").value;
  score = 0;
  firstsend = false;
  failsafe = millis();
  red = -1;
  if (id == 1) {
    makepairs();
  }
  while (red == -1) {
    if (gameSettingChangeCheckWithDelay(1)) {
      return;
    }
  }
  for (int j = 0; j < NUMPIXELS; j++) {
    pixels.setPixelColor(j, pixels.Color(red, green, blue));
  }
  pixels.show();
  if (gameSettingChangeCheckWithDelay(getVar("remembertime").value)) {
    return;
  }
  else {
    setGameState(GAMEOFF);
  }

}


//goal: change to the correct function
//input: none
//output: none
void gamemain() {
  switch (gameState) {
    case GAMESTEPPED: gamestepped(); break;
    case GAMECORRECT: gamecorrect(); break;
    case GAMEOFF: gameoff(); break;
  }
}
