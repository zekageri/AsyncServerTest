#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <socketHandler/socketHandler.h>
#include <uptimeHandler/uptimeHandler.h>

UptimeHandler uptimeHandler;
SocketHandler socketHandler;
AsyncWebServer server(80);
#define INDEX_PATH "/index.html"

void testTask(void* parameter);

const char *ssid = "HITEC Industrial Technology";
const char *password = "Hitec_EtHiP";

void setupAP() {
    Serial.printf("Setting up AP mode.\n");
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP("AsyncServerTest")) {
        Serial.println("Failed to set up AP mode.");
        return;
    }
    delay(300);
    Serial.printf("IP: %s\n", WiFi.softAPIP().toString().c_str());
}

void setupWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        setupAP();
        return;
    }
    Serial.printf("Wifi connected to %s\n", ssid);
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
}

void setupEndpoints() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, INDEX_PATH, "text/html");
    });
}

void setup() {
    Serial.begin(115200);
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount file system!");
        ESP.restart();
    }
    setupWifi();
    setupEndpoints();

    socketHandler.init(&server, "/main");
    server.serveStatic("/", LittleFS, "/");
    server.begin();

    xTaskCreateUniversal(testTask, "testTask", 2000, NULL, 2, NULL, -1);

}


long lastPongMS = 0;
void loop() {
    uptimeHandler.run();
    socketHandler.run();
    if(millis() - lastPongMS >= 1000){
        lastPongMS = millis();
        int core = xPortGetCoreID();
        socketHandler.sendAll("Ping from core %d uptime: %s",core,uptimeHandler.get());
    }
}

void testTask(void* parameter){
    int core = xPortGetCoreID();
    for(;;){
        socketHandler.sendAll("Ping from core %d",core);
        vTaskDelay(1000);
    }
}