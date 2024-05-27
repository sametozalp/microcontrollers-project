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

const char* web_adresi = "api.thingspeak.com";    // baglanilacak web sunucu adresi veya IP adresi
const uint16_t port = 80;           // baglanilacak sunucu portu

String uzanti = "/channels/2561421/feeds.json?api_key=MQW98XSY33FOPU72&results=2";              // baglanilacak olan web sayfasinin uzantisi

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
      //fire = true;
      WiFiClient istemci;       // istemci nesnesini oluştur

      if (!istemci.connect(web_adresi, port)) {
        //Serial.println("Sunucuya baglanma hatasi");
        delay(5000);
        return;
      }
      //Serial.println("WEB sunucusuna baglandi");
      delay(2000);

      //--------------- sunucudan sayfa isteme -----------------
      String http_req = "";
      http_req += "GET ";
      http_req += uzanti;
      http_req += " HTTP/1.1\r\n";
      http_req += "Host: ";
      http_req += web_adresi;
      http_req += "\r\n";
      http_req += "Connection: close\r\n";
      istemci.println(http_req);
      //------------------ isteme sonu -----------------------

      //-------- 5 sn lik zaman döngüsü kur ve istek geldi mi diye kontrol et ------
      // çalışma zamanı milisaniye cinsinden tutan millis() fonksiyonundan süre al, başlangıc zamanı
      unsigned long onceki_zaman = millis();
      while (istemci.available() == 0) {
        if (millis() - onceki_zaman > 5000) {
          Serial.println("Baglanti hatasi");
          istemci.stop();
          delay(2000);
          return;
        }
      }
      //-------------- veri gelmis satır satır oku ekrana yazdır --------------------
      while (istemci.available())
      {
        String satir = istemci.readStringUntil('\r');
        int index_numarasi = satir.indexOf("last_entry_id");  // aranan String buraya yazılır
        if (index_numarasi != -1)           // aranan string satir degiskeni icerisinde varsa if içine girer
        {
          String last_entry_id = satir.substring(index_numarasi + 15);
          String final_last_entry_id = last_entry_id.substring(0, last_entry_id.indexOf("}"));
          int feeds_index = last_entry_id.indexOf("entry_id\":" + final_last_entry_id);
          String value = last_entry_id.substring(feeds_index + 9);
          String ilk_tirnaktan_itibaren = value.substring(value.indexOf("\"") + 10);
          int son_tirnak_index = ilk_tirnaktan_itibaren.indexOf("\"");
          String istenen = ilk_tirnaktan_itibaren.substring(0, son_tirnak_index);
          bot.sendPhoto(chat, istenen, "");

        }
        //Serial.println(satir);
      }
      //Serial.println("-------------------------");
      istemci.stop();
      delay(2000);
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

void url_al() {
  WiFiClient istemci;       // istemci nesnesini oluştur

  if (!istemci.connect(web_adresi, port)) {
    //Serial.println("Sunucuya baglanma hatasi");
    delay(5000);
    return;
  }
  //Serial.println("WEB sunucusuna baglandi");
  delay(2000);

  //--------------- sunucudan sayfa isteme -----------------
  String http_req = "";
  http_req += "GET ";
  http_req += uzanti;
  http_req += " HTTP/1.1\r\n";
  http_req += "Host: ";
  http_req += web_adresi;
  http_req += "\r\n";
  http_req += "Connection: close\r\n";
  istemci.println(http_req);
  //------------------ isteme sonu -----------------------

  //-------- 5 sn lik zaman döngüsü kur ve istek geldi mi diye kontrol et ------
  // çalışma zamanı milisaniye cinsinden tutan millis() fonksiyonundan süre al, başlangıc zamanı
  unsigned long onceki_zaman = millis();
  while (istemci.available() == 0) {
    if (millis() - onceki_zaman > 5000) {
      Serial.println("Baglanti hatasi");
      istemci.stop();
      delay(2000);
      return;
    }
  }
  //-------------- veri gelmis satır satır oku ekrana yazdır --------------------
  while (istemci.available())
  {
    String satir = istemci.readStringUntil('\r');
    int index_numarasi = satir.indexOf("last_entry_id");  // aranan String buraya yazılır
    if (index_numarasi != -1)           // aranan string satir degiskeni icerisinde varsa if içine girer
    {
      String last_entry_id = satir.substring(index_numarasi + 15);
      String final_last_entry_id = last_entry_id.substring(0, last_entry_id.indexOf("}"));
      int feeds_index = last_entry_id.indexOf("entry_id\":" + final_last_entry_id);
      String value = last_entry_id.substring(feeds_index + 9);
      String ilk_tirnaktan_itibaren = value.substring(value.indexOf("\"") + 10);
      int son_tirnak_index = ilk_tirnaktan_itibaren.indexOf("\"");
      String istenen = ilk_tirnaktan_itibaren.substring(0, son_tirnak_index);
      Serial.println("nerdeyiz: ");
      Serial.println(istenen);

    }
    //Serial.println(satir);
  }
  //Serial.println("-------------------------");
  istemci.stop();
  delay(2000);
}
