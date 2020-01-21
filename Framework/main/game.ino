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

//what happens when a state changes
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

//check if someone is stepping on a step
bool checkGameStepping() {
  if (getRunningAvg() > getVar("threshold").value) {
    setGameState(GAMESTEPPED);
    return true;
  }
  return false;
}

//read the colours from a message
void colourmessage(String msg) {
  int space = msg.indexOf(" ");
  int first = msg.indexOf(",", space + 1);
  int second = msg.indexOf(",", first + 1);
  red = (msg.substring(space + 1, first)).toInt();
  green = (msg.substring(first + 1, second)).toInt();
  blue = (msg.substring(second + 1, msg.length())).toInt();
}

//compare the made steps to the send start and end. Correct = true, false = false.
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

//if correct send correct to those steps. same for false.
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

//what to do when a message arrives
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

//make pairs from all steps
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

//pop from array function
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

//check if state changes, setting changes or someone stepping on step
//delay also wait_in_millis
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

//check if the setting changes and do wait_in_millis
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

//check if someone is stepping on a step
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
  //if ((millis() - failsafe) > 30000) { do we want this?
  //  settingup();
  //}
}

//enough correct pairs will celebrate. then reset.
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

//correct has been send, stay on. 
void gamecorrect() {
  for (int j = 0; j < NUMPIXELS; j++) {
    pixels.setPixelColor(j, pixels.Color(red, green, blue));
  }
  pixels.show();
  //if ((millis() - failsafe) > 30000) { do we want this?
  //  settingup();
  //}
}

//go off
void gameoff() {
  clearPixels();
  checkGameStepping();
  //if ((millis() - failsafe) > 30000) { do we want this?
  //  settingup();
  //}
}

//set up function!
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

//main "loop" (loops because loop in main)
void gamemain() {
  switch (gameState) {
    case GAMESTEPPED: gamestepped(); break;
    case GAMECORRECT: gamecorrect(); break;
    case GAMEOFF: gameoff(); break;
  }
}
