#include <math.h>
#include <WiFi.h>
#include <time.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <ESP32RotaryEncoder.h>
#include "system.h"
#include "wifiCredentials.h"

#define HANDLE_OTA_INTERVAL 500 // how many ms to wait between OTA checks

RotaryEncoder rotaryEncoder0 = RotaryEncoder(ROTARY_ENCODER_0_A_PIN, ROTARY_ENCODER_0_B_PIN, ROTARY_ENCODER_0_BUTTON_PIN);
RotaryEncoder rotaryEncoder1 = RotaryEncoder(ROTARY_ENCODER_1_A_PIN, ROTARY_ENCODER_1_B_PIN, ROTARY_ENCODER_1_BUTTON_PIN);

volatile int rotationInput0 = 0;
volatile int rotationInput1 = 0;
volatile bool buttonPressed0 = false;
volatile bool buttonPressed1 = false;

static int lastEncoderValue0 = 0;
static int lastEncoderValue1 = 0;

static int handleOtaTimer = 0;

void knob0Turned(long value)
{
    rotationInput0 += value - lastEncoderValue0;
    lastEncoderValue0 = value;
}

void knob1Turned(long value)
{
    rotationInput1 += value - lastEncoderValue1;
    lastEncoderValue1 = value;
}

void knob0Pressed(unsigned long duration)
{
    buttonPressed0 = true;
}

void knob1Pressed(unsigned long duration)
{
    buttonPressed1 = true;
}

void rotaryEncoderSetup()
{
    const int lowerBoundary = -1000000;
    const int upperBoundary = 1000000;
    const bool wrapValues = true;

    rotaryEncoder0.setEncoderType(EncoderType::HAS_PULLUP); // TODO: See if button presses are truly more reliable with this set to EncoderType::FLOATING.
    rotaryEncoder0.setBoundaries(lowerBoundary, upperBoundary, wrapValues);
    rotaryEncoder0.onTurned(&knob0Turned);
    rotaryEncoder0.onPressed(&knob0Pressed);
    rotaryEncoder0.begin();

    rotaryEncoder1.setEncoderType(EncoderType::HAS_PULLUP); // TODO: See if button presses are truly more reliable with this set to EncoderType::FLOATING.
    rotaryEncoder1.setBoundaries(lowerBoundary, upperBoundary, wrapValues);
    rotaryEncoder1.onTurned(&knob1Turned);
    rotaryEncoder1.onPressed(&knob1Pressed);
    rotaryEncoder1.begin();
}


void connectToWifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPassword);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("WiFi Failed!");
        return;
    }
    delay(300); // TODO: see if this can be removed
    ArduinoOTA.setPort(3232); // The default
    ArduinoOTA.setHostname("ESP32-LED-Matrix");
    ArduinoOTA.begin();
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
}

// This must be called periodically so that it can receive a 
// program update over wifi.
void handleOtaPeriodic()
{
    if (millis() - handleOtaTimer > HANDLE_OTA_INTERVAL)
    {
        handleOtaTimer = millis();
        ArduinoOTA.handle();
    }
}

void synchrnoizeTime()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
    setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
}