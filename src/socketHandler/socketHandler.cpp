#include <SocketHandler/SocketHandler.h>

void SocketHandler::init(AsyncWebServer *server, const char* endpoint) {
    sendMutex = xSemaphoreCreateMutex();

    mainSocket = new AsyncWebSocket(endpoint);
    server->addHandler(mainSocket);
    mainSocket->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_CONNECT) {
            Serial.printf("%d client connected.\n", client->id());
            printClientCount();
        } else if (type == WS_EVT_DISCONNECT) {
            Serial.println("Client disconnected");
            printClientCount();
        } else if (type == WS_EVT_DATA) {
            AwsFrameInfo *info = (AwsFrameInfo *)arg;
            for (size_t i = 0; i < len; i++) {
                if (!payloadTooLarge) {
                    buffer[bufferIndex] = data[i];
                }
                bufferIndex++;
                if (bufferIndex >= BUFF_SIZE) {
                    payloadTooLarge = true;
                }
            }
            if (bufferIndex >= info->len) {
                // Got all data.
                buffer[bufferIndex] = '\0';
                bufferIndex = 0;
                if (payloadTooLarge) {
                    Serial.println("Payload was too large. Abort processing the message!");
                    payloadTooLarge = false;
                    return;
                }
                processMessage(client->id());
            }
        }
    });
}

boolean SocketHandler::sendTo(int clientID, const char *msg, ...) {
    if (mainSocket->count() < 1) {
        return false;
    }
    va_list args;
    va_start(args, msg);
    vsprintf(sendBuff, msg, args);
    va_end(args);
    mainSocket->text(clientID, sendBuff);
    return true;
}

boolean SocketHandler::sendAll(const char *msg, ...) {
    if (mainSocket->count() < 1) {
        return false;
    }
    if ( xSemaphoreTake (sendMutex, portMAX_DELAY)) {
        va_list args;
        va_start(args, msg);
            vsprintf(sendBuff, msg, args);
        va_end(args);
        mainSocket->textAll(sendBuff);
    }
    xSemaphoreGive (sendMutex);
    return true;
}

void SocketHandler::printClientCount() {
    if( !isDebugOn ){ return; }
    Serial.printf("There are %d client(s).\n", mainSocket->count());
}

void SocketHandler::processMessage(int clientID) {
    Serial.printf("***** Got a socket message from client %d *****\n", clientID);

    StaticJsonDocument<10000> doc;
    DeserializationError error = deserializeJson(doc,buffer);
    if( error ){
        Serial.printf("\nNon json message\n%s\n",buffer);
        return;
    }

    Serial.println();
    serializeJsonPretty(doc,Serial);
    Serial.println();

}

void SocketHandler::sendPong(){
    if(millis() - lastPongMS >= 1000){
        lastPongMS = millis();
        int core = xPortGetCoreID();
        sendAll("Ping from %d",core);
    }
}

void SocketHandler::run(){
    sendPong();
    mainSocket->cleanupClients();
}