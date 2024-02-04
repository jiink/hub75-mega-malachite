#include <math.h>
#include <WiFi.h>
#include <time.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include "system.h"
#include "wifiCredentials.h"

#define HANDLE_OTA_INTERVAL 500 // how many ms to wait between OTA checks

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

volatile int rotationInput = 0;
volatile bool buttonPressed = false;

static int lastEncoderValue = 0;
static unsigned long lastTimePressed = 0;

static int handleOtaTimer = 0;

void rotaryLoop()
{
    //dont print anything unless value changed
    if (rotaryEncoder.encoderChanged())
    {
        int currentValue = rotaryEncoder.readEncoder();
        rotationInput += currentValue - lastEncoderValue;
        lastEncoderValue = currentValue;
        Serial.printf("rotationInput: %d\r\n", rotationInput);
    }
    if (rotaryEncoder.isEncoderButtonClicked())
    {
        printf("m - l: %d - %d\r\n", millis(), lastTimePressed);
        // Soft debouncing
        if (abs((int)(millis() - lastTimePressed)) > 200)
        {
            printf("Pressed\r\n");
            buttonPressed = true;
            lastTimePressed = millis();
        }
    }
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
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