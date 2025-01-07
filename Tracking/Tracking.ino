#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> // Use WiFiClientSecure for HTTPS
#include <ESP8266HTTPClient.h>

// GPS setup
TinyGPSPlus gps;
SoftwareSerial gpsSerial(D1, D2); 

// WiFi credentials
const char* ssid = "TehseenPhone";        // Replace with your WiFi SSID
const char* password = "a1b2c3d4";        // Replace with your WiFi Password

// Backend server details
const char* server = "https://nice-lamprey-neat.ngrok-free.app/api/location"; // Replace with your actual ngrok URL

void setup() {
  Serial.begin(115200);       // Debugging serial monitor
  gpsSerial.begin(9600);      // GPS module communication baud rate

  // Connect to WiFi
  connectToWiFi();
  
  // Test server connection
  testServerConnection();
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    connectToWiFi();
  }

  // Read GPS data
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    Serial.print(c);  // Debug: Print raw GPS data
    if (gps.encode(c)) {
      if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        Serial.printf("Latitude: %.6f, Longitude: %.6f\n", latitude, longitude);

        // Send data to the server
        sendGPSData(latitude, longitude);
      } else {
        Serial.println("Waiting for a valid location...");
      }
    }
  }
}

// Function to connect to WiFi
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempts++;

    if (attempts > 20) {
      Serial.println("\nFailed to connect to WiFi. Rebooting...");
      ESP.restart();
    }
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to test server connection with a GET request
void testServerConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Optional: Use this to bypass SSL certificate verification

    HTTPClient http;
    Serial.println("Testing server connection...");

    if (http.begin(client, server)) {
      int httpCode = http.GET(); // Send a GET request

      if (httpCode > 0) {
        Serial.printf("GET Response Code: %d\n", httpCode);
        Serial.println("Server response: ");
        Serial.println(http.getString());
      } else {
        Serial.printf("GET failed: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.println("Failed to connect to the server for GET request!");
    }
  } else {
    Serial.println("WiFi not connected!");
  }
}

// Function to send GPS data to the backend
void sendGPSData(float latitude, float longitude) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client; // Use WiFiClientSecure for HTTPS
    client.setInsecure(); // Optional: Use this to bypass SSL certificate verification

    HTTPClient http;
    Serial.println("Attempting to connect to the server...");

    if (http.begin(client, server)) {
      http.addHeader("Content-Type", "application/json");

      // Construct payload
      String payload = "{\"latitude\":\"" + String(latitude, 6) + "\",\"longitude\":\"" + String(longitude, 6) + "\"}";
      Serial.println("Sending payload:");
      Serial.println(payload);

      // Send HTTP POST request
      int httpCode = http.POST(payload);

      // Debug HTTP response
      if (httpCode > 0) {
        Serial.printf("HTTP Response Code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
          Serial.println("Data sent successfully!");
          Serial.println("Server response: ");
          Serial.println (http.getString());
        } else {
          Serial.printf("Error in response: %s\n", http.errorToString(httpCode).c_str());
        }
      } else {
        Serial.printf("HTTP POST failed: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.println("Failed to connect to the server for POST request!");
    }
  } else {
    Serial.println("WiFi not connected!");
  }
}