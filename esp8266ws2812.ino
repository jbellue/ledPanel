#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "patternsManager.h"

#define LED_PIN   D4
#define LED_COUNT 12
#define HTTP_PORT 80
#define WEBSOCKET_PORT 81

extern const char index_html[];

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
PatternsManager pattern(&strip);
ESP8266WebServer server(HTTP_PORT);
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

void sendSettingsToClient(uint8_t num) {
    const size_t jsonSize = JSON_ARRAY_SIZE(6) + (PatternChoice::LAST_PATTERN + 1)*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5);
    StaticJsonDocument<jsonSize> doc;

    JsonObject settings = doc.createNestedObject("settings");
    settings["brightness"] = pattern.brightness();
    settings["colour1"] = pattern.colour1();
    settings["colour2"] = pattern.colour2();
    settings["speed"] = pattern.speed();
    settings["pattern"] = pattern.pattern();

    JsonArray patterns = doc.createNestedArray("patterns");
    const char* patternsName[PatternChoice::LAST_PATTERN + 1];
    uint8_t patternsColourNumber[PatternChoice::LAST_PATTERN + 1];
    pattern.getPatternsInfo(patternsName, patternsColourNumber);
    for(uint8_t i = 0; i <= PatternChoice::LAST_PATTERN; ++i) {
        JsonObject pattern = patterns.createNestedObject();
        pattern["name"] = patternsName[i];
        pattern["numColour"] = patternsColourNumber[i];
    }
    const int jsonStringSize = measureJson(doc); 
    char jsonString[jsonStringSize + 1];
    serializeJson(doc, jsonString, sizeof(jsonString));
    webSocket.sendTXT(num, jsonString);
}

void processReceivedText(uint8_t num, uint8_t* payload) {
    const size_t jsonSize = JSON_OBJECT_SIZE(3) + 30;
    StaticJsonDocument<jsonSize> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        const size_t capacity = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<capacity> errorDoc;
        errorDoc["error"] = "deserializeJson() failed";
        errorDoc["errorText"] = error.c_str();

        const int jsonErrorStringSize = measureJson(errorDoc); 
        char jsonErrorString[jsonErrorStringSize + 1];
        serializeJson(errorDoc, jsonErrorString, sizeof(jsonErrorString));
        webSocket.sendTXT(num, jsonErrorString);
    }
    else {
        const uint8_t brightness = doc["brightness"];
        if (brightness != 0) {
            pattern.brightness(brightness);
        }

        const JsonVariant jsonPattern = doc["pattern"];
        if (!jsonPattern.isNull()) {
            pattern.changePattern(jsonPattern.as<uint8_t>());
        }
        const JsonVariant jsonSpeed = doc["speed"];
        if (!jsonSpeed.isNull()) {
            pattern.speed(jsonSpeed.as<uint8_t>());
        }

        const uint32_t colour1 = doc["colour1"];
        if (colour1) {
            pattern.colour1(colour1);
        }
        const uint32_t colour2 = doc["colour2"];
        if (colour2) {
            pattern.colour2(colour2);
        }

        webSocket.sendTXT(num, "{\"status\":\"OK\"}");
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\r\n", num);
        break;
    case WStype_CONNECTED:
        sendSettingsToClient(num);
        break;
    case WStype_TEXT:
        processReceivedText(num, payload);
        break;
    case WStype_BIN:
        Serial.printf("[%u] get binary length: %u\r\n", num, length);
        hexdump(payload, length);

        // echo data back to browser
        webSocket.sendBIN(num, payload, length);
        break;
    default:
        Serial.printf("Invalid WStype [%d]\r\n", type);
        break;
    }
}

void setup() {
    pattern.begin();

    Serial.begin(115200);

    WiFiManager wifiManager;
    wifiManager.autoConnect();

    Serial.print("\nServer IP is ");
    Serial.println(WiFi.localIP());

    configServer();
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void configServer() {
    server.onNotFound([]() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(404, "text/plain", "Page not found");
    });
    server.on("/", []() {
        server.send_P(200, "text/html", index_html);
    });
}

void loop() {
    server.handleClient();
    webSocket.loop();
    pattern.update();
}

