#ifndef WIFI_H
#define WIFI_H

#include "utils.h"

class Image;
class HTTPClient;
class AsyncClient;

String wifiStatusStr();
String getBoardID();

class RobotServer {
  private:
    HTTPClient* client = 0;
    AsyncClient* aclient = 0;
    Settings* settings = 0; 
    State* state = 0;
    int udpPort = 4500;
    int tcpPort = 4600;

    String get(String para);
    void getCommands(void(*onCommand)(String&));
    void handleCommands(String commands, void(*onCommand)(String&));

  public:
    RobotServer();

    void start(Settings* settings, State* state);

    void disconnectSSID();
    void connectSSID();
    void checkSSIDConnection();

    void registerAtServer();
    void pollCommands(void(*onCommand)(String&));

    void sendImage(Image img);
};

#endif // WIFI_H
