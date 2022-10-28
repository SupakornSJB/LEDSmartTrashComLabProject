#ifdef ESP32
	#include <WiFi.h>
#else
 	#include <ESP8266WiFi.h>
#endif
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>
#include <Ultrasonic.h>

// Replace with your SSID and Password
char auth[] = "YOUR TOKEN";
const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";
float range = 100;
int rPin = 14;
int yPin = 12;
18
int gPin = 13;
int trigPin = 5;
int echoPin = 4;
int rfr = 1;

// Do not configure this
String pinStatus = "NONE";
Ultrasonic ultrasonic(trigPin, echoPin);
float dist = 0;
// Replace with your unique IFTTT URL resource
const char* resource = "https://maker.ifttt.com/trigger/Trash/with/key/cvjPzQUq2_mgstoLtAEFn";
// How your resource variable should look like, but with your own API KEY (that API KEY below 
is just an example):
//const char* resource = "/trigger/bme280_readings/with/key/nAZjOphL3d-ZO4N3k64-
1A7gTlNSrxMJdmqy3";
// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";
19
// Time to sleep
uint64_t uS_TO_S_FACTOR = 1000000; // Conversion factor for micro seconds to seconds
// sleep for 30 minutes = 1800 seconds
uint64_t TIME_TO_SLEEP = 2;
#define SEALEVELPRESSURE_HPA (1013.25)

void setup() {
	Serial.begin(9600); 
 delay(2000);
 Blynk.begin(auth, ssid, password);
 pinMode(rPin, OUTPUT);
 pinMode(yPin, OUTPUT);
 pinMode(gPin, OUTPUT);
 initWifi();
}
20
BLYNK_WRITE(V1) {
 range = param.asInt();
}
BLYNK_WRITE(V2) {
 rfr = param.asInt();
}
void loop() {
 // sleeping so wont get here
 Blynk.run();
 dist = ultrasonic.read();
 Blynk.virtualWrite(V0, String(((range-dist)/ range) * 100) + " %");
 if (dist < 0.25*range) {
 digitalWrite(rPin, HIGH);
 digitalWrite(yPin, LOW);
 digitalWrite(gPin, LOW);
 pinStatus = "RED";
21
 Blynk.notify("HEY!, come pick up your trash!");
 } else if (dist < 0.5*range) {
 digitalWrite(rPin, LOW);
 digitalWrite(yPin, HIGH);
 digitalWrite(gPin, LOW);
 pinStatus = "YELLOW";
 } else {
 digitalWrite(rPin, LOW);
 digitalWrite(yPin, LOW);
 digitalWrite(gPin, HIGH);
 pinStatus = "GREEN";
 }
 Serial.printf("distance: %f cm\n", dist);
Serial.printf("Slider: %f cm\n", range);
 Serial.printf("Waiting: %d seconds\n", rfr);
 Serial.println("Pin status: " + pinStatus);
 Serial.println("***************************");
 makeIFTTTRequest();
22
 delay(1000 * rfr);
}
// Establish a Wi-Fi connection with your router
void initWifi() {
 Serial.print("Connecting to: "); 
 Serial.print(ssid);
 WiFi.begin(ssid, password); 
 int timeout = 10 * 4; // 10 seconds
 while(WiFi.status() != WL_CONNECTED && (timeout-- > 0)) {
 delay(250);
 Serial.print(".");
 }
 Serial.println("");
 if(WiFi.status() != WL_CONNECTED) {
 Serial.println("Failed to connect, going back to sleep");
 }
23
 Serial.print("WiFi connected in: "); 
 Serial.print(millis());
 Serial.print(", IP address: "); 
 Serial.println(WiFi.localIP());
}
// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
 Serial.print("Connecting to "); 
 Serial.print(server);
 WiFiClient client;
 int retries = 5;
 while(!!!client.connect(server, 80) && (retries-- > 0)) {
 Serial.print(".");
 }
 Serial.println();
 if(!!!client.connected()) {
 Serial.println("Failed to connect...");
24
 }
 Serial.print("Request resource: "); 
 Serial.println(resource);
 String jsonObject = String("{\"value1\":\"") + dist + "\",\"value2\":\"" 
 + range + "\",\"value3\":\"" + pinStatus + "\"}";
 client.println(String("POST ") + resource + " HTTP/1.1");
 client.println(String("Host: ") + server); 
 client.println("Connection: close\r\nContent-Type: application/json");
 client.print("Content-Length: ");
 client.println(jsonObject.length());
 client.println();
 client.println(jsonObject);
 int timeout = 5 * 10; // 5 seconds 
 while(!!!client.available() && (timeout-- > 0)){
 delay(100);
 }
 if(!!!client.available()) {
25
 Serial.println("No response...");
 }
 while(client.available()){
 Serial.write(client.read());
 }