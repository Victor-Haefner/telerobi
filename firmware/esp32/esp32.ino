#include "utils.h"
#include "wifi.h"
#include "esp_camera.h"
#include "trine_cam.h"
#include "config_site.h"
#include "esp32-hal-ledc.h"

Settings settings;
State state;

RobotServer botServer;
RobotConfigurator configurator;

void setup() {
  configurator.readPreferences(&settings);

  state.needsSSIDConnect = true;
  state.doCheckWifiStatus = true;
  state.disconnectingWifi = false;
  state.state = "unconnected";
  state.doStream = false;
  state.connected_ssid = "none";

  if (settings.botModel == "Elegoo") {
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, 33, 4); //Serial ESP32 Cam
    //pinMode(13, OUTPUT);
    //digitalWrite(13, HIGH);
  }

  if (settings.botModel == "Telerobi") {
    Serial2.begin(9600, SERIAL_8N1, 3, 1); //Serial ESP32 Cam
    ledcAttachPin(4, 0);
    ledcSetup(0, 4000, 8);
    ledcWrite(0, 15); //255
  }

  Serial.println("ESP32 set up for model "+settings.botModel);

  //delay(3000);

  // open access point for local config
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.softAP("trineBot", "bot12345"); // Password with at least 8 characters, if less, the AP will be named ELEGOO-something and be open access
  IPAddress IP = WiFi.softAPIP();
  Serial.println("AP IP and mac address: ");
  Serial.println(IP);
  Serial.println(WiFi.macAddress());

  // load ssid, password, server IP address and robot name from preferences
  botServer.start(&settings, &state);
  configurator.start(&settings, &state);
  
  /*Serial.println(" init camera");
  if (settings.botModel == "Elegoo") init_camera("M5STACK_WIDE");
  if (settings.botModel == "Telerobi") init_camera("AI_THINKER");*/
  Serial.println("Finished setup");
}

void on_stream_capture(Image img) {
  botServer.sendImage(img);
}

void processMessage(String& data) {
  Serial.println(data);
  if (data == "StartStream") { state.doStream = true; return; }
  if (data == "StopStream") { state.doStream = false; return; }
  if (data.substring(0,4) == "Cmd:") Serial2.println(data); // send to arduino
}

void testCmd() {
  delay(5000);
  Serial.println("send command!");
  //Serial2.println("f");
  Serial2.print("cmd:A"); 
  Serial2.print(char(1)); 
  Serial2.print("S"); 
  Serial2.print(char(180)); 
  Serial2.print("D"); 
  Serial2.print(char(5)); 
  Serial2.print("O"); 
  Serial2.print(char(0)); 

  Serial2.print(char(0)); 
  Serial2.print(char(10));
}

void loop() {
  //testCmd();
  if (state.state == "unregistred") botServer.registerAtServer();
  
  botServer.pollCommands(processMessage); // TODO: induces lag in camera stream!
  //if (state.doStream) capture_cam(on_stream_capture);
  configurator.processHTTPinput(); // local network, SSID: trineBot
  
  if (state.needsSSIDConnect) botServer.connectSSID();
  if (state.doCheckWifiStatus) botServer.checkSSIDConnection();
}
