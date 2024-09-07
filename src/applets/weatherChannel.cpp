#include "system.h"
#include "weatherChannel.h"
#include "sprites.h"
#include "digits.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

enum State
{
    STATE_FETCHING,
    STATE_DISPLAYING,
};

enum Condition
{
    CONDITION_CLEAR,
    CONDITION_CLOUDY,
    CONDITION_ERROR, 
};

// The weather data actually displayed to the user
struct WeatherDisplay
{
    int temp;
    Condition condition;
};
WeatherDisplay weatherDisplay;
State wstate = STATE_FETCHING;
unsigned long fetchTimer = 0;
unsigned long weatherGetterTimer = 0;
const uint16_t bgCol = matrix->color565(189, 234, 255);

static int findMostCommonWeatherCode(int arr[], int size) {
  int maxCount = 0;
  int mostCommon = -1;
  
  for (int i = 0; i < size; i++) {
    int count = 0;
    
    for (int j = 0; j < size; j++) {
      if (arr[j] == arr[i]) {
        count++;
      }
    }
    
    if (count > maxCount) {
      maxCount = count;
      mostCommon = arr[i];
    }
  }
  
  return mostCommon;
}

static Condition weatherCodeToCondition(int weatherCode)
{
    if (weatherCode >= 200 && weatherCode < 300)
    {
        return CONDITION_CLOUDY;
    }
    else if (weatherCode >= 300 && weatherCode < 400)
    {
        return CONDITION_CLEAR;
    }
    else
    {
        return CONDITION_ERROR;
    }
}

static WeatherDisplay acquireWeatherData()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Not connected to WiFi");
        return {0, CONDITION_ERROR};
    }
    HTTPClient http;
    // If not working, replace '/' with '%2F' in "America/Los_Angeles"
    http.begin("https://api.open-meteo.com/v1/forecast?latitude=33.88&longitude=-117.89&hourly=weather_code&daily=temperature_2m_max&temperature_unit=fahrenheit&wind_speed_unit=mph&precipitation_unit=inch&timezone=America/Los_Angeles&forecast_days=1");
    int responseCode = http.GET();
    if (responseCode != 200)
    {
        Serial.print("HTTP GET failed, error: ");
        Serial.println(responseCode);
        http.end();
        return {0, CONDITION_ERROR};
    }
    String payload = http.getString();
    http.end();
    Serial.println(payload);
    const size_t capacity = JSON_ARRAY_SIZE(24) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + 1024;
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return {0, CONDITION_ERROR};
    }
    // Extract the temperature and weather codes
    WeatherDisplay output;
    output.temp = doc["daily"]["temperature_2m_max"][0];
    // Find most common weather code in hourly report
    JsonArray jWeatherCodes = doc["hourly"]["weather_code"];
    int weatherCodes[24];
    for (int i = 0; i < 24; i++)
    {
        weatherCodes[i] = jWeatherCodes[i];
    }
    int mostCommonWeatherCode = findMostCommonWeatherCode(weatherCodes, 24);
    output.condition = weatherCodeToCondition(mostCommonWeatherCode);
    return output;
}

// x and y are top left corner
static void drawFrame(int x, int y, int w, int h, bool dark, bool drawBg)
{
    uint16_t litCol = matrix->color565(240, 240, 112);
    uint16_t shadedCol = matrix->color565(216, 120, 24);
    uint16_t bgCol = matrix->color565(200, 240, 240);
    if (dark)
    {
        litCol = matrix->color565(0, 0, 56);
        shadedCol = matrix->color565(0, 0, 56);
        bgCol = matrix->color565(88, 224, 240);
    }
    if (drawBg)
    {
        matrix->fillRect(x + 1, y + 1, w - 2, h - 2, bgCol);
    }
    matrix->drawRect(x + 2, y + 2, w - 2, h - 2, matrix->color565(16, 104, 168)); // Shadow
    matrix->drawRect(x + 0, y + 0, w - 1, h - 1, litCol); // Light
    matrix->drawRect(x + 1, y + 1, w - 1, h - 1, shadedCol); // Shaded
}

static void drawFrameCentered(int x, int y, int w, int h, bool drawBg)
{
    bool beDark = false;
    if (w < 0)
    {
        w = -w;
        beDark = true;
    }
    drawFrame(x - w / 2, y - h / 2, w, h, beDark, drawBg);
}

static void fetchUpdate()
{
    const unsigned long FETCH_LENGTH = 2000;
    if (millis() - fetchTimer > FETCH_LENGTH)
    {
        wstate = STATE_DISPLAYING;
        fetchTimer = millis();
    }
    float amplitude = 36.0;
    float frameW = amplitude * sin(millis() / 100.0);

    drawFrameCentered(19, 15, frameW, 30, true);
}

static void drawNumber(int number, int x, int y, uint16_t color)
{
    if (number == 0) {
        drawDigitBig(0, x, y, color);
        return;
    }

    int numDigits = (int)log10(number) + 1; // Calculate the number of digits
    int startX = x;

    for (int i = numDigits - 1; i >= 0; --i) {
        int digit = (number / (int)pow(10, i)) % 10; // Extract the current digit
        drawDigitBig(digit, startX, y, color);
        startX += DIGITBIG_WIDTH; // Move to the next position
    }
}

static void drawTemperature(int temp, int x, int y, uint16_t color)
{
    drawNumber(temp, x, y, color);
    drawDigitBig(10, 58, 6, color);
}

static void reset(bool forceFetchNewWeatherData)
{
    wstate = STATE_FETCHING; 
    // weatherDisplay.temp = random(50, 100);
    // weatherDisplay.condition = random(0, 2) == 0 ? CONDITION_CLEAR : CONDITION_CLOUDY;
    fetchTimer = millis();
    const unsigned long millisInDay = 86400000; 

    if (forceFetchNewWeatherData || (millis() - weatherGetterTimer > millisInDay))
    {
        Serial.println("Fetching new weather data");
        weatherDisplay = acquireWeatherData();
        weatherGetterTimer = millis();
    }
}

void weatherChannelSetup(void)
{
    reset(false);
}

void weatherChannelLoop(void)
{
    if (buttonPressed0)
    {
        reset(true);
        buttonPressed0 = false;
    }
    matrix->fillScreen(bgCol);
    switch (wstate)
    {
        case STATE_FETCHING:
            fetchUpdate();
            break;
        case STATE_DISPLAYING:
            drawFrame(1, 2, 36, 30, false, false);
            drawSprite(matrix, 2, 3, sunSprite);
            if (millis() - fetchTimer < 2000)
            {
                if ((millis() / 100) % 2 == 0)
                {
                    drawTemperature(weatherDisplay.temp, 39, 6, matrix->color565(8, 56, 136));
                }
            }
            else
            {
                drawTemperature(weatherDisplay.temp, 39, 6, matrix->color565(8, 56, 136));
            }
            break;
    }
    matrix->flipDMABuffer();
}