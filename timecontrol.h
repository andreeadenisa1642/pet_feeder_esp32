#pragma once
#include <Arduino.h>
#include <vector>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

struct FeedTime { uint8_t h; uint8_t m; };

extern std::vector<FeedTime> feedTimes;
extern bool autoEnabled;
extern NTPClient timeClient;

inline String trimStr(const String& s) {
  int start = 0; while(start < (int)s.length() && isspace(s[start])) start++;
  int end = s.length()-1; while(end>=0 && isspace(s[end])) end--;
  return s.substring(start, end+1);
}

inline FeedTime parseTimeString(const String& token) {
  int colon = token.indexOf(':');
  if(colon < 1) return {255,255};
  uint8_t h = token.substring(0, colon).toInt();
  uint8_t m = token.substring(colon+1).toInt();
  if(h >= 24 || m >= 60) return {255,255};
  return {h,m};
}

inline bool equals(const FeedTime& a, const FeedTime& b) {
  return a.h == b.h && a.m == b.m;
}

inline bool containsTime(const FeedTime& ft) {
  for(auto& t : feedTimes) if(equals(t, ft)) return true;
  return false;
}

inline bool eraseTime(const FeedTime& ft) {
  for(auto it = feedTimes.begin(); it != feedTimes.end(); ++it) {
    if(equals(*it, ft)) { feedTimes.erase(it); return true; }
  }
  return false;
}

inline void saveConfig() {
  StaticJsonDocument<256> doc;
  doc["auto"] = autoEnabled;
  JsonArray arr = doc.createNestedArray("times");
  for(auto& t : feedTimes) {
    char buf[6]; sprintf(buf, "%02d:%02d", t.h, t.m);
    arr.add(buf);
  }
  File f = SPIFFS.open("/config.json", FILE_WRITE);
  if(!f) return;
  serializeJson(doc, f);
  f.close();
}

inline void loadConfig() {
  if(!SPIFFS.begin(true)) return; 

  File f = SPIFFS.open("/config.json", FILE_READ);
  if(!f) {
    feedTimes = {{9,0},{13,0},{18,0}};  // ore predefinite
    autoEnabled = true;
    saveConfig();
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if(err) return;

  autoEnabled = doc["auto"] | true;
  feedTimes.clear();
  for(const char* s : doc["times"].as<JsonArray>()) {
    FeedTime ft = parseTimeString(String(s));
    if(ft.h != 255) feedTimes.push_back(ft);
  }
  if(feedTimes.empty()) feedTimes = {{9,0},{13,0},{18,0}};
}

inline bool checkAutoFeeding() {
  if(!autoEnabled) return false;

  timeClient.update();
  static uint32_t lastEpoch = 0;

  uint8_t h = timeClient.getHours();
  uint8_t m = timeClient.getMinutes();

  for(auto& t : feedTimes) {
    if(t.h == h && t.m == m) {
      uint32_t now = timeClient.getEpochTime();
      if(now - lastEpoch > 55) {
        lastEpoch = now;
        return true;
      }
    }
  }
  return false;
}
