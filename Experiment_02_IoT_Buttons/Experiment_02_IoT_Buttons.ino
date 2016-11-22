/**
 * IoT Kit - IoT Buttons
 * Author: Shawn Hymel (SparkFun Electronics)
 * Date: October 30, 2016
 * 
 * Push one of three buttons to make something happen on the
 * Internet using IFTTT. Button C will generate a random number
 * between 1-100 to post to ThingSpeak.
 * 
 * Connections:
 *   Thing Dev |  Button
 *  -----------|---------
 *       4     |    A 
 *      12     |    B
 *      13     |    C
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
const char * WRITE_API_KEY = "<YOUR CHANNEL WRITE API KEY>";

// Pin definitions
const int BTN_A_PIN = 4;
const int BTN_B_PIN = 12;
const int BTN_C_PIN = 13;
const int LED_PIN = 5;

// Global variables
WiFiClient client;
int last_btn_a = HIGH;
int last_btn_b = HIGH;
int last_btn_c = HIGH;

void setup() {

  // Set up LED for debugging
  pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi
  connectWiFi();

  // Initialize connection to ThingSpeak
  ThingSpeak.begin(client);

  // Seed the random number generator
  randomSeed(analogRead(A0));
}

void loop() {

  int btn_a;
  int btn_b;
  int btn_c;
  int rnd;

  // Look for a falling edge on button A with debounce
  btn_a = digitalRead(BTN_A_PIN);
  if ( (btn_a == LOW) && (last_btn_a == HIGH) ) {
    delay(30);
    if ( digitalRead(BTN_A_PIN) == LOW ) {
      digitalWrite(LED_PIN, LOW);
      ThingSpeak.writeField(CHANNEL_ID, 1, 1, WRITE_API_KEY);
    }
  }
  last_btn_a = btn_a;

  // Look for a falling edge on button B with debounce
  btn_b = digitalRead(BTN_B_PIN);
  if ( (btn_b == LOW) && (last_btn_b == HIGH) ) {
    delay(30);
    if ( digitalRead(BTN_B_PIN) == LOW ) {
      digitalWrite(LED_PIN, LOW);
      ThingSpeak.writeField(CHANNEL_ID, 2, 1, WRITE_API_KEY);
    }
  }
  last_btn_b = btn_b;

  // Look for a falling edge on button C with debounce
  // Randomly choose a number between 1-100 and post it
  btn_c = digitalRead(BTN_C_PIN);
  if ( (btn_c == LOW) && (last_btn_c == HIGH) ) {
    delay(30);
    if ( digitalRead(BTN_C_PIN) == LOW ) {
      digitalWrite(LED_PIN, LOW);
      rnd = random(1, 101);
      ThingSpeak.writeField(CHANNEL_ID, 3, rnd, WRITE_API_KEY);
    }
  }
  last_btn_c = btn_c;

  // Turn off LED
  digitalWrite(LED_PIN, HIGH);
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
