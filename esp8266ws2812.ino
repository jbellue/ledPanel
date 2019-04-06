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

void sendSettingsToClient(const uint8_t num) {
    const size_t jsonSize = JSON_ARRAY_SIZE(PatternChoice::LAST_PATTERN + 1) + (PatternChoice::LAST_PATTERN + 2)*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5);
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

void processReceivedText(const uint8_t num, uint8_t* payload) {
    const size_t jsonSize = JSON_OBJECT_SIZE(3) + 30;
    StaticJsonDocument<jsonSize> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        const size_t capacity = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<capacity> errorDoc;
        errorDoc["error"] = F("deserializeJson() failed");
        errorDoc["errorText"] = error.c_str();

        const int jsonErrorStringSize = measureJson(errorDoc); 
        char jsonErrorString[jsonErrorStringSize + 1];
        serializeJson(errorDoc, jsonErrorString, sizeof(jsonErrorString));
        webSocket.sendTXT(num, jsonErrorString);
    }
    else {
        const JsonVariantConst jsonBrightness = doc["brightness"];
        if (!jsonBrightness.isNull()) {
            pattern.brightness(jsonBrightness.as<uint8_t>());
        }

        const JsonVariantConst jsonPattern = doc["pattern"];
        if (!jsonPattern.isNull()) {
            pattern.changePattern(jsonPattern.as<uint8_t>());
        }
        const JsonVariantConst jsonSpeed = doc["speed"];
        if (!jsonSpeed.isNull()) {
            pattern.speed(jsonSpeed.as<uint8_t>());
        }

        const JsonVariantConst jsonColour1 = doc["colour1"];
        if (!jsonColour1.isNull()) {
            pattern.colour1(jsonColour1.as<uint32_t>());
        }
        const JsonVariantConst jsonColour2 = doc["colour2"];
        if (!jsonColour2.isNull()) {
            pattern.colour2(jsonColour2.as<uint32_t>());
        }

        webSocket.sendTXT(num, "{\"status\":\"OK\"}");
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
    case WStype_CONNECTED:
        sendSettingsToClient(num);
        break;
    case WStype_TEXT:
        processReceivedText(num, payload);
        break;
    default:
        Serial.print(F("Ignored WStype "));
        Serial.println(type);
        break;
    }
}

void setup() {
    pattern.begin();

    Serial.begin(115200);

    WiFiManager wifiManager;
    wifiManager.autoConnect();

    // There's no other endpoint, so send the index to every request
    server.onNotFound([]() {
        server.send_P(200, "text/html", index_html);
    });
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    server.handleClient();
    webSocket.loop();
    pattern.update();
}

