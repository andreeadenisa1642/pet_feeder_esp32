#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include "timecontrol.h"
#include "sensors.h"
#include "control.h"

extern WiFiClientSecure    client;
extern UniversalTelegramBot bot;
extern std::vector<FeedTime> feedTimes;
extern bool  autoEnabled;

inline void listHours(const String& chat_id) {
  String msg = autoEnabled ? "🟢 Hrănire automată ON\n" 
                           : "🔴 Hrănire automată OFF\n";
  for(auto& t : feedTimes) {
    char buf[6]; sprintf(buf, "%02d:%02d", t.h, t.m);
    msg += String(buf) + '\n';
  }
  bot.sendMessage(chat_id, msg, "");
}

inline void handleNewMessages(int numNewMessages) {
  for(int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text    = trimStr(bot.messages[i].text);

    if(text == "/start") {
      String help =
        "Comenzi disponibile:\n"
        "/feed - hrănire manuală\n"
        "/list - listează orarul\n"
        "/set HH:MM,... - setează lista\n"
        "/add HH:MM - adaugă oră\n"
        "/del HH:MM - șterge oră\n"
        "/autoon /autooff - pornește/oprește mod auto\n"
        "/status - distanță & greutate";
      bot.sendMessage(chat_id, help, "");
    }
    else if(text == "/feed") {
      serveFood();
      bot.sendMessage(chat_id, "Porție de mâncare a fost servită!🍽️", "");
    }
    else if(text == "/status") {
      float dist = readDistance();
      float w    = readWeight();
      char buf[64];
      sprintf(buf, "Nivel rezervor: %.1f cm\nGreutate bol: %.1f g", dist, w);
      bot.sendMessage(chat_id, buf, "");
    }
    else if(text == "/list") {
      listHours(chat_id);
    }
    else if(text.startsWith("/set ")) {
      String list = text.substring(5);
      list.replace(" ", "");
      feedTimes.clear();
      int idx = 0;
      while(idx < list.length()) {
        int comma = list.indexOf(',', idx);
        String token = comma == -1 ? list.substring(idx) : list.substring(idx, comma);
        FeedTime ft = parseTimeString(token);
        if(ft.h != 255 && !containsTime(ft)) feedTimes.push_back(ft);
        if(comma == -1) break;
        idx = comma + 1;
      }
      saveConfig();
      bot.sendMessage(chat_id, "Orele au fost actualizate.⏰", "");
    }
    else if(text.startsWith("/add ")) {
      FeedTime ft = parseTimeString(text.substring(5));
      if(ft.h == 255) bot.sendMessage(chat_id, "Oră invalidă.", "");
      else if(containsTime(ft)) bot.sendMessage(chat_id, "Duplicat.", "");
      else if(feedTimes.size() >= 10) bot.sendMessage(chat_id, "Listă plină.(max 10)", "");
      else {
        feedTimes.push_back(ft);
        saveConfig();
        bot.sendMessage(chat_id, "Oră adăugată.", "");
      }
    }
    else if(text.startsWith("/del ")) {
      FeedTime ft = parseTimeString(text.substring(5));
      if(eraseTime(ft)) {
        saveConfig();
        bot.sendMessage(chat_id, "Oră ștearsă.", "");
      } else bot.sendMessage(chat_id, "Oră inexistentă.", "");
    }
    else if(text == "/autoon") {
      autoEnabled = true;
      saveConfig();
      bot.sendMessage(chat_id, "Hrănire automată pornită ✅", "");
    }
    else if(text == "/autooff") {
      autoEnabled = false;
      saveConfig();
      bot.sendMessage(chat_id, "Hrănire automată oprită ⛔", "");
    }
  }
}