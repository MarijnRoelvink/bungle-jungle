#include <WiFiNINA.h>
#include <MQTT.h>

const char ssid[] = "iot-net";
const char pass[] = "interactive";
const char mqtt_username[] = "35e5494d";
const char mqtt_password[] = "52d131e1f30b531c";
char network_address[] = "broker.shiftr.io";
WiFiClient net;
MQTTClient client;

//goal: connect to wifi and broker
//input: none
//output: none
void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("+");
    delay(1000);
  }

  char cId[3];
  itoa(id, cId, 10);
  Serial.print("\nconnecting...");
  while (!client.connect(cId, mqtt_username, mqtt_password)) {
    Serial.print("x");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/all");
  client.subscribe("/" + String(id));
  client.subscribe("/check");
}

//goal: reset the mqtt
//input: none
//output: none
void resetMqtt() {
  Serial.println("Disconnecting!");
  client.disconnect();
  client.begin(network_address, net);
  client.onMessage(messageReceived);
  connect();
}

//goal: get the id send from a random message
//input: msg = message to get id from
//output: the id read from the message
int getNumber(String msg) {
  int index = msg.indexOf(" ");
  if (index != -1) {
    return msg.substring(index, msg.length()).toInt();
  } else {
    return 0;
  }
}

//goal: receive messages starting with change to change the value
//input: msg = message to read the values uit
//output: none
void changemessage(String msg) {
  int first = msg.indexOf(" ");
  int second = msg.indexOf(" ", first + 1);
  String varname = msg.substring(first + 1, second);
  int value = msg.substring(second + 1, msg.length()).toInt();
  changeVar(varname, value);
  if(varname == "idle_setting" && state == INACTIVE) {
    setState(INACTIVE); //reinitialize idle setting
  }
}

//goal: receive message and send them to the correct functions
//input: topic = receiver of the message. payload = contents of message
//output: none
void messageReceived(String &topic, String &payload) {
  String msg = payload;
  if (msg.startsWith("change setting")) {
    int settingIndex = msg.indexOf(" ", msg.indexOf(" ") + 1);
    if (settingIndex != -1) {
      int setting = msg.substring(settingIndex, msg.length()).toInt();
      changeVar("setting", setting);
      sendMessage("all", "up " + String(id));
      switch (setting) {
        case 1: ; break;
        case 2: settingup(); break;
      }
      return;
    }
  }
  if (msg.startsWith("change network")) {
    int settingIndex = msg.indexOf(" ", msg.indexOf(" ") + 1);
    if (settingIndex != -1) {
      String newNet = msg.substring(settingIndex+1, msg.length());
      newNet.toCharArray(network_address, newNet.length()+1);
      Serial.println(network_address);
      resetMqtt();
    }
  }
  if (msg.startsWith("change")) {
    changemessage(msg);
  }
  if (getVar("setting").value == 1) {
    if (msg == "firefly" && state == INACTIVE) {
      setLauraState(L_FIREFLY);
    }
    if (msg.startsWith("on")) {
      if(state == INACTIVE) {
        setState(OFF);
      }
      lastOn = getNumber(msg);
    }
    if (msg == "failsafe") {
      failsafe = millis();
    }
    if (msg.startsWith("step")) {
      failsafe = millis();
    }
    if(msg.startsWith("idle-mode") && state == OFF) {
      setState(INACTIVE);
    }
    if(msg.startsWith("color-index")) {
      currIndex = (getNumber(msg) + 1) % colourSize; 
    }
  }
  else if (getVar("setting").value == 2) {
    gameMsg(msg);
  }
}

//goal: send message
//input: target = receivers of the message. msg = message to be send
//output: none
void sendMessage(String target, String msg) {
  client.publish("/" + target, msg);
}

//goal: initialization of the mqtt
//input: none
//output: none
void initMqtt() {
  Serial.println("WiFi.begin");
  WiFi.begin(ssid, pass);
  client.begin(network_address, net);
  client.onMessage(messageReceived);
  connect();
}

//goal: loop that checks the mqtt
//input: none
//output: none
void loopMqtt() {
  client.loop();
  if (!client.connected()) {
    connect();
  }
}
