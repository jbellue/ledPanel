#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
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
    const size_t jsonSize = JSON_ARRAY_SIZE(PatternChoice::LAST_PATTERN + 1) + (PatternChoice::LAST_PATTERN + 1)*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
    StaticJsonDocument<jsonSize> doc;

    doc["id"] = num;
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
        const size_t maxElementNumber = 5;
        // we need to add two elements:
        // 1 for the sender,
        // 1 to hold the "settings" object
        const size_t jsonSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(maxElementNumber);
        StaticJsonDocument<jsonSize> outputDoc;
        outputDoc["sender"] = num;
        JsonObject settings = outputDoc.createNestedObject("settings");
        const JsonVariantConst jsonBrightness = doc["brightness"];
        if (!jsonBrightness.isNull()) {
            const uint8_t newBrightness = jsonBrightness.as<uint8_t>();
            pattern.brightness(newBrightness);
            settings["brightness"] = newBrightness;
        }

        const JsonVariantConst jsonPattern = doc["pattern"];
        if (!jsonPattern.isNull()) {
            const uint8_t newPattern = jsonPattern.as<uint8_t>();
            pattern.changePattern(newPattern);
            settings["pattern"] = newPattern;
        }
        const JsonVariantConst jsonSpeed = doc["speed"];
        if (!jsonSpeed.isNull()) {
            const uint8_t newSpeed = jsonSpeed.as<uint8_t>();
            pattern.speed(newSpeed);
            settings["speed"] = newSpeed;
        }

        const JsonVariantConst jsonColour1 = doc["colour1"];
        if (!jsonColour1.isNull()) {
            const uint32_t newColour1 = jsonColour1.as<uint32_t>();
            pattern.colour1(newColour1);
            settings["colour1"] = newColour1;
        }
        const JsonVariantConst jsonColour2 = doc["colour2"];
        if (!jsonColour2.isNull()) {
            const uint32_t newColour2 = jsonColour2.as<uint32_t>();
            pattern.colour2(newColour2);
            settings["colour2"] = newColour2;
        }

        const int jsonStringSize = measureJson(outputDoc); 
        char jsonString[jsonStringSize + 1];
        serializeJson(outputDoc, jsonString, sizeof(jsonString));
        webSocket.broadcastTXT(jsonString);
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

    ArduinoOTA.onEnd([]() {
        webSocket.disconnect();
        ESP.reset();
    });

    ArduinoOTA.begin();
}

void loop() {
    server.handleClient();
    webSocket.loop();
    ArduinoOTA.handle();
    pattern.update();
}

