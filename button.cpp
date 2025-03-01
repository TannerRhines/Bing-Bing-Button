#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// WiFi Credentials
const char* ssid = "Dandelion Manor";  // insert your wifi address
const char* password = "9073787544D";  // insert your wifi password

// Static IP Config (with DNS added)
IPAddress staticIP();
IPAddress gateway();
IPAddress subnet();
IPAddress dns();  // Google DNS

// API Gateway URL
const char* aws_api_url = ""; // insert aws api gateway url 

// Button & DFPlayer Mini
const int buttonPin = 2;
SoftwareSerial mySerial(5, 4);
DFRobotDFPlayerMini myDFPlayer;

unsigned long lastPress = 0;

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);
    pinMode(buttonPin, INPUT_PULLUP);

    connectWiFi();

    Serial.println("Initializing DFPlayer Mini...");
    delay(1000);
    if (!myDFPlayer.begin(mySerial)) {  
        Serial.println("DFPlayer Mini not found! Check wiring.");
        while (true);
    }
    Serial.println("DFPlayer Mini connected!");
    myDFPlayer.volume(20);
}

void loop() {
    checkWiFi();

    if (digitalRead(buttonPin) == LOW && millis() - lastPress > 500) {
        lastPress = millis();
        handleButtonPress();
    }
}

void handleButtonPress() {
    Serial.println("Button Pressed! Playing MP3...");
    myDFPlayer.play(1);
    sendSNS();
}

void sendSNS() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        Serial.println("Sending request to API Gateway...");
        http.begin(client, aws_api_url);  // Keep HTTPS for security

        http.addHeader("Content-Type", "application/json");
        String payload = "{}";

        int httpResponseCode = http.POST(payload);

        Serial.print("AWS API Response Code: ");
        Serial.println(httpResponseCode);

        if (httpResponseCode > 0) {
            Serial.println("AWS API Full Response:");
            Serial.println(http.getString());
            Serial.println("SNS Notification Sent!");
        } else {
            Serial.print("Error sending SNS: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi Disconnected - Cannot Send SNS!");
    }
}

void connectWiFi() {
    Serial.println("Configuring Static IP...");
    WiFi.config(staticIP, gateway, subnet, dns);

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 10) {
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("Device IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.print("\nFailed to connect to WiFi! Status Code: ");
        Serial.println(WiFi.status());
    }
}

void checkWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Lost - Reconnecting...");
        connectWiFi();
    }
}