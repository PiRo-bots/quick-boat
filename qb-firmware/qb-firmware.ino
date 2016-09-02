
/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>
#include <Hash.h>

#include <Servo.h>

/* Set these to your desired credentials. */
const char *ssid = "quickboat";
const char *password = "thereisnospoon";

//servo configuration

Servo rudderServo;
int rudderServoPin =16;
int rudderPos = 0;
int rudderOffset = 3;

int speed = 0;

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

#define USE_SERIAL Serial
WebSocketsServer webSocket = WebSocketsServer(81);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        
        // send message to client
        webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            {
              USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
              
              //
              // send message to client
              // webSocket.sendTXT(num, "message here");

              if(payload[0] == 'r'){//control the rudder
                USE_SERIAL.print("Moving rudder...");
                String msg = String((char*)payload);
                int val = msg.substring(1).toInt();
                rudderPos = max(0,min(val,180));
                USE_SERIAL.printf("to [%u] \n", rudderPos);
                rudderServo.write(rudderPos);
                delay(3);
              }
              if(payload[0] == 's'){//control the speed
                String msg = String((char*)payload);
                int val = msg.substring(1).toInt();
                speed = val
              }
                
                
              
              
              /*if(payload[0] == 'l'){
                if(rudderPos < 180){
                  rudderPos += rudderOffset;
                }
                rudderServo.write(rudderPos);              // tell servo to go to position in variable 'pos'
                delay(3);                       // waits 15ms for the servo to reach the position
              }
              if(payload[0] == 'r'){
                if(rudderPos >= 0){
                  rudderPos -=rudderOffset;        
                }
                rudderServo.write(rudderPos);              // tell servo to go to position in variable 'pos'
                delay(3);    
              }*/
            }
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);

            // send message to client
            // webSocket.sendBIN(num, payload, lenght);
            break;
    }

}




void setup() {
  USE_SERIAL.begin(115200);

  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

   for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
  
  USE_SERIAL.println();
  USE_SERIAL.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  USE_SERIAL.print("AP IP address: ");
  USE_SERIAL.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  USE_SERIAL.println("HTTP server started");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


  //configure rudder servo
  rudderServo.attach(rudderServoPin);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

