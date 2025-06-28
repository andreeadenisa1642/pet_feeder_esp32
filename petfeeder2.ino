#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Ticker.h>
#include <WiFiUdp.h>
#include <UniversalTelegramBot.h>
#include <NTPClient.h>
#include <vector>
#include <ESP32Servo.h>  

#include "config.h"
#include "sensors.h"
#include "control.h"
#include "timecontrol.h"
#include "telegram.h"

Ticker reservoirTicker;
bool reservoirAlertSent = false;

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

HX711 scale;
Servo dispenserServo;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200);   // +2 h Romania vara

std::vector<FeedTime> feedTimes;
bool autoEnabled = true;

//conectare WiFi
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wi-Fi");
  uint32_t t0 = millis();
  while(WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if(WiFi.status() == WL_CONNECTED) {
    Serial.print("Conectat, Adresa IP = ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Conexiunea Wi-Fi a eșuat.");
  }
}

// verficare rezervor
void checkReservoir() {
  float dist = readDistance();
  if(dist > RESERVOIR_EMPTY_CM && !reservoirAlertSent) {
    char msg[64];
    sprintf(msg, "Rezervor gol (%.1f cm)!⚠️", dist);
    bot.sendMessage(CHAT_ID, msg, "");
    reservoirAlertSent = true;
  }
  if(dist <= RESERVOIR_EMPTY_CM) reservoirAlertSent = false;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();
  client.setInsecure();

  initUltrasonic();  
  initScale();
  initServo();

  loadConfig();
  timeClient.begin();

  reservoirTicker.attach(60, checkReservoir);
  Serial.println("Pet Feeder pregătit!🐾");
}

unsigned long lastTelegramCheck = 0;

void loop() {
  if(checkAutoFeeding()) {
    serveFood();
    bot.sendMessage(CHAT_ID, "Porție automată servită!⏰", "");
  }

  if(millis() - lastTelegramCheck > 1000) {
    int newMsgs = bot.getUpdates(bot.last_message_received + 1);
    if(newMsgs) handleNewMessages(newMsgs);
    lastTelegramCheck = millis();
  }
}
