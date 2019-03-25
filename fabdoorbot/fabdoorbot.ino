/*******************************************************************
   An example of bot that echos back any messages received
*                                                                  *
   written by Giacarlo Bacchio (Gianbacchio on Github)
   adapted by Brian Lough
*******************************************************************/
#include "fabDoorToken.h"

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// Initialize Wifi connection to the router
char ssid[] = WIFI_SSID;     // your network SSID (name)
char password[] = WIFI_PASS; // your network key

// Initialize Telegram BOT
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 500; //mean time between sccan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

const int ledPin = D0;
const int pirPin = D3;
int ledStatus = 0;

int PIRsensor = 0;

unsigned int count=0;

AdafruitIO_Feed *id = io.feed("fabdoorbotid","growolff");
AdafruitIO_Feed *counter = io.feed("fabdoorbotcount","growolff");

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String user_id = String(bot.messages[i].from_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    Serial.println(from_name);
    /*
      if (text == "/ledon") {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
      }

      if (text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
      }
    */
    if (text == "/open") {
      count++;
      digitalWrite(ledPin, HIGH);
      delay(1000);
      digitalWrite(ledPin,LOW);
      bot.sendMessage(user_id, "Puerta abierta :)" , "");
      id->save(user_id);
    }

    if (text == "/open@fabdoorbot") {
      count++;
      digitalWrite(ledPin, HIGH);
      delay(1000);
      digitalWrite(ledPin, LOW);
      bot.sendMessage(user_id, "Puerta abierta :)" , "");
      id->save(user_id);
    }
/*
    if (text == "/get") { // get PIR sensor status
      if (PIRsensor == 1) {
        bot.sendMessage(chat_id, "Sensor is active", "");
      }
      else if (PIRsensor == 0) {
        bot.sendMessage(chat_id, "Sensor is not active", "");
      }
    }
    if (text == "/status") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }
*/
    if (text == "/start") {
      String welcome = "Welcome to FabDoorBot interface, " + from_name + ".\n";
      welcome += "Using me you can: \n\n";
      welcome += "/open : to open the door\n";
      welcome += "/open@fabdoorbot : to open the door in group chats\n";
      welcome += "/status : Returns counter at night\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW); // initialize pin as off

  // connect to io.adafruit.com
  io.connect();
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
}

void loop() {
  io.run();
  //PIRsensor = digitalRead(pirPin);

  if (millis() > Bot_lasttime + Bot_mtbs)  {
    //Serial.println(PIRsensor);
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
