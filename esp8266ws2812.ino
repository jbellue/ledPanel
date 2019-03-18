#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "patternsManager.h"
#include "wifiCredentials.h"

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
    StaticJsonDocument<JSON_ARRAY_SIZE(PatternChoice::LAST_PATTERN + 1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4)> doc;
    doc["settings"]["brightness"] = strip.getBrightness();
    doc["settings"]["colour1"] = pattern.colour1();
    doc["settings"]["colour2"] = pattern.colour2();
    doc["settings"]["speed"] = pattern.speed();

    JsonArray patterns = doc.createNestedArray("patterns");
    const char* patternsName[PatternChoice::LAST_PATTERN + 1];
    pattern.getPatternsName(patternsName);
    for(uint8_t i = 0; i <= PatternChoice::LAST_PATTERN; ++i) {
        patterns.add(patternsName[i]);
    }
    String jsonString;
    serializeJson(doc, jsonString);
    webSocket.sendTXT(num, jsonString);
}

void processReveivedText(uint8_t num, uint8_t* payload) {
    const size_t jsonSize = JSON_OBJECT_SIZE(3) + 30;
    DynamicJsonDocument doc(jsonSize);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        // these should probably be part of the JSON response...
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        webSocket.sendTXT(num, "{\"status\":\"KO\"}");
    }
    else {
        const uint8_t brightness = doc["brightness"];
        if (brightness != 0) {
            strip.setBrightness(brightness);
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
        processReveivedText(num, payload);
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

    // init WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to " WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
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

