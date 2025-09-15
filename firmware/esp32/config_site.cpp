#include "config_site.h"
#include "utils.h"
#include "trine_cam.h"
#include "wifi.h"

//#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;

Map parseUri(String uri) {
  Serial.println(" parseUri: " + uri);
  Map params;

  SplitString splitString = strSplit(uri, '?');
  String left = splitString.strings[0];
  String right = splitString.strings[1];
  
  SplitString splitString2 = strSplit(left, '/');
  if (splitString2.N > 0) {
    Serial.println("   uri path: " + splitString2.strings[splitString2.N-1]);
    params.set("path", splitString2.strings[splitString2.N-1]);
  }
  if (splitString2.N <= 1) return params;
  
  SplitString splitString3 = strSplit(right, '&');
  if (splitString3.N == 0) return params;

  SplitString pairs = splitString3;
  for (int i=0; i<pairs.N; i++) {
    String pair = pairs.strings[i];
    SplitString splitString4 = strSplit(pair, '=');
    if (splitString4.N != 2) continue;
    Serial.println("   param: " + splitString4.strings[0] + " : " + splitString4.strings[1]);
    params.set(splitString4.strings[0], splitString4.strings[1]);
  }
  
  return params;
}

void closeConnection(WiFiClient& client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type:text/html");
    client.println("Connection: close");
    client.println();
    client.println();
}

WiFiClient* tmpClient = 0;

void on_capture(Image img) {
  Serial.println("Got cam capture: ");
  Serial.print(" N pixels: ");
  Serial.println(img.N, DEC);
  Serial.print(" width: ");
  Serial.println(img.width, DEC);
  Serial.print(" height: ");
  Serial.println(img.height, DEC);

  String imgData = img.toString();

  tmpClient->println("HTTP/1.1 200 OK");
  tmpClient->println("Content-Type: image/jpeg");
  tmpClient->print("Content-Length: ");
  tmpClient->println(img.N);
  tmpClient->println("Connection: close");
  tmpClient->println();
  tmpClient->println(imgData);
  tmpClient->println();
  tmpClient->println();
}

RobotConfigurator::RobotConfigurator() : server(80) {}

void RobotConfigurator::start(Settings* se, State* st) {
  settings = se;
  state = st;
  server.begin();
}

void RobotConfigurator::readPreferences(Settings* settings) {
  preferences.begin("credentials", false);
  settings->ssid = preferences.getString("ssid", "trine");
  settings->password = preferences.getString("password", "76543210");
  settings->serverIPasString = preferences.getString("serverIP", "192.168.0.100");
  settings->botName = preferences.getString("botName", "DummyBot");
  settings->botModel = preferences.getString("botModel", "Telerobi");
  settings->serverPath = preferences.getString("serverPath", "/web/trine");
  preferences.end();

  //settings->ssid = "KIT-IoT";
  //settings->password = "5yj(835we%`eb,H:W_ltpocC";

  Serial.println(" loaded ssid: " + settings->ssid);
  Serial.println(" loaded pass: " + settings->password);
  Serial.println(" loaded serv: " + settings->serverIPasString);
  Serial.println(" loaded name: " + settings->botName);
  Serial.println(" loaded model: "+ settings->botModel);
  Serial.println(" loaded server path: "+ settings->serverPath);
}

void RobotConfigurator::handleClientData(String data, WiFiClient& client) {  
  Serial.println(" handleClientData: " + data);
  SplitString splitString = strSplit(data, ' ');
  if (splitString.N <= 1) { closeConnection(client); return; }
  String uri = splitString.strings[1];

  delay(1000);
  
  Map params = parseUri(uri);
  String path = params.get("path");
  //Serial.println(" map size: ");
  //Serial.println(params.N, DEC);

  Serial.println(" got path: " + path);
  
  if (path == "settings") {
    Serial.println("  serve settings page");
    servePageSettings(client);
    return;
  }

  if (path == "captureCam") {
    Serial.println("    got captureCam");
    tmpClient = &client;
    capture_cam(on_capture);
    tmpClient = 0;    
    return;
  }
  
  if (path == "handleSettings") {
    closeConnection(client);
    settings->ssid = params.get("ssid");
    settings->password = params.get("password");
    settings->serverIPasString = params.get("serveradd");
    settings->botName = params.get("bot");
    settings->botModel = params.get("model");
    if (settings->ssid == "") return;
    Serial.println("got new settings:");
    Serial.println("    got ssid: " + settings->ssid);
    Serial.println("    got pass: " + settings->password);
    Serial.println("    got server address: " + settings->serverIPasString);
    Serial.println("    got robot name: " + settings->botName);
    Serial.println("    got robot model: " + settings->botModel);
    preferences.begin("credentials", false);
    preferences.putString("ssid", settings->ssid);
    preferences.putString("password", settings->password);
    preferences.putString("serverIP", settings->serverIPasString);
    preferences.putString("botName", settings->botName);
    preferences.putString("botModel", settings->botModel);
    preferences.end();
    state->needsSSIDConnect = true;
    state->state = "unconnected";
    return;
  }
  
  closeConnection(client);
}

void RobotConfigurator::servePageSettings(WiFiClient& client) {
  Serial.println("servePageSettings\n");

  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type:text/html");
  client.println("Connection: close");
  client.println();
  
  // HTML web page to handle 2 input fields (SSID, Password)
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");

  // CSS
  client.println("<style>");
  client.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 2px 5px;");
  client.println(".button2 {background-color: #555555;}");
  client.println("</style></head><body>");

  // Heading
  client.println("<h1>Telerobi</h1>");
  
  // Robot Server
  client.println("<h2>Server</h2>");
  client.println("<div>Server Address:<input type='text' name='Server Address' id = 'input_serveraddress' value='" + settings->serverIPasString + "'></div>");
  client.println("<div>Robot Name:<input type='text' name='Robot Name' id = 'input_robotname' value='" + settings->botName + "'></div>");

  String model = settings->botModel;
  client.println("<select id='RobotModel'>");
  if (model != "Telerobi") client.println("<option value='Telerobi'>Telerobi</option>");
  else client.println("<option value='Telerobi' selected>Telerobi</option>");
  if (model != "Elegoo") client.println("<option value='Elegoo'>Elegoo</option>");
  else client.println("<option value='Elegoo' selected>Elegoo</option>");
  client.println("</select>");

  // Local SSID and Password
  client.println("<h2>Wi-Fi Login</h2>");
  client.println("<div>SSID:<input type='text' name='SSID' id = 'input_ssid' value='" + settings->ssid + "'></div>");
  client.println("<div>Password:<input type='text' name='Password' id = 'input_password' value='" + settings->password + "'></div>");

  client.println("<p><button class=\"button\" onclick=\"submitSettings()\">Submit</button></p>");
  client.println("<p><button class=\"button\" onclick=\"getImage()\">getImage</button></p>");
  client.println("<p>Bot has the MAC: " + WiFi.macAddress() + "</p>");
  client.println("<p>Configured to connect to: " + settings->ssid + "</p>");
  client.println("<p>Wifi status: " + wifiStatusStr() + "</p>");
  if (WiFi.status() == WL_CONNECTED) {
    client.println("<p>Connected to: " + state->connected_ssid + ", "+WiFi.SSID() + "</p>");
    client.println("<p>UDP ports: " + state->ports + "</p>");
  } else {
    client.println("<p>Not Connected</p>");
  }
  client.println("<img id='testImg'></img>");
  client.println("</body></html>");

  //Function for saving user input
  client.println("<script type='text/javascript'>");

  client.println("function send(data) {");
  client.println("  var xmlHttp = new XMLHttpRequest();");
  client.println("  xmlHttp.open( 'GET', window.location.hostname+'/'+data, false );");
  client.println("  xmlHttp.send( null );");
  client.println("  return xmlHttp.responseText;");
  client.println("}");

  client.println("function submitSettings() {");
  client.println("  var ssid = document.getElementById('input_ssid').value;");
  client.println("  var pass = document.getElementById('input_password').value;");
  client.println("  var bot = document.getElementById('input_robotname').value;");
  client.println("  var serveradd = document.getElementById('input_serveraddress').value;");
  client.println("  var cbox = document.getElementById('RobotModel');");
  client.println("  var model = cbox.options[cbox.selectedIndex].value;");
  client.println("  send('handleSettings?ssid='+ssid+'&password='+pass+'&serveradd='+serveradd+'&bot='+bot+'&model='+model);");
  client.println("}");

  client.println("function getImage() {");
  client.println("  var data = send('captureCam');");
  client.println("  var img = document.getElementById('testImg');");
  client.println("  img.src = 'data:image/png;base64,'+data;");
  client.println("  img.alt = 'test cam capture';");
  client.println("}");

  client.println("</script>");

   // The HTTP response ends with another blank line
  client.println();
  // Break out of the while loop
}

void RobotConfigurator::processHTTPinput() {
  // Variable to store the HTTP request
  String header;

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            handleClientData(header, client);
            break;
          }
          currentLine = "";
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    //Serial.println("");
  }
}


