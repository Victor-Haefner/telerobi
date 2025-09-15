#include "wifi.h"
#include "trine_cam.h"
#include "AsyncTCP.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>

unsigned int localPort = 2390;
WiFiUDP Udp; // TODO: put in class

String wifiStatusStr() {
  int status = WiFi.status();
  if (status == WL_CONNECTED) return "connected";
  if (status == WL_IDLE_STATUS) return "idle";
  if (status == WL_NO_SSID_AVAIL) return "bad ssid";
  if (status == WL_CONNECT_FAILED) return "failed";
  if (status == WL_SCAN_COMPLETED) return "scan completed";
  if (status == WL_CONNECTION_LOST) return "connection lost";
  if (status == WL_DISCONNECTED) return "disconnected";
  if (status == WL_NO_SHIELD) return "no shield";
  if (status == WL_STOPPED) return "stopped";
  return "unknown status " + String(status);
}

String getBoardID() {
  return WiFi.macAddress();
}

RobotServer::RobotServer() {
  client = new HTTPClient();
  aclient = new AsyncClient();
}

void RobotServer::start(Settings* se, State* st) {
  settings = se;
  state = st;

  Udp.begin(localPort);
}

size_t sendUDPPacket(int port, const String& address, const String& header, const byte* data, size_t chunkSize, size_t dataSize) { // first byte is data offset, then header string, then data
  if (chunkSize == 0) return 0;
  
  byte packetBuffer[1024];
  byte headerSize = min(255u,header.length());
  byte headerOffset = 2 + sizeof(chunkSize) + sizeof(dataSize);
  byte dataOffset = headerOffset+headerSize;
  packetBuffer[0] = headerOffset;
  packetBuffer[1] = dataOffset;

  chunkSize = min(chunkSize, size_t(1024 - dataOffset));
  memcpy((char*)packetBuffer+2, &dataSize, sizeof(dataSize));
  memcpy((char*)packetBuffer+2+sizeof(dataSize), &chunkSize, sizeof(chunkSize));
  strcpy((char*)packetBuffer+headerOffset, header.c_str());
  memcpy((char*)packetBuffer+dataOffset, data, chunkSize);
  
  size_t packetSize = dataOffset + chunkSize;
  Udp.beginPacket(address.c_str(), port);
  Udp.write(packetBuffer,packetSize);
  Udp.endPacket();

  return chunkSize;
}

void RobotServer::sendImage(Image img) {
  //Serial.print("Send image ");
  //Serial.println(img.N);

  for (size_t n = 0; n<img.N; ) {
    String header = "A";
    if (n == 0) header = "N";
    
    size_t written = sendUDPPacket(udpPort, settings->serverIPasString, header, img.data + n, img.N-n, img.N);
    n += written;
    if (written == 0) break;
  }
}

void RobotServer::disconnectSSID() {
  if (WiFi.status() == WL_DISCONNECTED || WiFi.status() == 255) {
    Serial.println("Wifi is disconnected!\n");
    state->disconnectingWifi = false;
    return;
  }

  if (!state->disconnectingWifi) {
    Serial.println(" disconnecting wifi, status: "+wifiStatusStr()+"\n");
    WiFi.disconnect();
    state->disconnectingWifi = true;
  }
}

void RobotServer::connectSSID() {
  if (WiFi.status() != WL_DISCONNECTED && WiFi.status() != WL_NO_SHIELD && WiFi.status() != WL_STOPPED) {
    disconnectSSID();
    Serial.println("Wait for wifi disconnected!\n");
    return;
  }
  
  Serial.println("Connect to SSID "+settings->ssid+", "+wifiStatusStr()+"\n");
  WiFi.begin(settings->ssid.c_str(), settings->password.c_str());
  state->needsSSIDConnect = false;
  state->doCheckWifiStatus = true;
  state->disconnectingWifi = false;
}

void RobotServer::checkSSIDConnection() {
  int status = WiFi.status();
  
  if (status == WL_DISCONNECTED || status == WL_STOPPED) {
    state->connected_ssid = "none";
    Serial.print(".");
    Serial.flush();
    delay(100);
    //Serial.println(" not yet connected to the WiFi " + ssid + " network!");
    //Serial.println("  status: " + wifiStatusStr() + "\n");
  }

  if (status == WL_NO_SSID_AVAIL) {
    state->connected_ssid = "none";
    state->doCheckWifiStatus = false;
  }
  
  if (status == WL_CONNECTED) {
    state->doCheckWifiStatus = false;
    state->connected_ssid = settings->ssid;
    state->state = "unregistred";
    Serial.println("Connected to the WiFi network " + settings->ssid + "\n");
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
  }
}

String RobotServer::get(String para) {
  String host = settings->serverIPasString;
  String uri = "https://"+host+settings->serverPath+"/"+para;
  //Serial.println(uri);

  client->begin(uri.c_str());
  int r = client->GET();
  String data;
  if (r>0) data = client->getString();
  client->end();
  return data;
}

static bool asyncBusy = false;

void RobotServer::handleCommands(String commands, void(*onCommand)(String&)) {
  SplitString splitString = strSplit(commands, '\t', '\n');
  if (splitString.N == 0) return;
        
  for (int i=0; i<splitString.N; i++) {
    String cmd = splitString.strings[i];
    //Serial.println(" got command: " + cmd);
    //if (cmd != "") (*onCommand)(cmd); // TODO: crash!

    if (cmd == "StartStream") { state->doStream = true; return; }
    if (cmd == "StopStream") { state->doStream = false; return; }
    if (cmd.substring(0,4) == "Cmd:") {
      //for (int j = 0; j<cmd.length(); j++) Serial.print(" "+String(int(cmd[j])));   
      Serial2.println(cmd); // send to arduino
    }
  }
}

void RobotServer::getCommands(void(*onCommand)(String&)) {
  if (!asyncBusy) {
    aclient->onConnect([&](void* arg, AsyncClient* aclient) {
      aclient->onData([&](void* arg, AsyncClient* aclient, void* data, size_t len) {
        String commands((const char*)data, len);
        handleCommands(commands, onCommand);
        aclient->close();
      }, NULL);

      aclient->onDisconnect([](void * arg, AsyncClient * aclient) {
        asyncBusy = false;
      }, NULL);

      //send the request
      aclient->write("getCommands\n");
    }, NULL);

    aclient->onError([](void * arg, AsyncClient * aclient, int error) {
      asyncBusy = false;
    }, NULL);

    asyncBusy = true;
    aclient->connect(settings->serverIPasString.c_str(), tcpPort);
  }
}

void RobotServer::registerAtServer() {
  Serial.println("register at server");
  state->botID = getBoardID();

  Serial.println("Connected to server, register robot!");
  String commands = get("pollCommands.php?botID="+state->botID);
  state->ports = get("register.php?uid="+state->botID+"&name="+settings->botName); // php: 'echo "ports: $bot->udpPort $bot->tcpPort";'
  Serial.println(commands);
  
  SplitString splitString = strSplit(state->ports, ' ');
  if (splitString.N > 2) {
    udpPort = splitString.strings[1].toInt();
    tcpPort = splitString.strings[2].toInt();
  }
  Serial.print("Got ports, ");
  Serial.println(state->ports);
  Serial.print(" ..using ports: ");
  Serial.print(udpPort);
  Serial.print(" and ");
  Serial.println(tcpPort);

  state->state = "running";
}

void RobotServer::pollCommands( void(*onCommand)(String&) ) {
  //Serial.println("poll");
  getCommands(onCommand); // test

  /*String commands = get("pollCommands.php");
  handleCommands(commands, onCommand);*/
}


