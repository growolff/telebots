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

// Initialize Telegram BOT
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int ledPin = 2;
const int relePin = 5; //D1 -> GPIO5
bool ledState = HIGH;

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
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

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/led_on to turn GPIO ON \n";
      welcome += "/led_off to turn GPIO OFF \n";
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
      bot.sendMessage(chat_id, "Open", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      digitalWrite(relePin, HIGH);
      delay(1000);
      bot.sendMessage(chat_id, "Close", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      digitalWrite(relePin, LOW);
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
}
