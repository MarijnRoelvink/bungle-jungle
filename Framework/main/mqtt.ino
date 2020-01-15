#include <WiFiNINA.h>
#include <MQTT.h>

const char ssid[] = "iot-net";
const char pass[] = "interactive";
const char mqtt_username[] = "35e5494d";
const char mqtt_password[] = "52d131e1f30b531c";
char network_address[] = "broker.shiftr.io";
WiFiClient net;
MQTTClient client;

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


void resetMqtt() {
  Serial.println("Disconnecting!");
  client.disconnect();
  client.begin(network_address, net);
  client.onMessage(messageReceived);
  connect();
}

int getNumber(String msg) {
  int index = msg.indexOf(" ");
  if (index != -1) {
    return msg.substring(index, msg.length()).toInt();
  } else {
    return 0;
  }
}

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

void messageReceived(String &topic, String &payload) {
  String msg = payload;
  if (msg.startsWith("change setting")) {
    int settingIndex = msg.indexOf(" ", msg.indexOf(" ") + 1);
    if (settingIndex != -1) {
      int setting = msg.substring(settingIndex, msg.length()).toInt();
      changeVar("setting", setting);
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

void sendMessage(String target, String msg) {
  client.publish("/" + target, msg);
}

void initMqtt() {
  Serial.println("WiFi.begin");
  WiFi.begin(ssid, pass);
  client.begin(network_address, net);
  client.onMessage(messageReceived);
  connect();
}

void loopMqtt() {
  client.loop();
  if (!client.connected()) {
    connect();
  }
}
