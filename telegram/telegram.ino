#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "Samet";
const char* password = "Samet3579";
const char* botToken = "7063608304:AAEJL1BOLGbo2Ba1OophMXm_9yrveoYfXnU";

// Telegram chat ID, bu alıcı kullanıcının chat ID'si olmalı
//int64_t chat_id = YOUR_CHAT_ID;

// Wi-Fi client
WiFiClientSecure client;

// Telegram bot nesnesi
UniversalTelegramBot bot(botToken, client);

void setup() {

  Serial.begin(9600);

  // Wi-Fi bağlantısı
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");

  // Telegram bot bağlantısı
  Serial.println("Bot is starting...");

  
}

void loop() {
  // Herhangi bir ek işlem yapmadan sürekli döngü
  bool a = bot.sendMessage("-1002054933903", "Arduino Botunuz başlatıldı!");
  Serial.println(a);
}
