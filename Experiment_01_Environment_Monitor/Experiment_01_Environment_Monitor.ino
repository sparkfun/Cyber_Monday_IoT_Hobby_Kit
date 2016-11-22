/**
 * IoT Kit - Temperature and Humidity Logger
 * Author: Shawn Hymel (SparkFun Electronics)
 * Date: October 30, 2016
 * 
 * Log temperature and humidity data to a channel on
 * thingspeak.com once every 20 seconds.
 * 
 * Connections:
 *   Thing Dev |  RHT03
 *  -----------|---------
 *      3V3    | 1 (VDD) 
 *        4    | 2 (DATA)
 *      GND    | 4 (GND)
 *      
 * Development environment specifics:
 *  Arduino IDE v1.6.5
 *  Distributed as-is; no warranty is given.  
 */

#include <ESP8266WiFi.h>
#include <SparkFun_RHT03.h>
#include "ThingSpeak.h"

// WiFi and Channel parameters
const char WIFI_SSID[] = "<YOUR WIFI SSID>";
const char WIFI_PSK[] = "<YOUR WIFI PASSWORD>";
unsigned long CHANNEL_ID = <THINGSPEAK CHANNEL ID>;
const char * WRITE_API_KEY = "<THINGSPEAK WRITE API KEY>";

// Pin definitions
const int RHT03_DATA_PIN = 4;
const int LED_PIN = 5;

// Global variables
WiFiClient client;
RHT03 rht;

void setup() {

  // Set up LED for debugging
  pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi
  connectWiFi();

  // Initialize connection to ThingSpeak
  ThingSpeak.begin(client);
  
  // Call rht.begin() to initialize the sensor and our data pin
  rht.begin(RHT03_DATA_PIN);
}

void loop() {

  // Flash LED to show that we're sampling
  digitalWrite(LED_PIN, LOW);
  
  // Call rht.update() to get new humidity and temperature values from the sensor.
  int updateRet = rht.update();
  
  // If successful, the update() function will return 1.
  if (updateRet == 1)
  {
    
    // The tempC(), tempF(), and humidity() functions can be 
    // called after a successful update()
    float temp_c = rht.tempC();
    float temp_f = rht.tempF();
    float humidity = rht.humidity();

    // Write the values to our ThingSpeak channel
    ThingSpeak.setField(1, temp_c);
    ThingSpeak.setField(2, temp_f);
    ThingSpeak.setField(3, humidity);
    ThingSpeak.writeFields(CHANNEL_ID, WRITE_API_KEY);
  }
  else
  {
    
    // If the update failed, try delaying for some time
    delay(RHT_READ_INTERVAL_MS);
  }

  // Turn LED off when we've posted the data
  digitalWrite(LED_PIN, HIGH);

  // ThingSpeak will only accept updates every 15 seconds
  delay(20000);
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
