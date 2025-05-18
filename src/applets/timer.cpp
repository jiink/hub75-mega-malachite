#include "timer.h"
#include "system.h" // Contains BUZZER_PIN, matrix, rotationInput0, buttonPressed0
#include "digits.h" // For draw2DigitBigNumber, drawDigitBig
#include "letters.h"  // For drawString

// --- START OF CONFIGURATION AND GLOBALS ---

#define MIN_TO_MS(x) (long)((long)x * 60L * 1000L)

long timeLeft = MIN_TO_MS(5);
unsigned long startTimestamp = 0;
uint16_t digitColor;
uint16_t digitGreyedColor;

const int TIMER_Y_POS = 8;
// Assuming draw2DigitBigNumber("00") takes 15px width, and drawDigitBig(11,...) for colon takes 3px
const int DIGIT_PAIR_WIDTH = 15;
const int COLON_WIDTH = 3;
// For "HH:MM" or "MM:SS", total width is 15 (HH) + 3 (:) + 15 (MM) = 33px
const int TIME_DISPLAY_WIDTH = DIGIT_PAIR_WIDTH + COLON_WIDTH + DIGIT_PAIR_WIDTH;
// Center it on a 64px wide matrix: (64 - 33) / 2 = 15 (integer division)
const int TIME_DISPLAY_X_OFFSET = (64 - TIME_DISPLAY_WIDTH) / 2;


typedef enum {
    HOURS_MINUTES,
    MINUTES_SECONDS
} DisplayMode_t;
DisplayMode_t currentDisplayMode = HOURS_MINUTES;

bool silenceAlarm = true;
int lastDisplayNumComparable = 0;
long toneOffset = 0;
unsigned long toneOffsetTimer = 0;
const unsigned long maxAlarmLength = MIN_TO_MS(2);
unsigned long alarmStartedTimestamp = 0;
bool doAlarm = false;
bool showOffMessage = false;

// extern volatile bool buttonPressed0; // Assumed from system.h or context

// --- END OF CONFIGURATION AND GLOBALS ---

void timerSetup(void) {
    if (nightMode) {
        digitColor = matrix->color565(100, 20, 50);
        digitGreyedColor = matrix->color565(50, 0, 0);
    } else {
        digitColor = matrix->color565(255, 255, 255);
        digitGreyedColor = matrix->color565(100, 20, 50);
    }
    startTimestamp = millis();
    
    silenceAlarm = true;
    doAlarm = false;
    showOffMessage = false;

    // Initial display mode and comparable number (no change needed here)
    long hours_init = timeLeft / 3600000L;
    long minutes_init = (timeLeft % 3600000L) / 60000L;
    if (hours_init >= 1) {
        currentDisplayMode = HOURS_MINUTES;
        lastDisplayNumComparable = (int)(hours_init * 100 + minutes_init);
    } else {
        currentDisplayMode = MINUTES_SECONDS;
        lastDisplayNumComparable = (int)(minutes_init * 100 + ( (timeLeft % 60000L) / 1000L) );
    }
}

void timerLoop(void) {
    unsigned long currentTime = millis();

    // 1. Handle Knob Input (Time Adjustment) - STEP-BY-STEP PROCESSING
    while (rotationInput0 != 0) {
        long current_timeLeft_before_step = timeLeft; // Use for determining adjustment scale

        long step_direction = 0;
        if (rotationInput0 > 0) {
            step_direction = 1; // Increase time
            rotationInput0--;
        } else { // rotationInput0 < 0
            step_direction = -1; // Decrease time
            rotationInput0++;
        }

        // Determine timeToAdjustByMs based on timeLeft *before* this single step
        long timeToAdjustByMs_this_step = MIN_TO_MS(1); // Default 1 minute
        if (current_timeLeft_before_step > MIN_TO_MS(10)) {
            timeToAdjustByMs_this_step = MIN_TO_MS(5);  // 5 minute steps if > 10 min total
        } else if (current_timeLeft_before_step < 5000L && current_timeLeft_before_step > 0) { // between 0 and 5s (exclusive of 0 for setting)
            timeToAdjustByMs_this_step = 1000L;         // 1 second steps
        } else if (current_timeLeft_before_step < MIN_TO_MS(1) && current_timeLeft_before_step > 0) { // between 0 and 1m (exclusive of 0)
             // This also covers the 5s to 1m range not caught by the previous condition
            timeToAdjustByMs_this_step = 10000L;        // 10 second steps
        }
        // If timeLeft is 0 and knob is turned, default to 1-minute steps to start setting time
        else if (current_timeLeft_before_step == 0 && step_direction > 0) {
             timeToAdjustByMs_this_step = MIN_TO_MS(1);
        }


        // Apply adjustment. The acceleration term (abs(inc)^3) from 7segtimer
        // simplifies to 1 if inc is +/-1, so we just use step_direction.
        timeLeft += timeToAdjustByMs_this_step * step_direction;

        // Common logic after any successful knob adjustment for this step
        if (timeLeft > 0) { // Only unsilence if time is actually set
            if (timeLeft > 5000L || current_timeLeft_before_step == 0) { // Unsilence if significant time set or setting from zero
                 silenceAlarm = false;
            }
            showOffMessage = false;
            doAlarm = false;
        }
    } // End while (rotationInput0 != 0)


    // Clamp timeLeft: minimum 0, maximum 99h 59m
    if (timeLeft < 0L) timeLeft = 0L;
    // Max time: 99 hours, 59 minutes, 59 seconds (effectively).
    // MIN_TO_MS(99*60 + 59) is 99h59m. Add 59 seconds for full range.
    // if (timeLeft > (MIN_TO_MS(99L * 60L + 59L) + 59000L) ) {
    //     timeLeft = MIN_TO_MS(99L * 60L + 59L) + 59000L;
    // }


    // 2. Handle Button Click (Assumes 'buttonPressed0' is set by the system)
    if (buttonPressed0) {
        if (timeLeft <= 0L) {
            silenceAlarm = true;
            showOffMessage = true;
        }
        buttonPressed0 = false;
    }

    // 3. Time Countdown
    long timeElapsed = currentTime - startTimestamp;
    
    if (!doAlarm && timeLeft > 0) { // Only count down if timer is running and not already alarming
        timeLeft -= timeElapsed;
    }
    
    if (timeLeft <= 0L) {
        timeLeft = 0L;
        if (!doAlarm) {
            doAlarm = true;
            if (!showOffMessage) { // Don't start alarm sound if "OFF" was just requested
                silenceAlarm = false;
            }
            toneOffset = 0;
            alarmStartedTimestamp = currentTime;
        }
    } else {
        if (doAlarm) { // If it was alarming but time was added by knob
          doAlarm = false;
          noTone(BUZZER_PIN);
        }
    }
    startTimestamp = currentTime; // Update startTimestamp for next frame's elapsed calculation


    // 4. Calculate hours, minutes, seconds for display
    long hours = timeLeft / 3600000L;
    long minutes = (timeLeft % 3600000L) / 60000L;
    long seconds = (timeLeft % 60000L) / 1000L;

    // 5. Determine Display Mode (HH:MM or MM:SS) - Simplified
    if (hours < 1) {
        currentDisplayMode = MINUTES_SECONDS;
    } else {
        currentDisplayMode = HOURS_MINUTES;
    }

    // 6. Calculate currentDisplayNumComparable for beep logic
    int currentDisplayNumComparable_now;
    if (currentDisplayMode == HOURS_MINUTES) {
        currentDisplayNumComparable_now = (int)(hours * 100 + minutes);
    } else {
        currentDisplayNumComparable_now = (int)(minutes * 100 + seconds);
    }

    // 7. Clear screen
    matrix->fillScreen(matrix->color565(0, 0, 0));

    // 8. Handle Alarm State or Display Time
    if (doAlarm) {
        if (silenceAlarm) {
            noTone(BUZZER_PIN);
            if (showOffMessage) {
                drawString((char*)" OFF", 4, (64 - 4*6)/2, (32-8)/2 + 4, digitColor); // Adjust X,Y, font size
            } else {
                draw2DigitBigNumber(0, TIME_DISPLAY_X_OFFSET, TIMER_Y_POS, digitColor);
                drawDigitBig(11, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH, TIMER_Y_POS, digitColor);
                draw2DigitBigNumber(0, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH + COLON_WIDTH, TIMER_Y_POS, digitColor);
            }
        } else { // Alarm is sounding
            if ((currentTime / 50) % 2 == 0) {
                tone(BUZZER_PIN, (unsigned int)(toneOffset + 80));
            } else {
                tone(BUZZER_PIN, (unsigned int)(toneOffset + 80 + 96));
            }
            if (currentTime - toneOffsetTimer >= 5000) {
                toneOffsetTimer = currentTime;
                toneOffset += 400;
                if (toneOffset > 1600) toneOffset = 0;
            }

            if ((currentTime % 500) < 250) {
                matrix->fillScreen(matrix->color565(255, 255, 255));
                drawString((char*)"TIMES UP", 8, 4, 16, matrix->color565(0,0,0));
            } else {
                drawString((char*)"TIMES UP", 8, 4, 16, matrix->color565(255,0,0));
            }

            if (currentTime - alarmStartedTimestamp > maxAlarmLength) {
                silenceAlarm = true;
            }
        }
    } else { // Timer Running (Not doAlarm, timeLeft > 0)
        // Pre-alarm beeps
        if (currentDisplayNumComparable_now != lastDisplayNumComparable) {
            if (timeLeft < 1000L && timeLeft > 0L) {
                tone(BUZZER_PIN, 1200, 500);
            } else if (timeLeft < 10000L && timeLeft > 0L) {
                tone(BUZZER_PIN, 300, 50);
            } else if (timeLeft < 30000L && timeLeft > 0L) {
                tone(BUZZER_PIN, 900, 10);
            } else {
                noTone(BUZZER_PIN);
            }
        }

        // Display Time
        int val1, val2;
        if (currentDisplayMode == HOURS_MINUTES) {
            val1 = (int)hours;
            val2 = (int)minutes;
        } else { // MINUTES_SECONDS
            val1 = (int)minutes;
            val2 = (int)seconds;
        }
        uint16_t hoursColor = hours > 0 ? digitColor : digitGreyedColor;
        draw2DigitBigNumber(hours, TIME_DISPLAY_X_OFFSET - 2, TIMER_Y_POS, hoursColor); // hours
        drawDigitBig(11, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH - 2, TIMER_Y_POS, digitGreyedColor); // Colon
        uint16_t minutesColor = (hours == 0 && minutes == 0) ? digitGreyedColor : digitColor;
        draw2DigitBigNumber(minutes, TIME_DISPLAY_X_OFFSET + DIGIT_PAIR_WIDTH + COLON_WIDTH + 2, TIMER_Y_POS, minutesColor); // minutes
        uint16_t secondsColor = (hours == 0 && minutes == 0) ? digitColor : digitGreyedColor; 
        draw2DigitNumber(seconds, 28, 23, secondsColor);
    }

    // 10. Flip Display Buffer
    matrix->flipDMABuffer();

    // 11. Update last known values
    lastDisplayNumComparable = currentDisplayNumComparable_now;
    // startTimestamp is now updated right after timeElapsed calculation
}
