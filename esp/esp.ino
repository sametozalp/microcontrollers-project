#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
//#include <FirebaseArduino.h>
#include <ESP8266Firebase.h>

#define WIFI_SSID "Samet"
#define WIFI_PASSWORD "Samet357910"
#define BOT_TOKEN "7063608304:AAEJL1BOLGbo2Ba1OophMXm_9yrveoYfXnU"

#define FIREBASE_HOST "uploadimageiot-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "gdrXOHlGgsxLdHKdOTeQGA0cQMIvY5TJDu1F2DCv"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

//Firebase firebase(FIREBASE_HOST);

const int ledPin = LED_BUILTIN;
int ledStatus = 0;
volatile int person_count = 0;

String chat = "";

String photo_url = "https://storage.googleapis.com/uploadimageiot.appspot.com/iot/880011a3-700d-4f82-a8f1-718023a80cce.png";
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

  //Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
//*************************************************************************************************************
void loop() {

  if (Serial.available() > 0) {
      person_count = Serial.parseInt();
  }
  
  person_count = random(0,3);
  Serial.print(person_count);
  
  //photo_url = firebase.getString("url");

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    //Serial.println("mesaj istegi geldi..");
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
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

    if (text == "/ledon") {
      digitalWrite(ledPin, LOW);
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    else if (text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, HIGH);
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    else if (text == "/status") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    else if (text == "/goruntu_al") {
      Serial.println("Goruntu gonderiliyor..");
      //photo_url = firebase.getString("url");
      //Serial.println(photo_url);
      bot.sendPhoto(chat_id, photo_url, "");
    }

    else if (text == "/start") {
      String welcome = "Hoşgeldin " + from_name + "!\n";
      welcome += "Bu bir kamera izleme sistemidir.\n";
      welcome += "Görüntüdeki kişi sayısını belirli zaman aralıklarıyla alabilir ve görüntüyü anlık çekebilirsin.\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    else if (text == "/kac_kisi") {

       String message = "Şu anda kamerada ";
        message += person_count;
        message += " kişi var..";
        bot.sendMessage(chat_id, message, "");
      /*
      if (Serial.available() > 0) {
        person_count = Serial.parseInt();
        //bot.sendMessage("812672293", ""+person_count, "");

       
      }
      */
    }
  }
}
