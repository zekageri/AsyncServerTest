#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <socketHandler/socketHandler.h>

SocketHandler socketHandler;
AsyncWebServer server(80);
#define INDEX_PATH "/index.html"

const char *ssid = "YOUR SSID";
const char *password = "YOURPW";

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
}

void loop() {
    socketHandler.run();
}