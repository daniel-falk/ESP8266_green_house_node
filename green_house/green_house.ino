#include <ESP8266WiFi.h> // Wifi
#include <ESP8266WebServer.h> // HTTP web server
#include "i2c.h"
#include "log.h"

#define NET_NAME "***"
#define NET_PASS "***"

#define LSTAT D3 // Status LED (Extern LED)
#define LACT D4 // Activity LED (Blue chip LED)

#define BAUD 115200
#define SERVER_PORT 80

ESP8266WebServer server(SERVER_PORT);

void setup() {

  delay(1000);
  
  Serial.begin(BAUD);
  pinMode(LSTAT, OUTPUT);
  pinMode(LACT, OUTPUT);

  Serial.println("");
  Serial.println("Connecting to wifi...");
  WiFi.begin(NET_NAME, NET_PASS);
  Serial.print("Waitning for connection");

  int timeout = 50;
  while(WiFi.status() != WL_CONNECTED){
    digitalWrite(LSTAT, HIGH);
    delay(50);
    digitalWrite(LSTAT, LOW);
    delay(50);
    Serial.print(".");
    if(!timeout--) {
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.print("Connected with IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  server.on("/", web_main);

  server.begin();

  info("Init I2C communication...");
  bool s = i2c_init();
  if (s)
    info("ok");
  else
    info("failed");
}

void loop() {
  digitalWrite(LACT, HIGH);
  delay(20);
  digitalWrite(LACT, LOW);
  delay(20);
  server.handleClient();
}

void web_main() {
  info("Client request");
  int t;
  i2c_read_weather(&t, NULL, NULL);
  int buf_len = 1024;
  char buf[buf_len];
  snprintf(buf, buf_len,
    "<html>"
      "<head>"
        "<title>ESP8266 weather station - Green House</title>"
        "<style>"
          ".content {border:1px solid #000; padding:20px; width: 100%; margin: 20px;}"
         "</style>"
       "</head>"
      "<body>"
        "<div class='content'>Temperature: %d</div>"
      "</body>"
     "</html>", 
     t); 

  server.send(200, "text/html", buf);
}

