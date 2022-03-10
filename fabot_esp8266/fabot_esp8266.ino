/*******************************************************************
   Telegram Bot for esp8266

*******************************************************************/
#include "secrets.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> //
#include <ArduinoJson.h> // version 6.15.2

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

const int num_user = 3;
const char* users[num_user] = {ID_GROW, ID_VCR, ID_JUC};

// Initialize Telegram BOT
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int ledPin = 2;
const int relePin = 14; //D5 -> GPIO14
bool ledState = HIGH;

// Checks for new messages every 1 second.
int botRequestDelay = 500;
unsigned long lastTimeBotRan;

void WiFi_connect() {
  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("..WiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

}

void setup() {
  Serial.begin(115200);

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
#endif

  WiFi_connect();

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  pinMode(relePin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, ledState); // initialize pin as off
  digitalWrite(relePin, !ledState);
  delay(100);
}


void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for( int n = 0; n<num_user ; n++){
    // Chat id of the requester
    String chat_id = String(bot.messages[0].chat_id);
    if (chat_id == users[n]) {
        // Print the received message
        String text = bot.messages[0].text;
        Serial.println(text);

        String from_name = bot.messages[0].from_name;

        if (text == "/start") {
          String welcome = "Welcome, " + from_name + ".\n";
          welcome += "Use the following commands to control your outputs.\n\n";
          welcome += "/led_on to turn board led ON \n";
          welcome += "/led_off to turn board led OFF \n";
          welcome += "/open to open the door \n";
          welcome += "/state to request current GPIO state \n";
          bot.sendMessage(chat_id, welcome, "");
        }

        if (text == "/led_on") {
          bot.sendMessage(chat_id, "LED state set to ON", "");
          ledState = LOW;
          digitalWrite(ledPin, ledState);
        }

        if (text == "/led_off") {
          bot.sendMessage(chat_id, "LED state set to OFF", "");
          ledState = HIGH;
          digitalWrite(ledPin, ledState);
        }
        if (text == "/open") {
          bot.sendMessage(chat_id, "Open door", "");
          ledState = LOW;
          digitalWrite(ledPin, ledState);
          digitalWrite(relePin, !ledState);
          delay(500);
          //bot.sendMessage(chat_id, "Close", "");
          ledState = HIGH;
          digitalWrite(ledPin, ledState);
          digitalWrite(relePin, !ledState);
        }
        if (text == "/state") {
          if (!digitalRead(ledPin)) {
            bot.sendMessage(chat_id, "LED is ON", "");
          }
          else {
            bot.sendMessage(chat_id, "LED is OFF", "");
          }
        }
      }
  else {
    //bot.sendMessage(chat_id, "Unauthorized user", "");
    continue;
  }
}
}
