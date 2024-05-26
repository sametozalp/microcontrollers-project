#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP8266Firebase.h>

#define WIFI_SSID "Samet"
#define WIFI_PASSWORD "Samet357910"
#define BOT_TOKEN "7063608304:AAEJL1BOLGbo2Ba1OophMXm_9yrveoYfXnU"

#define FIREBASE_HOST "uploadimageiot-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "gdrXOHlGgsxLdHKdOTeQGA0cQMIvY5TJDu1F2DCv"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

Firebase firebase(FIREBASE_HOST);

const int ledPin = LED_BUILTIN;
int ledStatus = 0;
volatile int person_count = 0;
const unsigned long BOT_MTBS = 1000;
unsigned long bot_lasttime;
String chat = "";

volatile bool fire = false; // görüntü alma istegi gelince true olur
String photo_url = "";
//*************************************************************************************************************
void setup() {

  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  delay(100);
  //digitalWrite(ledPin, OUTPUT);

  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert);
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Baglandi..");
}
//*************************************************************************************************************
void loop() {

  if (Serial.available() > 0) {
    person_count = Serial.parseInt();
    person_count = person_count % 10;
  }

  if (fire == true) {
    photo_url = firebase.getString("url");

    Serial.println(photo_url);

    if (photo_url.length() != 0) {
      bot.sendPhoto(chat, photo_url, "");
    } else {
      bot.sendMessage(chat, "Görüntü bulunamadı..", "");
    }

    fire = false;
  }

  //person_count = random(0,3);
  Serial.print(person_count);

  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.println(numNewMessages);
    while (numNewMessages) {
      //Serial.println("mesaj istegi geldi..");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }

  delay(500);
}

//*************************************************************************************************************
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    chat = chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    
    if (text == "/led_yak") {
      digitalWrite(ledPin, LOW);
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led yandı", "");
    }

    else if (text == "/led_sondur") {
      ledStatus = 0;
      digitalWrite(ledPin, HIGH);
      bot.sendMessage(chat_id, "Led söndü", "");
    }

    else if (text == "/led_durum") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Led yanıyor", "");
      } else {
        bot.sendMessage(chat_id, "Led sönük", "");
      }
    }

    else if (text == "/goruntu_al") {
      fire = true;
    }

    else if (text == "/start") {
      String welcome = "Hoşgeldin " + from_name + "!\n";
      welcome += "Bu bir kamera izleme sistemidir.\n";
      welcome += "Görüntüdeki kişi sayısını belirli zaman aralıklarıyla alabilir ve görüntüyü anlık çekebilirsin.\n\n";
      welcome += "Aşağıdaki komutları kullanarak sistemle iletişim kurabilirsin..\n";
      welcome += "/led_yak - esp8266 üzerindeki ledi yak\n";
      welcome += "/led_sondur - esp8266 üzerindeki ledi söndür\n";
      welcome += "/led_durum - esp8266 üzerindeki ledin durumunu öğren\n";
      welcome += "/kac_kisi - kamerada kaç kişi var ?\n";
      welcome += "/goruntu_al - kamera görüntüsünü gönder\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    else if (text == "/kac_kisi") {

      String message = "Şu anda kamerada ";
      message += person_count;
      message += " kişi var..";
      bot.sendMessage(chat_id, message, "");
    }
  }
}
