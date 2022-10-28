#ifndef CONFIG_SITE_H
#define CONFIG_SITE_H

#include <Arduino.h>
#include <WebServer.h>

class WiFiClient;
struct Settings;
struct State;

class RobotConfigurator {
  private:
    WiFiServer server;
    Settings* settings = 0; 
    State* state = 0;

  public:
    RobotConfigurator();

    void start(Settings* settings, State* state);

    void handleClientData(String data, WiFiClient& client);
    void servePageSettings(WiFiClient& client);
    void readPreferences(Settings* settings);
    void processHTTPinput();
};

#endif // CONFIG_SITE_H