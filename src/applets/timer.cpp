#include "timer.h"
#include "system.h" // Contains BUZZER_PIN, matrix, rotationInput0, buttonPressed0 (assumed for click handling)
#include "digits.h" // For draw2DigitBigNumber, drawDigitBig
#include "letters.h"  // For drawString

// --- START OF CONFIGURATION AND GLOBALS ---

// MIN_TO_MS macro from 7segtimer.cpp
#define MIN_TO_MS(x) (long)((long)x * 60L * 1000L)

// Core timer variables
long timeLeft = MIN_TO_MS(5);      // Current time remaining in milliseconds (changed from int to long)
unsigned long startTimestamp = 0;  // For calculating elapsed time
uint16_t digitColor;               // Initialized in setup, color for the time display

// Display settings for LED matrix
const int TIMER_Y_POS = 8;             // Y position for time display (from original timer.cpp)
const int DIGIT_PAIR_WIDTH = 15;       // ASSUMED width of draw2DigitBigNumber output (e.g., "HH" or "MM")
const int COLON_WIDTH = 3;             // ASSUMED width of drawDigitBig(11, ...) for colon
const int TIME_DISPLAY_WIDTH = DIGIT_PAIR_WIDTH + COLON_WIDTH + DIGIT_PAIR_WIDTH; // e.g., HH:MM is 15+3+15 = 33px
const int TIME_DISPLAY_X_OFFSET = (64 - TIME_DISPLAY_WIDTH) / 2; // Assuming 64px wide matrix for centering

// Display mode
typedef enum {
    HOURS_MINUTES, // HH:MM
    MINUTES_SECONDS // MM:SS
} DisplayMode_t;
DisplayMode_t currentDisplayMode = HOURS_MINUTES;

// Alarm and sound variables
bool silenceAlarm = true;          // True if alarm sound should be suppressed
int lastDisplayNumComparable = 0;  // Tracks the displayed number (HHMM or MMSS) to control beeps on change
long toneOffset = 0;               // For rising pitch alarm sound
unsigned long toneOffsetTimer = 0; // Timer for adjusting toneOffset
const unsigned long maxAlarmLength = MIN_TO_MS(2); // Auto-silence alarm after 2 minutes
unsigned long alarmStartedTimestamp = 0; // Timestamp when alarm (timeLeft == 0) first started
bool doAlarm = false;              // True if timeLeft is 0 and alarm should be active
bool showOffMessage = false;       // True if "OFF" should be displayed (after click when timer is 0)

// Assumed global from system context for button click (e.g., set by an ISR or main polling loop)
// extern volatile bool buttonPressed0; // If not available, click logic needs to be adapted

// --- END OF CONFIGURATION AND GLOBALS ---

void timerSetup(void) {
    pinMode(BUZZER_PIN, OUTPUT);    // BUZZER_PIN is defined in system.h
    digitColor = matrix->color565(255, 255, 255); // Set default color for digits

    //timeLeft = MIN_TO_MS(5);        // Initial time
    startTimestamp = millis();
    
    silenceAlarm = true;            // Alarm is initially silent
    doAlarm = false;
    showOffMessage = false;

    // Determine initial display mode and lastDisplayNumComparable
    long hours_init = timeLeft / 3600000L;
    long minutes_init = (timeLeft % 3600000L) / 60000L;
    long seconds_init = (timeLeft % 60000L) / 1000L;

    if (hours_init >= 1) {
        currentDisplayMode = HOURS_MINUTES;
        lastDisplayNumComparable = (int)(hours_init * 100 + minutes_init);
    } else {
        currentDisplayMode = MINUTES_SECONDS;
        lastDisplayNumComparable = (int)(minutes_init * 100 + seconds_init);
    }
}

void timerLoop(void) {
    unsigned long currentTime = millis();

    // 1. Handle Knob Input (Time Adjustment)
    if (rotationInput0 != 0) {
        long timeToAdjustByMs = MIN_TO_MS(1); // Default step: 1 minute
        if (timeLeft > MIN_TO_MS(10)) {         // If > 10 min, step is 5 minutes
            timeToAdjustByMs = MIN_TO_MS(5);
        } else if (timeLeft < 5000L) {          // If < 5 sec, step is 1 second
            timeToAdjustByMs = 1000L;
        } else if (timeLeft < MIN_TO_MS(1)) {   // If < 1 min (and not < 5sec), step is 10 seconds
            timeToAdjustByMs = 10000L;
        }

        long effective_increment = (long)rotationInput0;
        if (effective_increment != 0) {
            // Accelerated adjustment: mimics 7segtimer's `inc * abs(inc)^3`
            // Assuming positive rotationInput0 means increase time
            long accelerated_knob_value = effective_increment * abs(effective_increment) * abs(effective_increment) * abs(effective_increment);
            timeLeft += timeToAdjustByMs * accelerated_knob_value;
        }
        
        rotationInput0 = 0; // Consume rotation input

        if (timeLeft > 5000L) { // If time adjusted to be > 5s, ensure alarm will sound when it runs out
            silenceAlarm = false;
        }
        showOffMessage = false; // If time is adjusted, no longer show "OFF"
        doAlarm = false;        // Timer is running, so alarm is off
    }

    // Clamp timeLeft: minimum 0, maximum e.g., 99h 59m
    if (timeLeft < 0L) timeLeft = 0L;
    if (timeLeft > MIN_TO_MS(99 * 60 + 59)) timeLeft = MIN_TO_MS(99 * 60 + 59);

    // 2. Handle Button Click (Assumes 'buttonPressed0' is set by the system)
    if (buttonPressed0) {
        if (timeLeft <= 0L) { // Only if timer has already run out
            silenceAlarm = true;
            showOffMessage = true; // Request to show "OFF" message
        }
        // else: Click while timer is running could pause/resume, but not specified.
        buttonPressed0 = false; // Consume the click event
    }

    // 3. Time Countdown
    long timeElapsed = currentTime - startTimestamp;
    
    if (!doAlarm) { // Only count down if timer is actually running (timeLeft > 0 initially)
        timeLeft -= timeElapsed;
    }
    
    if (timeLeft <= 0L) {
        timeLeft = 0L;
        if (!doAlarm) { // Transitioning to alarm state for the first time
            doAlarm = true;
            // If timer runs down naturally, alarm should sound unless user explicitly silenced it via "OFF"
            if (!showOffMessage) {
                silenceAlarm = false;
            }
            toneOffset = 0; // Reset pitch offset for alarm
            alarmStartedTimestamp = currentTime; // Log when alarm started for auto-silence
        }
    } else { // timeLeft > 0
        if (doAlarm) { // If it was alarming but time was added
          doAlarm = false;
          noTone(BUZZER_PIN); // Stop alarm sound immediately
        }
    }

    // 4. Calculate hours, minutes, seconds for display
    long hours = timeLeft / 3600000L;
    long minutes = (timeLeft % 3600000L) / 60000L;
    long seconds = (timeLeft % 60000L) / 1000L;

    // 5. Determine Display Mode (HH:MM or MM:SS)
    if (hours < 1 && timeLeft < MIN_TO_MS(60)) { // Show MM:SS if less than 1 hour total
        currentDisplayMode = MINUTES_SECONDS;
    } else {
        currentDisplayMode = HOURS_MINUTES;
    }

    // 6. Calculate currentDisplayNumComparable for beep logic (integer like HHMM or MMSS)
    int currentDisplayNumComparable_now;
    if (currentDisplayMode == HOURS_MINUTES) {
        currentDisplayNumComparable_now = (int)(hours * 100 + minutes);
    } else {
        currentDisplayNumComparable_now = (int)(minutes * 100 + seconds);
    }

    // 7. Clear screen before drawing
    matrix->fillScreen(matrix->color565(0, 0, 0));

    // 8. Handle Alarm State (Sound and Visuals for timeLeft == 0)
    if (doAlarm) {
        if (silenceAlarm) {
            noTone(BUZZER_PIN);
            if (showOffMessage) {
                // Display " OFF" - Adjust X, Y position as needed for your font/display
                // Assuming 6px char width for drawString and 8px height
                drawString((char*)" OFF", 4, (64 - 4*6)/2, (32-8)/2 + 4, digitColor);
            } else {
                // Alarm auto-silenced or was never started loudly; display 00:00
                draw2DigitBigNumber(0, TIME_DISPLAY_X_OFFSET, TIMER_Y_POS, digitColor);
                drawDigitBig(11, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH, TIMER_Y_POS, digitColor); // Colon
                draw2DigitBigNumber(0, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH + COLON_WIDTH, TIMER_Y_POS, digitColor);
            }
        } else { // Alarm is sounding
            // Alternating tone, similar to 7segtimer
            if ((currentTime / 50) % 2 == 0) {
                tone(BUZZER_PIN, (unsigned int)(toneOffset + 80));
            } else {
                tone(BUZZER_PIN, (unsigned int)(toneOffset + 80 + 96));
            }
            // Increase pitch periodically
            if (currentTime - toneOffsetTimer >= 5000) {
                toneOffsetTimer = currentTime;
                toneOffset += 400;
                if (toneOffset > 1600) {
                    toneOffset = 0;
                }
            }

            // "TIMES UP" flashing text (adapted from original timer.cpp)
            // Adjust X, Y position for "TIMES UP" (8 chars)
            // Using original timer.cpp's y=16, x=4, assuming it was somewhat centered.
            // A more calculated X: (64 - 8 * approx_char_width) / 2
            if ((currentTime % 500) < 250) {
                matrix->fillScreen(matrix->color565(255, 255, 255)); // Flash screen white
                drawString((char*)"TIMES UP", 8, 4, 16, matrix->color565(0,0,0)); // Black text
            } else {
                // Screen is already black from fillScreen or previous white flash ended
                drawString((char*)"TIMES UP", 8, 4, 16, matrix->color565(255,0,0)); // Red text
            }

            // Auto-silence after maxAlarmLength
            if (currentTime - alarmStartedTimestamp > maxAlarmLength) {
                silenceAlarm = true;
                // showOffMessage remains false, so it will display 00:00 after auto-silence
            }
        }
    } else { // 9. Timer Running (Not doAlarm, timeLeft > 0)
        showOffMessage = false; // Timer is running, so not in "OFF" state

        // Pre-alarm beeps, only if displayed time value changed
        if (currentDisplayNumComparable_now != lastDisplayNumComparable) {
            if (timeLeft < 1000L && timeLeft > 0L) {        // Last second warning (e.g., 00:01 -> 00:00)
                tone(BUZZER_PIN, 1200, 500); // freq, duration_ms
            } else if (timeLeft < 10000L && timeLeft > 0L) { // < 10 seconds warning
                tone(BUZZER_PIN, 300, 50);
            } else if (timeLeft < 30000L && timeLeft > 0L) { // < 30 seconds warning
                tone(BUZZER_PIN, 900, 10);
            } else {
                noTone(BUZZER_PIN); // If no specific warning, ensure buzzer is off
            }
        }

        // Display Time (HH:MM or MM:SS)
        if (currentDisplayMode == HOURS_MINUTES) {
            draw2DigitBigNumber((int)hours, TIME_DISPLAY_X_OFFSET, TIMER_Y_POS, digitColor);
            drawDigitBig(11, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH, TIMER_Y_POS, digitColor); // Colon
            draw2DigitBigNumber((int)minutes, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH + COLON_WIDTH, TIMER_Y_POS, digitColor);
        } else { // MINUTES_SECONDS
            draw2DigitBigNumber((int)minutes, TIME_DISPLAY_X_OFFSET, TIMER_Y_POS, digitColor);
            drawDigitBig(11, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH, TIMER_Y_POS, digitColor); // Colon
            draw2DigitBigNumber((int)seconds, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH + COLON_WIDTH, TIMER_Y_POS, digitColor);
        }
    }

    // 10. Flip Display Buffer
    matrix->flipDMABuffer();

    // 11. Update last known values for next iteration
    lastDisplayNumComparable = currentDisplayNumComparable_now;
    startTimestamp = currentTime; // Update startTimestamp for next frame's elapsed calculation
}
