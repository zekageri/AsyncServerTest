#ifndef socketHandler_h
#define socketHandler_h

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define BUFF_SIZE 10000
class SocketHandler {
    private:
        SemaphoreHandle_t sendMutex = NULL;
        AsyncWebSocket* mainSocket;

        boolean isDebugOn = true;
        boolean payloadTooLarge = false;
        int bufferIndex = 0;
        char buffer[BUFF_SIZE];
        char sendBuff[BUFF_SIZE];

        void printClientCount();
        void processMessage(int clientID);

        long lastPongMS = 0;
        void sendPong();
    public:
        void init(AsyncWebServer* server, const char* endpoint);
        boolean sendTo(int clientID, const char* msg, ...);
        boolean sendAll(const char* msg, ...);
        void run();
};

#endif