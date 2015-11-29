/*  Derived from https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient.ino
 * 
 *  This sketch sends data via HTTP GET requests to maker.ifttt.com service.
 *
 *  You need to get a privateKey at maker.ifttt and paste it
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>

const char* ssid     = "your-ssid";
const char* password = "your-password";

const char* host = "maker.ifttt.com";
const char* privateKey = "your-privateKey";
const char* eventName   = "esp8266_doorbell";

const int retryLimit = 10;

// Using a static IP should save time
IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);  
IPAddress subnet(255, 255, 255, 0);  

/* Returns true if the request was sent and a reply was 
  received properly, else returns false */
bool sendNotification() {
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }
  
  // We now create a URI for the request
  String url = "/trigger/";
  url += eventName;
  url += "/with/key/";
  url += privateKey;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection\n");
  delay(100);

  return true;
}


void setup() {
  /* Our button is on CH_PD, this will keep CH_PD high
    during the duration of sending the data */
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);  
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  bool worked = sendNotification();
  int tries = 0;
  while (!worked && tries < retryLimit){
     delay(500);
     Serial.println("Try #" + (tries+1));  
     worked = sendNotification();
     tries++;
  }
}

void loop() {
  /* Turns off GPIO2 which should bring CH_PD low */
  digitalWrite(2, LOW);
}

