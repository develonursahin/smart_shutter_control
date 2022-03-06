#define BLYNK_PRINT Serial
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define BOTtoken "1842571253:AAHAkN0hyEyvsLHxGUtTONCCs3EFgQSbtKA"  // BotFather'dan aldığınız TOKEN
#define CHAT_ID "943122067"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int botIstekGecikmesi = 1000;
unsigned long botSonCalismaZamani;

char auth[] = "zFmC_gZoSFgLpPlsUZ7A3nO1wxG3eqq6"; //Blynk Token.
char ssid[] = "SAHINEMLAK"; //Nodemcu nun bağlanacağı wifi ağının adı.
char pass[] = "s4h1N_3mL4k"; //Nodemcu nun bağlanacağı wifi ağının şifresi.
char password[] = "s4h1N_3mL4k";
unsigned long now = 0;
unsigned long before = 0;
int wait = 70000;

void stopper(){
  digitalWrite(D0,HIGH);
  digitalWrite(D1, HIGH);
  Blynk.notify("ŞAHİN EMLAK'IN KEPENKİ DURDURULDU");
  Blynk.virtualWrite(V0, "KEPENK DURDURULDU"); //Uygulamadaki kepenk durumu.
  Serial.println("KEPENK DURDURULDU"); //Seri Porttaki kepenk durumu.     
}

void counter1(){
    now = millis();
  if  (now - before >= wait)
  {
    before = now;
    stopper();
    Blynk.notify("ŞAHİN EMLAK'IN KEPENKİ AÇILDI");
    Blynk.virtualWrite(V0, "KEPENK AÇIK"); //Uygulamadaki kepenk durumu.
    Serial.println("KEPENK AÇIK"); //Seri Porttaki kepenk durumu.
  }
}

void counter2(){
    now = millis();
  if  (now - before >= wait)
  {
    before = now;
    stopper();
    Blynk.notify("ŞAHİN EMLAK'IN KEPENKİ KAPANDI");
    Blynk.virtualWrite(V0, "KEPENK KAPALI"); //Uygulamadaki kepenk durumu.
    Serial.println("KEPENK KAPALI"); //Seri Porttaki kepenk durumu. 
  }
}

// yeni bir mesaj geldiğinde ne yapılacağını belirleyen fonksiyon
void yeniMesajlariYonet(int kacYeniMesaj) {
  Serial.println("Gelen Yeni Mesaj");
  Serial.println(String(kacYeniMesaj));

  for (int i=0; i<kacYeniMesaj; i++) {
    // talebi yapan kullanıcının chatid'si
    String chat_id = String(bot.messages[i].chat_id);
    //eğer farklı bir kullanıcıdan komut gelirse izin vermeyeceğiz
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Yetkisiz Kullanıcı", "");
      continue;
    }
    
    // Alınan komut mesajını yazdıralım
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/baslat") {
      String merhaba = "Merhaba, " + from_name + ".\n";
      merhaba += "Kepenk'i kontrol etmek için alttaki komutları kullanabilirsin.\n\n";
      merhaba += "/kepenk_ac ile Kepenk Açılır \n";
      merhaba += "/kepenk_kapat ile Kepenk Kapatılır \n";
      merhaba += "/durdur ile Kepenk durdurulur \n";
      bot.sendMessage(chat_id, merhaba, "");
    }

    if (text == "/kepenk_ac") {
      bot.sendMessage(chat_id, "Kepenk Açıldı.", "");
              digitalWrite(D0, LOW); //Butonu aktif eder.
              digitalWrite(D1, HIGH); //Butonu pasif eder.
              Blynk.notify("ŞAHİN EMLAK'IN KEPENKİ AÇILIYOR");
              Blynk.virtualWrite(V0, "<< KEPENK AÇILIYOR >>"); //Uygulamadaki kepenk durumu.
              Serial.println("KEPENK AÇILIYOR"); //Seri Porttaki kepenk durumu.
              counter1();
    }
    
    if (text == "/kepenk_kapat") {
      bot.sendMessage(chat_id, "Kepenk Kapatıldı.", "");
              digitalWrite(D1, LOW); //Butonu aktif eder.
              digitalWrite(D0, HIGH); //Butonu pasif eder.
              Blynk.notify("ŞAHİN EMLAK'IN KEPENKİ KAPANIYOR");
              Blynk.virtualWrite(V0, "<< KEPENK KAPANIYOR >>"); //Uygulamadaki kepenk durumu.
              Serial.println("KEPENK KAPANIYOR"); //Seri Porttaki kepenk durumu.
              counter2();
    }
    
    if (text == "/durdur") {
      bot.sendMessage(chat_id, "Kepenk Durduruldu.", "");
      stopper();
      }
    }
  }


void setup()
{
  stopper();
 // Serial.begin(9600); //Seri Port ekranın bağlanması.
  Blynk.begin(auth, ssid, pass); //Blynk değişkenleri.
  pinMode(D0,OUTPUT); //Dijital Pin 0 - AÇ
  pinMode(D1,OUTPUT); //Dijital Pin 1 - KAPAT
  pinMode(D2,OUTPUT); //Dijital Pin 2 - DURDUR
  pinMode(V0,OUTPUT); //Sanal Pin 0 - KEPENK DURUMU

    Serial.begin(115200);
      #ifdef ESP8266 
    client.setInsecure();
  #endif
  Serial.println("Başlatılıyor");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Bağlantı hatası, yeniden başlıyor!!!");
    delay(5000); //5sn bekle
    ESP.restart();//sistemi yeniden başlat
  }

  //iletişim için gerekli port
  // standart olarak  8266 portu gelir
  // ArduinoOTA.setPort(8266); //komutu ile değiştirilebilir

  // Sunucu ismi olarak  esp8266-[ChipID] gelir
   ArduinoOTA.setHostname("Kepenk Kontrol"); //komutu ile değiştirilebilir

  // Standart olarak şifre sormaz ama sorması istenirse
  // ArduinoOTA.setPassword("admin"); //komutu ile şifre belirlenebilir

  // Aynı zamanda bu şifre MD5 kriptolama algoritması ile kriptolanabilir.
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

//////////////////////////////////////////////////////////////////////////////////////
//daha sonra yeniden OTA kod atabilmek için gerekli OTA tanımlamalarını başlatmalıyız
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOT: FS kullandıysanız FS.end()komutu ile dosyayı kapatmalısınız
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Hata[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Yetki Hatası");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Baslatma Basarisiz");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Baglanti Basarisiz");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Veri Alinamadi");
    else if (error == OTA_END_ERROR) Serial.println("Sonlandirma Basarisiz");
  });
  ArduinoOTA.begin();//OTA'yi başlatıyor
  Serial.println("Sistem Hazir");
  Serial.print("IP Adresiniz: ");
  Serial.println(WiFi.localIP());
} 



void switchstate(){
    if (digitalRead(D0)==LOW) //Dijital Pin 0'ın LOW olup olmadığı kontrol edilir.
    {
          if(digitalRead(D2)==LOW){
              digitalWrite(D1, HIGH); //Butonu pasif eder.
              Blynk.notify("ŞAHİN EMLAK'IN KEPENKİ AÇILIYOR");
              Blynk.virtualWrite(V0, "<< KEPENK AÇILIYOR >>"); //Uygulamadaki kepenk durumu.
              Serial.println("KEPENK AÇILIYOR"); //Seri Porttaki kepenk durumu.
              counter1();
            //myDelay();
            //delay(21000); //Butonun aktif olacağı süre.              
            //digitalWrite(D0, HIGH); //Butonu pasif eder.
          }
                   
          else if(digitalRead(D2)==HIGH){
                stopper();
          }
   }

   if (digitalRead(D1)==LOW) //Dijital Pin 1'in LOW olup olmadığı kontrol edilir.
   {   
       if(digitalRead(D2)==LOW){
              digitalWrite(D0, HIGH); //Butonu pasif eder.
              Blynk.notify("ŞAHİN EMLAK'IN KEPENKİ KAPANIYOR");
              Blynk.virtualWrite(V0, "<< KEPENK KAPANIYOR >>"); //Uygulamadaki kepenk durumu.
              Serial.println("KEPENK KAPANIYOR"); //Seri Porttaki kepenk durumu.
              counter2();
            //myDelay();
            //delay(21000); //Butonun aktif olacağı süre.              
            //digitalWrite(D1, HIGH); //Butonu pasif eder.
        }
              
        else if(digitalRead(D2)==HIGH){
                stopper();
        }            
   }
  }


void loop()
{ 
  ArduinoOTA.handle();//sonraki kod atma işlemleri için kullanmamız gerekiyor
    if (millis() > botSonCalismaZamani + botIstekGecikmesi)  {
    int kacYeniMesaj = bot.getUpdates(bot.last_message_received + 1);

    while(kacYeniMesaj) { //yeni mesaj gelmişse yazdıralım
      Serial.println("mesaj alındı");
      yeniMesajlariYonet(kacYeniMesaj);
      kacYeniMesaj = bot.getUpdates(bot.last_message_received + 1);
    }
    botSonCalismaZamani = millis();
  }
  Blynk.run();
  switchstate();
}
