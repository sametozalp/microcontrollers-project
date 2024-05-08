#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "Samet"
#define WIFI_PASSWORD "Samet357910"
#define BOT_TOKEN "7063608304:AAEJL1BOLGbo2Ba1OophMXm_9yrveoYfXnU"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

const int ledPin = LED_BUILTIN;
int ledStatus = 0;

String test_photo_url = "https://firebasestorage.googleapis.com/v0/b/uploadimageiot.appspot.com/o/iot%2Fcap.png?alt=media&token=42f500df-5cc1-4738-9210-bb22f6468f1f";

int person = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("a");
  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert);
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("b");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  Serial.println("c");

  }

  Serial.println("Baglandi..");
  // LED PIN
  for (int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  Serial.println("selam");
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  Serial.println("sela");
  while (numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
  Serial.println("sel");

  person = random(14);
  Serial.println(person);

  if (person != 0) {
    for (int i = 2; i < person + 2; i++) {
      digitalWrite(i, HIGH);
    }
  }

  delay(1000);

  close_leds();
}

void close_leds() {
  for (int i = 2; i < 14; i++) {
    digitalWrite(i, LOW);
  }
}


void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/ledon") {
      digitalWrite(ledPin, LOW);
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, HIGH);
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if (text == "/goruntu_al") {
      bot.sendPhoto(chat_id, test_photo_url, "");
    }

    if (text == "/start") {
      String welcome = "Hoşgeldin " + from_name + "!\n";
      welcome += "/Bu bir kamera izleme sistemidir.\n";
      welcome += "Görüntüdeki kişi sayısını belirli zaman aralıklarıyla alabilir ve görüntüyü anlık çekebilirsin.\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}
