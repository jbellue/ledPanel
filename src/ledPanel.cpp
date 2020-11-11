#include <Arduino.h>

#include <WebSocketsServer.h>

#include <ESP8266WiFi.h>

#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncWebServer.h>

#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "patternsManager.h"

// Include the header file we create with gulp
#include "static/index.html.gz.h"

#define LED_PIN   2
#define LED_COUNT 12
#define HTTP_PORT 80
#define WEBSOCKET_PORT 81

// Variable to hold the last modification datetime
char last_modified[50];

AsyncWebServer server = AsyncWebServer(HTTP_PORT);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
PatternsManager pattern(&strip);
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

void onHome(AsyncWebServerRequest *request) {
    // Check if the client already has the same version and respond with a 304 (Not modified)
    if (request->header("If-Modified-Since").equals(last_modified)) {
        request->send(304);

    }
    else {
        // Dump the byte array in PROGMEM with a 200 HTTP code (OK)
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);

        // Tell the browswer the contemnt is Gzipped
        response->addHeader("Content-Encoding", "gzip");

        // And set the last-modified datetime so we can check if we need to send it again next time or not
        response->addHeader("Last-Modified", last_modified);

        request->send(response);
    }
}

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
    const size_t jsonStringSize = measureJson(doc); 
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

        const size_t jsonErrorStringSize = measureJson(errorDoc); 
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

        const size_t jsonStringSize = measureJson(outputDoc); 
        char jsonString[jsonStringSize + 1];
        serializeJson(outputDoc, jsonString, sizeof(jsonString));
        webSocket.broadcastTXT(jsonString);
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_CONNECTED) {
        sendSettingsToClient(num);
    }
    else if (type == WStype_TEXT) {
        processReceivedText(num, payload);
    }
}

void setup() {
    pattern.begin();

    // Populate the last modification date based on build datetime
    sprintf(last_modified, "%s %s GMT", __DATE__, __TIME__);

    DNSServer dns;
    AsyncWiFiManager wifiManager(&server, &dns);
    wifiManager.autoConnect();

    server.on("/", HTTP_GET, onHome);
    server.onNotFound([](AsyncWebServerRequest *request){ request->send(404); });
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
    ArduinoOTA.handle();
    pattern.update();
}
