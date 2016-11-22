/**
 * IoT Kit - Twitter Monitor
 * Author: Shawn Hymel (SparkFun Electronics)
 * Date: October 31, 2016
 * 
 * Set up an IFTTT recipe to send an HTTP request to ThingSpeak
 * each time a hashtag is posted to Twitter. This sketch monitors
 * the ThingSpeak channel, flashes an LED whenever a new Tweet
 * was logged, and then clears the channel.
 * 
 * Connections:
 *  None! We'll be using the onboard LED
 *      
 * Development environment specifics:
 *  Arduino IDE v1.6.5
 *  Distributed as-is; no warranty is given.  
 */

#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

// WiFi and Channel parameters
const char WIFI_SSID[] = "<YOUR WIFI SSID>";
const char WIFI_PSK[] = "<YOUR WIFI PASSWORD>";
unsigned long CHANNEL_ID = <YOUR THINGSPEAK CHANNEL ID>;
const char * READ_API_KEY = "<YOUR CHANNEL READ API KEY>";
const char * ACCOUNT_API_KEY = "<YOUR ACCOUNT API KEY>";

// Remote site information
const char HTTP_SITE[] = "api.thingspeak.com";
const int HTTP_PORT = 80;

// Pin definitions
const int LED_PIN = 5;

// Global variables
WiFiClient client;

void setup() {
  
  // Set up LED
  pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi
  connectWiFi();

  // Initialize connection to ThingSpeak
  ThingSpeak.begin(client);

  // For debugging
  Serial.begin(9600);

  // Send clear request to channel (start fresh)
  if (!clearChannel()) {
    Serial.println("Error connecting to ThingSpeak API");
  } else {
    Serial.println("Connected and listening!");
  }
}

void loop() {

  int tweet_result;
  unsigned long timestamp;

  // See if there was a Twitter hit on our channel
  tweet_result = ThingSpeak.readIntField(CHANNEL_ID, 1,READ_API_KEY);

  // See if we had at least one Tweet since last polling
  if ( tweet_result == 1 ) {

    // Flash LED on for 1 second if we got a Tweet
    digitalWrite(LED_PIN, LOW);
    timestamp = millis();

    // Clear the data channel
    if ( !clearChannel() ) {
      Serial.println("Error: Could not clear channel!");
    } else {
      Serial.println("Channel cleared");
    }

    // Turn LED off after 1 second
    while ( millis() < (timestamp + 1000) ) {
      delay(1);
    }
    digitalWrite(LED_PIN, HIGH);
  }

  // IFTTT won't be able to write more than once per 15 seconds
  delay(15000);
}

// Attempt to connect to WiFi
void connectWiFi() {
  
  byte led_status = 0;
  
  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);
  
  // Initiate connection with SSID and PSK
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  
  // Blink LED while we wait for WiFi connection
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(LED_PIN, led_status);
    led_status ^= 0x01;
    delay(100);
  }
  
  // Turn LED off when we are connected
  digitalWrite(LED_PIN, HIGH);
}

// Send HTTP DELETE request to clear the channel
bool clearChannel() {
  
  // Attempt to make a connection to the remote server
  if ( !client.connect(HTTP_SITE, HTTP_PORT) ) {
    return false;
  }
  
  // Make an HTTP DELETE request
  client.print("DELETE /channels/");
  client.print(String(CHANNEL_ID));
  client.println("/feeds HTTP/1.1");
  client.print("Host: ");
  client.println(HTTP_SITE);
  client.println("Accept: */*");
  client.println("Accept-Encoding: gzip, deflate");
  client.println("Connection: close");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: 24");
  client.println();
  client.print("api_key=");
  client.println(ACCOUNT_API_KEY);
  
  return true;
}
