/**
 * @file main.cpp
 * @brief Main Entry Point for Gadget V1 Modular OS.
 * 
 * This firmware implements a non-blocking state machine architecture to 
 * manage multiple hardware modules (Sensors, IR, RF Analyzer, WiFi Repeater).
 * 
 * @author Wayan
 * @version 2.0
 */

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "DisplayManager.h"
#include "OSDManager.h"
#include "SensorManager.h"
#include "IRManager.h"
#include "ButtonManager.h"
#include "RFAnalyzerManager.h"
#include "WiFiRepeaterManager.h"
#include "AppManager.h"
#include "EventManager.h"
#include "TaskScheduler.h"
#include "SafeMode.h"
#include "SystemInfo.h"
#include "GlobalLogger.h"
#include "FileExplorer.h"
#include "BootManager.h"
#include "EyeAnimation.h"
// OSD app registered locally below to avoid linker ordering issues
#include <LittleFS.h>

/**
 * @enum SystemState
 * @brief Defines the global execution state of the device.
 * 
 * WHY: We use a global state machine to ensure only one heavy module (e.g., WiFi Repeater)
 * is active at a time, preventing memory overflow and hardware resource contention on the 
 * single-core ESP8266.
 */
enum SystemState {
    STATE_BOOT,
    STATE_NORMAL,
    STATE_MAIN_MENU,
    STATE_RULER,
    STATE_IR_MENU,
    STATE_IR_CLONE,
    STATE_IR_BRUTEFORCE,
    STATE_IR_BRUTEFORCE_FOUND,
    STATE_IR_MANAGE,
    STATE_RF_MENU,
    STATE_RF_LIVE,
    STATE_RF_DEAUTH,
    STATE_RF_BEACON,
    STATE_RF_SPIKE,
    STATE_RF_STATS,
    STATE_RF_HEATMAP,
    STATE_RF_RADAR,
    STATE_RF_LOGGER,
    STATE_RF_LOG_VIEWER,
    STATE_REPEATER_MENU,
    STATE_REPEATER_SCAN,
    STATE_REPEATER_SSID_LIST,
    STATE_REPEATER_KEYBOARD,
    STATE_REPEATER_CONNECTING,
    STATE_REPEATER_ACTIVE,
    STATE_REPEATER_SAVED_NETS,
    STATE_REPEATER_DEL_CONFIRM,
    STATE_REPEATER_AP_CONFIG,
    STATE_REPEATER_AP_KB,
    STATE_OSD_MENU,
    STATE_OSD_CUSTOM_TEXT,
    STATE_OSD_RUNNING_TEXT,
    STATE_OSD_EYE_ANIMATION,
    STATE_SETTINGS_MENU,
    STATE_SETTINGS_SCREEN_SLEEP
};

SystemState currentState = STATE_BOOT;
WiFiRepeaterManager repeaterManager;

unsigned long lastDisplayUpdate = 0;
unsigned long lastInteractionTime = 0;
int menuIndex = 0;
int rulerOffset = 0;

int irMenuIndex = 0;
int rfMenuIndex = 0;
int bfIndex = 0;
int bfTotal = 0;
int fileIndex = 0;
int fileTotal = 0;
String currentFileName = "";

// Repeater Variables
int repMenuIndex = 0;
int repWifiIndex = 0;
int repWifiTotal = 0;
String repKeyboardText = "";
int repKx = 0;
int repKy = 0;
unsigned long repConnectStartTime = 0;
String repSelectedSSID = "";
int repSavedNetIndex = 0;
bool repDelConfirmYes = false;
int repApConfigIndex = 0;
int repApKbTarget = 0;

bool displaySleeping = false;

static const int MAIN_MENU_ITEM_COUNT = 7;
static const AppId_t APP_OSD_MENU = 1;
static const AppId_t APP_OSD_RUNNING_TEXT = 2;
static const AppId_t APP_OSD_EYE_ANIMATION = 3;
static EyeAnimation* eyeAnimation = nullptr;

static bool shouldAllowSleep(SystemState state) {
    switch (state) {
        case STATE_NORMAL:
        case STATE_MAIN_MENU:
        case STATE_IR_MENU:
        case STATE_RF_MENU:
        case STATE_REPEATER_MENU:
        case STATE_REPEATER_SAVED_NETS:
        case STATE_REPEATER_DEL_CONFIRM:
        case STATE_OSD_MENU:
        case STATE_OSD_RUNNING_TEXT:
        case STATE_OSD_CUSTOM_TEXT:
        case STATE_OSD_EYE_ANIMATION:
        case STATE_SETTINGS_MENU:
        case STATE_SETTINGS_SCREEN_SLEEP:
            return true;
        default:
            return false;
    }
}

static void wakeDisplay() {
    if (displaySleeping) {
        displayManager.setDisplaySleep(false);
        displaySleeping = false;
    }
}

static void touchActivity() {
    lastInteractionTime = millis();
    wakeDisplay();
}

static void updateDisplaySleepState() {
    if (!osdManager.isSleepEnabled() || !shouldAllowSleep(currentState)) {
        wakeDisplay();
        return;
    }

    if (millis() - lastInteractionTime >= osdManager.getSleepTimeoutMs()) {
        if (!displaySleeping) {
            displayManager.setDisplaySleep(true);
            displaySleeping = true;
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nMemulai Sistem V1 Modular...");

    // WHY: LittleFS is used for persistent configs and IR code storage.
    if(!LittleFS.begin()){
        Serial.println("LittleFS Mount Failed");
    }

    // Initialize core managers
    eventManager.begin();
    taskScheduler.begin();
    globalLogger.begin();
    fileExplorer.begin();
    systemInfo.begin();
    appManager.begin();
    // Register core apps
    static const AppInterface osdAppLocal = {
        .id = APP_OSD_MENU,
        .name = "Onscreen Display",
        .onEnter = [](){ osdManager.onEnterMenu(); },
        .update = [](){ osdManager.update(); },
        .render = [](){ osdManager.render(); },
        .onExit = [](){ osdManager.onExit(); }
    };
    appManager.registerApp(&osdAppLocal);
    static const AppInterface osdRunningTextApp = {
        .id = APP_OSD_RUNNING_TEXT,
        .name = "Running Text",
        .onEnter = [](){ osdManager.onEnterRunningText(); },
        .update = [](){ osdManager.update(); },
        .render = [](){ osdManager.render(); },
        .onExit = [](){ osdManager.onExitRunningText(); }
    };
    appManager.registerApp(&osdRunningTextApp);
    static const AppInterface osdEyeAnimationApp = {
        .id = APP_OSD_EYE_ANIMATION,
        .name = "Eye Animation",
        .onEnter = [](){ if (eyeAnimation) eyeAnimation->setExpression(EyeAnimation::Idle); osdManager.onEnterEyeAnimation(); },
        .update = [](){ if (eyeAnimation) eyeAnimation->update(); },
        .render = [](){ if (eyeAnimation) eyeAnimation->render(); },
        .onExit = [](){ osdManager.onExit(); }
    };
    appManager.registerApp(&osdEyeAnimationApp);

    osdManager.begin();
    eyeAnimation = new EyeAnimation(displayManager.getDisplay());
    if (eyeAnimation) {
        eyeAnimation->begin();
        eyeAnimation->setExpression(EyeAnimation::Idle);
    }

    // WHY: Initialize I2C bus at 400kHz for optimal OLED and sensor throughput.
    Wire.begin(); 
    Wire.setClock(400000);
    // Inisialisasi Modul
    buttonManager.init();

    // Safe mode detection (button held at boot)
    safeMode.begin();

    bootManager.begin();
    bootManager.runChecks();

    rfManager.init();
    
    if(!displayManager.init()) {
        Serial.println("Gagal menemukan layar OLED SSD1306");
        while(1) delay(10);
    }

    displayManager.setDisplaySleep(false);
    lastInteractionTime = millis();

    if(!sensorManager.initBME()) {
        Serial.println("Gagal menemukan BME280!");
        delay(500);
    }

    if(!sensorManager.initToF()) {
        Serial.println("Gagal menemukan VL53L0X!");
        delay(500);
    }

    irManager.init();
    repeaterManager.begin();

    Serial.println("Semua Sensor Berhasil Dimuat!");
}

void loop() {
    bool anyButtonPressed = buttonManager.isUpPressed() || buttonManager.isDownPressed() || buttonManager.isLeftPressed() || buttonManager.isRightPressed() || buttonManager.isOKPressed();
    if (anyButtonPressed) {
        touchActivity();
    }

    updateDisplaySleepState();

    // Core tickers
    taskScheduler.update();
    eventManager.process();
    appManager.update();
    if (currentState == STATE_OSD_RUNNING_TEXT && appManager.getActiveApp()) {
        if (millis() - lastDisplayUpdate > 66) {
            lastDisplayUpdate = millis();
            appManager.render();
        }
    }

    // Serial debug: press 'b' to dump BME readings directly
    if (Serial.available()) {
        int c = Serial.read();
        if (c == 'b' || c == 'B') {
            sensorManager.debugDump();
        }
    }

    // WHY: We use a global switch-case for State Management.
    // This pattern ensures only one module's complex logic runs at a time,
    // which is critical for the single-core ESP8266 performance.
    switch (currentState) {
        case STATE_BOOT:
            // Tampilkan animasi booting
            displayManager.showBootAnimation();
            currentState = STATE_NORMAL;
            break;

        case STATE_NORMAL:
            irManager.update();

            // Transisi ke Menu Utama jika tombol Kanan ditekan
            if (buttonManager.isRightJustPressed()) {
                currentState = STATE_MAIN_MENU;
                menuIndex = 0;
                displayManager.drawMainMenu(menuIndex); // Draw segera
                touchActivity();
                break;
            }

            if (buttonManager.isOKJustPressed()) {
                if (irManager.hasValidSignal()) {
                    displayManager.showSending();
                    irManager.transmitLastCode();
                    delay(500); 
                }
            }

            if (millis() - lastDisplayUpdate >= 500) {
                lastDisplayUpdate = millis();
                sensorManager.update();
                displayManager.drawDashboard(
                    sensorManager.getTemp(),
                    sensorManager.getHum(),
                    sensorManager.getPres(),
                    sensorManager.getAltitude(),
                    sensorManager.getBatteryPercent()
                );
            }
            break;

        case STATE_MAIN_MENU:
            if (buttonManager.isUpJustPressed()) {
                menuIndex--;
                if(menuIndex < 0) menuIndex = MAIN_MENU_ITEM_COUNT - 1;
                displayManager.drawMainMenu(menuIndex);
                touchActivity();
            }
            if (buttonManager.isDownJustPressed()) {
                menuIndex++;
                if(menuIndex > MAIN_MENU_ITEM_COUNT - 1) menuIndex = 0;
                displayManager.drawMainMenu(menuIndex);
                touchActivity();
            }
            if (buttonManager.isLeftJustPressed()) {
                // Kembali ke Dashboard
                currentState = STATE_NORMAL;
                displayManager.drawDashboard(
                    sensorManager.getTemp(),
                    sensorManager.getHum(),
                    sensorManager.getPres(),
                    sensorManager.getAltitude(),
                    sensorManager.getBatteryPercent()
                );
                touchActivity();
            }
            if (buttonManager.isOKJustPressed() || buttonManager.isRightJustPressed()) {
                if (menuIndex == 0) {
                    currentState = STATE_NORMAL;
                    displayManager.drawDashboard(
                        sensorManager.getTemp(),
                        sensorManager.getHum(),
                        sensorManager.getPres(),
                        sensorManager.getAltitude(),
                        sensorManager.getBatteryPercent()
                    );
                } else if (menuIndex == 1) {
                    currentState = STATE_RULER;
                    rulerOffset = 0; // Reset offset tiap masuk
                } else if (menuIndex == 2) {
                    currentState = STATE_IR_MENU;
                    irMenuIndex = 0;
                    displayManager.drawIRMenu(irMenuIndex);
                } else if (menuIndex == 3) {
                    currentState = STATE_RF_MENU;
                    rfMenuIndex = 0;
                    displayManager.drawRFMenu(rfMenuIndex);
                } else if (menuIndex == 4) {
                    currentState = STATE_REPEATER_MENU;
                    repMenuIndex = 0;
                    displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
                } else if (menuIndex == 5) {
                    currentState = STATE_OSD_MENU;
                    osdManager.onEnterMenu();
                    displayManager.drawOSDMenu(osdManager.getMenuIndex());
                } else if (menuIndex == 6) {
                    currentState = STATE_SETTINGS_MENU;
                    osdManager.onEnterSettingsMenu();
                    displayManager.drawSettingsMenu(0);
                }
                touchActivity();
            }
            break;

        case STATE_RULER:
            // Keluar ke Menu
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_MAIN_MENU;
                displayManager.drawMainMenu(menuIndex);
                break;
            }

            // Zeroing/Kalibrasi
            if (buttonManager.isOKJustPressed()) {
                rulerOffset = sensorManager.getRange();
            }

            if (millis() - lastDisplayUpdate >= 100) { // Update lebih cepat untuk penggaris
                lastDisplayUpdate = millis();
                sensorManager.update(); // Memicu pembacaan ToF & BME

                int realRange = sensorManager.getRange() - rulerOffset;
                displayManager.drawRuler(realRange, sensorManager.isRangeValid());
            }
            break;

        case STATE_IR_MENU:
            if (buttonManager.isUpJustPressed()) {
                irMenuIndex--;
                if(irMenuIndex < 0) irMenuIndex = 2;
                displayManager.drawIRMenu(irMenuIndex);
            }
            if (buttonManager.isDownJustPressed()) {
                irMenuIndex++;
                if(irMenuIndex > 2) irMenuIndex = 0;
                displayManager.drawIRMenu(irMenuIndex);
            }
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_MAIN_MENU;
                displayManager.drawMainMenu(menuIndex);
            }
            if (buttonManager.isOKJustPressed() || buttonManager.isRightJustPressed()) {
                if(irMenuIndex == 0) {
                    currentState = STATE_IR_CLONE;
                    irManager.update(); // Clear buffer
                    displayManager.drawIRCloneWaiting(irManager.getLastCode());
                } else if(irMenuIndex == 1) {
                    currentState = STATE_IR_BRUTEFORCE;
                    bfIndex = 0;
                    bfTotal = commonCodesSize + irManager.getSavedFilesCount();
                    displayManager.drawBruteforceProgress(bfIndex + 1, bfTotal, "Starting...");
                    lastDisplayUpdate = millis();
                } else if(irMenuIndex == 2) {
                    currentState = STATE_IR_MANAGE;
                    fileIndex = 0;
                    fileTotal = irManager.getSavedFilesCount();
                    currentFileName = irManager.getFileName(fileIndex);
                    displayManager.drawFileManager(currentFileName, fileIndex, fileTotal);
                }
            }
            break;

        case STATE_IR_CLONE:
            irManager.update();
            if(irManager.hasValidSignal()) {
                displayManager.drawIRCloneWaiting(irManager.getLastCode());
            }
            if(buttonManager.isLeftJustPressed()) {
                currentState = STATE_IR_MENU;
                displayManager.drawIRMenu(irMenuIndex);
            }
            if(buttonManager.isOKJustPressed()) {
                if(irManager.hasValidSignal()) {
                    String fname = "IR_" + String(millis());
                    irManager.saveToFile(fname);
                    displayManager.showMessage("Saved " + fname);
                    delay(1000);
                    currentState = STATE_IR_MENU;
                    displayManager.drawIRMenu(irMenuIndex);
                }
            }
            break;

        case STATE_IR_BRUTEFORCE:
            if(buttonManager.isLeftJustPressed()) {
                currentState = STATE_IR_MENU;
                displayManager.drawIRMenu(irMenuIndex);
                break;
            }
            if(buttonManager.isOKJustPressed()) {
                // TV merespon! Pindah ke mode evaluasi
                currentState = STATE_IR_BRUTEFORCE_FOUND;
                String cName = (bfIndex < commonCodesSize) ? commonCodes[bfIndex].name : irManager.getFileName(bfIndex - commonCodesSize);
                displayManager.showMessage("Isolate:\n" + cName);
                break;
            }

            if(millis() - lastDisplayUpdate > 1500) {
                lastDisplayUpdate = millis();
                if(bfIndex < bfTotal) {
                    String cName = (bfIndex < commonCodesSize) ? commonCodes[bfIndex].name : irManager.getFileName(bfIndex - commonCodesSize);
                    displayManager.drawBruteforceProgress(bfIndex + 1, bfTotal, cName);
                    irManager.sendBruteforceCode(bfIndex);
                    bfIndex++;
                } else {
                    displayManager.showMessage("Done!");
                    delay(1500);
                    currentState = STATE_IR_MENU;
                    displayManager.drawIRMenu(irMenuIndex);
                }
            }
            break;

        case STATE_IR_BRUTEFORCE_FOUND:
            if(buttonManager.isLeftJustPressed()) {
                currentState = STATE_IR_MENU;
                displayManager.drawIRMenu(irMenuIndex);
            }
            if(buttonManager.isUpJustPressed() && bfIndex > 1) {
                bfIndex--;
                String cName = (bfIndex-1 < commonCodesSize) ? commonCodes[bfIndex-1].name : irManager.getFileName((bfIndex-1) - commonCodesSize);
                displayManager.showMessage("Test: " + cName);
                irManager.sendBruteforceCode(bfIndex-1);
            }
            if(buttonManager.isDownJustPressed() && bfIndex < bfTotal) {
                bfIndex++;
                String cName = (bfIndex-1 < commonCodesSize) ? commonCodes[bfIndex-1].name : irManager.getFileName((bfIndex-1) - commonCodesSize);
                displayManager.showMessage("Test: " + cName);
                irManager.sendBruteforceCode(bfIndex-1);
            }
            if(buttonManager.isOKJustPressed()) {
                displayManager.showMessage("Got the Code!");
                delay(1000);
                currentState = STATE_IR_MENU;
                displayManager.drawIRMenu(irMenuIndex);
            }
            break;

        case STATE_IR_MANAGE:
            if(buttonManager.isLeftJustPressed()) {
                currentState = STATE_IR_MENU;
                displayManager.drawIRMenu(irMenuIndex);
            }
            if(buttonManager.isRightJustPressed()) { // Next File
                fileIndex++;
                if(fileIndex >= fileTotal) fileIndex = 0;
                currentFileName = irManager.getFileName(fileIndex);
                displayManager.drawFileManager(currentFileName, fileIndex, fileTotal);
            }
            if(buttonManager.isUpJustPressed()) { // Delete
                if(fileTotal > 0) {
                    irManager.deleteFile(currentFileName);
                    displayManager.showMessage("Deleted!");
                    delay(800);
                    fileTotal = irManager.getSavedFilesCount();
                    fileIndex = 0;
                    currentFileName = irManager.getFileName(fileIndex);
                    displayManager.drawFileManager(currentFileName, fileIndex, fileTotal);
                }
            }
            if(buttonManager.isOKJustPressed()) { // Send
                if(fileTotal > 0) {
                    displayManager.showSending();
                    irManager.loadAndSend(currentFileName);
                    delay(500);
                    displayManager.drawFileManager(currentFileName, fileIndex, fileTotal);
                }
            }
            break;

        case STATE_RF_MENU:
            if (buttonManager.isUpJustPressed()) {
                rfMenuIndex--;
                if(rfMenuIndex < 0) rfMenuIndex = 8;
                displayManager.drawRFMenu(rfMenuIndex);
            }
            if (buttonManager.isDownJustPressed()) {
                rfMenuIndex++;
                if(rfMenuIndex > 8) rfMenuIndex = 0;
                displayManager.drawRFMenu(rfMenuIndex);
            }
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_MAIN_MENU;
                displayManager.drawMainMenu(menuIndex);
            }
            if (buttonManager.isOKJustPressed() || buttonManager.isRightJustPressed()) {
                // Switch to specific RF State
                switch(rfMenuIndex) {
                    case 0: currentState = STATE_RF_LIVE; rfManager.start(ANALYZER_MODE_LIVE); break;
                    case 1: currentState = STATE_RF_DEAUTH; rfManager.start(ANALYZER_MODE_DEAUTH); break;
                    case 2: currentState = STATE_RF_BEACON; rfManager.start(ANALYZER_MODE_BEACON); break;
                    case 3: currentState = STATE_RF_SPIKE; rfManager.start(ANALYZER_MODE_SPIKE); break;
                    case 4: currentState = STATE_RF_STATS; rfManager.start(ANALYZER_MODE_STATS); break;
                    case 5: currentState = STATE_RF_HEATMAP; rfManager.start(ANALYZER_MODE_HEATMAP); break;
                    case 6: currentState = STATE_RF_RADAR; rfManager.start(ANALYZER_MODE_RADAR); break;
                    // For Logger and Log Viewer we don't start the sniffer immediately
                    case 7: currentState = STATE_RF_LOGGER; break; 
                    case 8: currentState = STATE_RF_LOG_VIEWER; break; 
                }
            }
            break;

        case STATE_RF_LIVE:
        case STATE_RF_DEAUTH:
        case STATE_RF_BEACON:
        case STATE_RF_SPIKE:
        case STATE_RF_STATS:
        case STATE_RF_HEATMAP:
        case STATE_RF_RADAR:
            rfManager.update();
            
            // Exit back to RF Menu
            if (buttonManager.isLeftJustPressed()) {
                rfManager.stop();
                currentState = STATE_RF_MENU;
                displayManager.drawRFMenu(rfMenuIndex);
                break;
            }

            // Render updates every 200ms
            if (millis() - lastDisplayUpdate > 200) {
                lastDisplayUpdate = millis();
                
                if (currentState == STATE_RF_LIVE) {
                    displayManager.drawRFLive(rfManager.getRSSI(), rfManager.getPPS(), rfManager.getStatusString());
                } 
                else if (currentState == STATE_RF_DEAUTH) {
                    displayManager.drawRFDeauth(rfManager.getMgmtPPS(), rfManager.getStatusString());
                }
                else if (currentState == STATE_RF_BEACON) {
                    displayManager.drawRFBeacon(rfManager.getUniqueSSIDCount(), rfManager.getStatusString());
                }
                else if (currentState == STATE_RF_SPIKE) {
                    displayManager.drawRFSpike(rfManager.getStatusString(), rfManager.getPeakRSSI());
                }
                else if (currentState == STATE_RF_STATS) {
                    displayManager.drawRFStats(rfManager.getMgmtPPS(), rfManager.getDataPPS(), rfManager.getCtrlPPS(), rfManager.getPPS());
                }
                else if (currentState == STATE_RF_HEATMAP) {
                    displayManager.drawRFHeatmap(rfManager.getHeatmap());
                }
                else if (currentState == STATE_RF_RADAR) {
                    displayManager.drawRFRadar(rfManager.getRSSI(), rfManager.getStatusString());
                }
            }
            break;

        case STATE_RF_LOGGER:
            // Placeholder Logger Menu
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_RF_MENU;
                displayManager.drawRFMenu(rfMenuIndex);
            }
            if (millis() - lastDisplayUpdate > 500) {
                lastDisplayUpdate = millis();
                displayManager.drawRFLogger("ON", 5, 2);
            }
            break;

        case STATE_RF_LOG_VIEWER:
            // Placeholder Log Viewer
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_RF_MENU;
                displayManager.drawRFMenu(rfMenuIndex);
            }
            if (millis() - lastDisplayUpdate > 500) {
                lastDisplayUpdate = millis();
                displayManager.drawRFLogViewer("rf_event.txt", "T:1000 R:-50", "P:20 S:CLEAN", "-----------");
            }
            break;

        case STATE_OSD_MENU:
            if (buttonManager.isUpJustPressed()) {
                osdManager.menuUp();
                displayManager.drawOSDMenu(osdManager.getMenuIndex());
            }
            if (buttonManager.isDownJustPressed()) {
                osdManager.menuDown();
                displayManager.drawOSDMenu(osdManager.getMenuIndex());
            }
            if (buttonManager.isLeftJustPressed()) {
                appManager.stopActive();
                currentState = STATE_MAIN_MENU;
                displayManager.drawMainMenu(menuIndex);
            }
            if (buttonManager.isOKJustPressed() || buttonManager.isRightJustPressed()) {
                if (osdManager.getMenuIndex() == 0) {
                    currentState = STATE_OSD_CUSTOM_TEXT;
                    osdManager.onEnterCustomText();
                    osdManager.render();
                } else if (osdManager.getMenuIndex() == 1) {
                    appManager.switchTo(APP_OSD_RUNNING_TEXT);
                    currentState = STATE_OSD_RUNNING_TEXT;
                    lastDisplayUpdate = 0;
                } else {
                    appManager.switchTo(APP_OSD_EYE_ANIMATION);
                    currentState = STATE_OSD_EYE_ANIMATION;
                    if (eyeAnimation) {
                        eyeAnimation->setExpression(EyeAnimation::Idle);
                        eyeAnimation->render();
                    }
                }
            }
            break;

        case STATE_OSD_CUSTOM_TEXT:
            static unsigned long osdHoldOkTime = 0;
            if (buttonManager.isUpJustPressed()) {
                osdManager.editorUp();
                osdManager.render();
            }
            if (buttonManager.isDownJustPressed()) {
                osdManager.editorDown();
                osdManager.render();
            }
            if (buttonManager.isRightJustPressed()) {
                osdManager.editorRight();
                osdManager.render();
            }
            if (buttonManager.isLeftJustPressed()) {
                osdManager.editorBackspace();
                if (strlen(osdManager.getCustomText()) == 0) {
                    osdManager.onEnterMenu();
                    currentState = STATE_OSD_MENU;
                    displayManager.drawOSDMenu(osdManager.getMenuIndex());
                } else {
                    osdManager.render();
                }
            }
            if (buttonManager.isOKJustPressed()) {
                osdManager.editorInsert();
                osdManager.render();
            }
            if (buttonManager.isOKPressed()) {
                if (osdHoldOkTime == 0) osdHoldOkTime = millis();
                if (millis() - osdHoldOkTime > 2000) {
                    osdHoldOkTime = 0;
                    if (osdManager.editorHoldSave()) {
                        displayManager.showMessage("Saved OSD Text");
                        osdManager.onEnterMenu();
                        currentState = STATE_OSD_MENU;
                        displayManager.drawOSDMenu(osdManager.getMenuIndex());
                    }
                }
            } else {
                osdHoldOkTime = 0;
            }
            break;

        case STATE_OSD_RUNNING_TEXT:
            if (buttonManager.isLeftJustPressed() || buttonManager.isRightJustPressed()) {
                appManager.stopActive();
                currentState = STATE_OSD_MENU;
                displayManager.drawOSDMenu(osdManager.getMenuIndex());
            }
            if (buttonManager.isOKJustPressed()) {
                osdManager.runningTogglePause();
            }
            break;

        case STATE_OSD_EYE_ANIMATION:
            if (buttonManager.isLeftJustPressed()) {
                appManager.stopActive();
                currentState = STATE_OSD_MENU;
                osdManager.onEnterMenu();
                displayManager.drawOSDMenu(osdManager.getMenuIndex());
            }
            if (millis() - lastDisplayUpdate > 200) {
                lastDisplayUpdate = millis();
                if (eyeAnimation) {
                    eyeAnimation->update();
                    eyeAnimation->render();
                }
            }
            break;

        case STATE_SETTINGS_MENU:
            if (buttonManager.isLeftJustPressed()) {
                osdManager.onExit();
                currentState = STATE_MAIN_MENU;
                displayManager.drawMainMenu(menuIndex);
            }
            if (buttonManager.isOKJustPressed() || buttonManager.isRightJustPressed()) {
                currentState = STATE_SETTINGS_SCREEN_SLEEP;
                osdManager.onEnterScreenSleep();
                displayManager.drawScreenSleepMenu(osdManager.isSleepEnabled(), osdManager.getSleepTimeoutMs());
            }
            break;

        case STATE_SETTINGS_SCREEN_SLEEP:
            if (buttonManager.isLeftJustPressed()) {
                osdManager.onEnterSettingsMenu();
                currentState = STATE_SETTINGS_MENU;
                displayManager.drawSettingsMenu(0);
            }
            if (buttonManager.isOKJustPressed()) {
                osdManager.sleepToggleEnabled();
                displayManager.drawScreenSleepMenu(osdManager.isSleepEnabled(), osdManager.getSleepTimeoutMs());
            }
            if (buttonManager.isUpJustPressed()) {
                osdManager.sleepTimeoutUp();
                displayManager.drawScreenSleepMenu(osdManager.isSleepEnabled(), osdManager.getSleepTimeoutMs());
            }
            if (buttonManager.isDownJustPressed()) {
                osdManager.sleepTimeoutDown();
                displayManager.drawScreenSleepMenu(osdManager.isSleepEnabled(), osdManager.getSleepTimeoutMs());
            }
            break;

        case STATE_REPEATER_MENU:
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_MAIN_MENU;
                displayManager.drawMainMenu(menuIndex);
            }
            if (buttonManager.isUpJustPressed()) {
                repMenuIndex--;
                if(repMenuIndex < 0) repMenuIndex = 3;
                displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
            }
            if (buttonManager.isDownJustPressed()) {
                repMenuIndex++;
                if(repMenuIndex > 3) repMenuIndex = 0;
                displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
            }
            if (buttonManager.isOKJustPressed() || buttonManager.isRightJustPressed()) {
                if (repMenuIndex == 0) { // Start Repeater
                    String ssid, pass;
                    // Auto load slot 0 if exists
                    if (repeaterManager.loadNetwork(0, ssid, pass)) {
                        repeaterManager.startRepeater(ssid, pass);
                        repConnectStartTime = millis();
                        repSelectedSSID = ssid;
                        currentState = STATE_REPEATER_CONNECTING;
                    } else {
                        // Tidak ada config, paksa ke menu scan
                        currentState = STATE_REPEATER_SCAN;
                        displayManager.drawWiFiList(0, 0, "", 0, false);
                        repWifiTotal = repeaterManager.scanNetworks();
                        repWifiIndex = 0;
                        currentState = STATE_REPEATER_SSID_LIST;
                        displayManager.drawWiFiList(repWifiIndex, repWifiTotal, repeaterManager.getScannedSSID(repWifiIndex), repeaterManager.getScannedRSSI(repWifiIndex), repeaterManager.getScannedEncryption(repWifiIndex));
                    }
                } else if (repMenuIndex == 1) { // Scan Jaringan
                    currentState = STATE_REPEATER_SCAN;
                    displayManager.drawWiFiList(0, 0, "", 0, false);
                    repWifiTotal = repeaterManager.scanNetworks();
                    repWifiIndex = 0;
                    currentState = STATE_REPEATER_SSID_LIST;
                    displayManager.drawWiFiList(repWifiIndex, repWifiTotal, repeaterManager.getScannedSSID(repWifiIndex), repeaterManager.getScannedRSSI(repWifiIndex), repeaterManager.getScannedEncryption(repWifiIndex));
                } else if (repMenuIndex == 2) { // Saved Networks
                    repSavedNetIndex = 0;
                    currentState = STATE_REPEATER_SAVED_NETS;
                    displayManager.drawSavedNetworksMenu(repSavedNetIndex, repeaterManager.getSavedNetworksCount(), repeaterManager.getSavedNetworkSSID(repSavedNetIndex));
                } else if (repMenuIndex == 3) { // AP Config
                    repApConfigIndex = 0;
                    currentState = STATE_REPEATER_AP_CONFIG;
                    displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
                }
            }
            break;

        case STATE_REPEATER_SCAN:
            break;

        case STATE_REPEATER_SSID_LIST:
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_REPEATER_MENU;
                displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
            }
            if (buttonManager.isUpJustPressed()) {
                repWifiIndex--;
                if(repWifiIndex < 0) repWifiIndex = repWifiTotal - 1;
                displayManager.drawWiFiList(repWifiIndex, repWifiTotal, repeaterManager.getScannedSSID(repWifiIndex), repeaterManager.getScannedRSSI(repWifiIndex), repeaterManager.getScannedEncryption(repWifiIndex));
            }
            if (buttonManager.isDownJustPressed()) {
                repWifiIndex++;
                if(repWifiIndex >= repWifiTotal) repWifiIndex = 0;
                displayManager.drawWiFiList(repWifiIndex, repWifiTotal, repeaterManager.getScannedSSID(repWifiIndex), repeaterManager.getScannedRSSI(repWifiIndex), repeaterManager.getScannedEncryption(repWifiIndex));
            }
            if (buttonManager.isOKJustPressed() || buttonManager.isRightJustPressed()) {
                if (repWifiTotal > 0) {
                    repSelectedSSID = repeaterManager.getScannedSSID(repWifiIndex);
                    repKeyboardText = "";
                    repKx = 0;
                    repKy = 0;
                    currentState = STATE_REPEATER_KEYBOARD;
                    displayManager.drawVirtualKeyboard(repKeyboardText, repKx, repKy);
                }
            }
            break;

        case STATE_REPEATER_KEYBOARD: {
            bool updateScreen = false;
            if (buttonManager.isLeftJustPressed()) {
                if (repKeyboardText.length() > 0) {
                    repKeyboardText.remove(repKeyboardText.length() - 1);
                    updateScreen = true;
                } else {
                    currentState = STATE_REPEATER_SSID_LIST;
                    displayManager.drawWiFiList(repWifiIndex, repWifiTotal, repeaterManager.getScannedSSID(repWifiIndex), repeaterManager.getScannedRSSI(repWifiIndex), repeaterManager.getScannedEncryption(repWifiIndex));
                }
            }
            if (buttonManager.isUpJustPressed()) { repKy--; if(repKy < 0) repKy = 2; updateScreen = true; }
            if (buttonManager.isDownJustPressed()) { repKy++; if(repKy > 2) repKy = 0; updateScreen = true; }
            if (buttonManager.isRightJustPressed()) { repKx++; if(repKx > 11) repKx = 0; updateScreen = true; }
            
            static unsigned long holdOkTime = 0;
            // Di sini kita cek logika tahan OK
            if (buttonManager.isOKPressed()) {
                if(holdOkTime == 0) holdOkTime = millis();
                if(millis() - holdOkTime > 2000) { // Tahan 2 Detik = ENTER
                    holdOkTime = 0;
                    repeaterManager.saveNetwork(repSelectedSSID, repKeyboardText);
                    repeaterManager.startRepeater(repSelectedSSID, repKeyboardText);
                    repConnectStartTime = millis();
                    currentState = STATE_REPEATER_CONNECTING;
                }
            } else {
                if (holdOkTime > 0) {
                    if(millis() - holdOkTime <= 2000) { // Cuma di-tap bentar
                        const char keys[3][13] = {
                            "ABCDEFGHIJKL",
                            "MNOPQRSTUVWX",
                            "YZ0123456789"
                        };
                        char c = keys[repKy][repKx];
                        if (c != ' ') {
                            repKeyboardText += c;
                        }
                        updateScreen = true;
                    }
                    holdOkTime = 0;
                }
            }

            if (updateScreen || (millis() - lastDisplayUpdate > 500)) {
                lastDisplayUpdate = millis();
                if (currentState == STATE_REPEATER_KEYBOARD) {
                    displayManager.drawVirtualKeyboard(repKeyboardText, repKx, repKy);
                }
            }
            break;
        }

        case STATE_REPEATER_CONNECTING:
            if (buttonManager.isLeftJustPressed()) {
                repeaterManager.stopRepeater();
                currentState = STATE_REPEATER_MENU;
                displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
            } else {
                int elapsed = (millis() - repConnectStartTime) / 1000;
                int left = 10 - elapsed;
                
                if (repeaterManager.isConnected()) {
                    repeaterManager.configureNAPT();
                    currentState = STATE_REPEATER_ACTIVE;
                } else if (left <= 0) {
                    repeaterManager.stopRepeater();
                    currentState = STATE_REPEATER_MENU;
                    displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
                } else {
                    if (millis() - lastDisplayUpdate > 500) {
                        lastDisplayUpdate = millis();
                        displayManager.drawRepeaterConnecting(left, repSelectedSSID);
                    }
                }
            }
            break;

        case STATE_REPEATER_ACTIVE:
            repeaterManager.update();
            if (buttonManager.isLeftJustPressed()) {
                repeaterManager.stopRepeater();
                currentState = STATE_REPEATER_MENU;
                displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
            }
            if (millis() - lastDisplayUpdate > 1000) {
                lastDisplayUpdate = millis();
                displayManager.drawRepeaterMonitor(
                    repeaterManager.getRxBytes(),
                    repeaterManager.getTxBytes(),
                    repeaterManager.getUpSpeed(),
                    repeaterManager.getDownSpeed(),
                    repeaterManager.getConnectedClients()
                );
            }
            break;

        case STATE_REPEATER_SAVED_NETS: {
            int total = repeaterManager.getSavedNetworksCount();
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_REPEATER_MENU;
                displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
            }
            if (buttonManager.isUpJustPressed() && total > 0) {
                repSavedNetIndex--; if(repSavedNetIndex < 0) repSavedNetIndex = total - 1;
                displayManager.drawSavedNetworksMenu(repSavedNetIndex, total, repeaterManager.getSavedNetworkSSID(repSavedNetIndex));
            }
            if (buttonManager.isDownJustPressed() && total > 0) {
                repSavedNetIndex++; if(repSavedNetIndex >= total) repSavedNetIndex = 0;
                displayManager.drawSavedNetworksMenu(repSavedNetIndex, total, repeaterManager.getSavedNetworkSSID(repSavedNetIndex));
            }
            if (buttonManager.isOKJustPressed() && total > 0) {
                // Load Network
                String ssid, pass;
                if(repeaterManager.loadNetwork(repSavedNetIndex, ssid, pass)) {
                    repeaterManager.startRepeater(ssid, pass);
                    repConnectStartTime = millis();
                    repSelectedSSID = ssid;
                    currentState = STATE_REPEATER_CONNECTING;
                }
            }
            if (buttonManager.isRightJustPressed() && total > 0) {
                // Delete Network Confirmation
                currentState = STATE_REPEATER_DEL_CONFIRM;
                repDelConfirmYes = false;
                displayManager.drawDeleteConfirmMenu(repeaterManager.getSavedNetworkSSID(repSavedNetIndex), repDelConfirmYes);
            }
            break;
        }

        case STATE_REPEATER_DEL_CONFIRM:
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_REPEATER_SAVED_NETS;
                displayManager.drawSavedNetworksMenu(repSavedNetIndex, repeaterManager.getSavedNetworksCount(), repeaterManager.getSavedNetworkSSID(repSavedNetIndex));
            }
            if (buttonManager.isUpJustPressed() || buttonManager.isDownJustPressed()) {
                repDelConfirmYes = !repDelConfirmYes;
                displayManager.drawDeleteConfirmMenu(repeaterManager.getSavedNetworkSSID(repSavedNetIndex), repDelConfirmYes);
            }
            if (buttonManager.isOKJustPressed()) {
                if(repDelConfirmYes) {
                    repeaterManager.deleteNetwork(repSavedNetIndex);
                    repSavedNetIndex = 0;
                }
                currentState = STATE_REPEATER_SAVED_NETS;
                displayManager.drawSavedNetworksMenu(repSavedNetIndex, repeaterManager.getSavedNetworksCount(), repeaterManager.getSavedNetworkSSID(repSavedNetIndex));
            }
            break;

        case STATE_REPEATER_AP_CONFIG:
            if (buttonManager.isLeftJustPressed()) {
                currentState = STATE_REPEATER_MENU;
                displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
            }
            if (buttonManager.isUpJustPressed()) {
                repApConfigIndex--; if(repApConfigIndex < 0) repApConfigIndex = 5;
                displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
            }
            if (buttonManager.isDownJustPressed()) {
                repApConfigIndex++; if(repApConfigIndex > 5) repApConfigIndex = 0;
                displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
            }
            if (buttonManager.isRightJustPressed() || buttonManager.isOKJustPressed()) {
                if (repApConfigIndex == 0) { // Edit SSID
                    repApKbTarget = 0;
                    repKeyboardText = repeaterManager.apConfig.ssidName;
                    repKx = 0; repKy = 0;
                    currentState = STATE_REPEATER_AP_KB;
                    displayManager.drawVirtualKeyboard(repKeyboardText, repKx, repKy);
                } else if (repApConfigIndex == 1) { // Edit Pass
                    repApKbTarget = 1;
                    repKeyboardText = repeaterManager.apConfig.password;
                    repKx = 0; repKy = 0;
                    currentState = STATE_REPEATER_AP_KB;
                    displayManager.drawVirtualKeyboard(repKeyboardText, repKx, repKy);
                } else if (repApConfigIndex == 2) { // Toggle WPA2
                    repeaterManager.apConfig.isWPA2 = !repeaterManager.apConfig.isWPA2;
                    displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
                } else if (repApConfigIndex == 3) { // Toggle Hidden
                    repeaterManager.apConfig.isHidden = !repeaterManager.apConfig.isHidden;
                    displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
                } else if (repApConfigIndex == 4) { // Save & Apply
                    repeaterManager.saveAPConfig();
                    displayManager.showMessage("Config Saved!");
                    delay(1000);
                    currentState = STATE_REPEATER_MENU;
                    displayManager.drawRepeaterMenu(repMenuIndex, repeaterManager.hasSavedNetworks());
                } else if (repApConfigIndex == 5) { // Factory Reset
                    repeaterManager.factoryResetAP();
                    displayManager.showMessage("Reset OK!");
                    delay(1000);
                    displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
                }
            }
            break;

        case STATE_REPEATER_AP_KB: {
            bool updateScreen = false;
            if (buttonManager.isLeftJustPressed()) {
                if (repKeyboardText.length() > 0) {
                    repKeyboardText.remove(repKeyboardText.length() - 1);
                    updateScreen = true;
                } else {
                    currentState = STATE_REPEATER_AP_CONFIG;
                    displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
                }
            }
            if (buttonManager.isUpJustPressed()) { repKy--; if(repKy < 0) repKy = 2; updateScreen = true; }
            if (buttonManager.isDownJustPressed()) { repKy++; if(repKy > 2) repKy = 0; updateScreen = true; }
            if (buttonManager.isRightJustPressed()) { repKx++; if(repKx > 11) repKx = 0; updateScreen = true; }
            
            static unsigned long holdOkTime2 = 0;
            if (buttonManager.isOKPressed()) {
                if(holdOkTime2 == 0) holdOkTime2 = millis();
                if(millis() - holdOkTime2 > 2000) { // ENTER
                    holdOkTime2 = 0;
                    if(repApKbTarget == 0) repeaterManager.apConfig.ssidName = repKeyboardText;
                    else repeaterManager.apConfig.password = repKeyboardText;
                    
                    currentState = STATE_REPEATER_AP_CONFIG;
                    displayManager.drawRepeaterConfigMenu(repApConfigIndex, repeaterManager.apConfig.ssidName, repeaterManager.apConfig.password, repeaterManager.apConfig.isWPA2, repeaterManager.apConfig.isHidden);
                }
            } else {
                if (holdOkTime2 > 0) {
                    if(millis() - holdOkTime2 <= 2000) {
                        const char keys[3][13] = {
                            "ABCDEFGHIJKL",
                            "MNOPQRSTUVWX",
                            "YZ0123456789"
                        };
                        char c = keys[repKy][repKx];
                        if (c != ' ') {
                            repKeyboardText += c;
                        }
                        updateScreen = true;
                    }
                    holdOkTime2 = 0;
                }
            }

            if (updateScreen || (millis() - lastDisplayUpdate > 500)) {
                lastDisplayUpdate = millis();
                if (currentState == STATE_REPEATER_AP_KB) {
                    displayManager.drawVirtualKeyboard(repKeyboardText, repKx, repKy);
                }
            }
            break;
        }
    }
}
