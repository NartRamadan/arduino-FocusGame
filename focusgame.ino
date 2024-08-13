#include "pitches.h"  // כוללת את ספריית הצלילים, שמכילה את התדרים עבור התווים המוזיקליים השונים

// הגדרת פינים ללדים
#define LED_RED 2       // פין לד אדום מחובר לפין 2
#define LED_YELLOW 3    // פין לד צהוב מחובר לפין 3
#define LED_GREEN 4     // פין לד ירוק מחובר לפין 4
#define LED_BLUE 12     // פין לד כחול מחובר לפין 12

// הגדרת פינים לכפתורים
#define BTN_RED 6       // פין כפתור אדום מחובר לפין 6
#define BTN_YELLOW 9    // פין כפתור צהוב מחובר לפין 9
#define BTN_GREEN 10    // פין כפתור ירוק מחובר לפין 10
#define BTN_BLUE 11     // פין כפתור כחול מחובר לפין 11

// הגדרת משתנים עבור כמות הצבעים, אורך הרצף, ופין הבאזזר
#define NUM_COLORS 4    // מספר הלדים/צבעים במשחק
#define SEQ_LENGTH 3    // אורך רצף הלדים שצריך לשחזר
#define BUZZER_PIN 5    // פין הבאזזר מחובר לפין 5

// הגדרת מצבי המשחק
#define STATE_START 10  // מצב התחלתי של המשחק
#define STATE_PLAY 20   // מצב משחק פעיל
#define STATE_WIN 30    // מצב זכייה במשחק
#define STATE_LOSE 40   // מצב הפסד במשחק

const int btnPins[NUM_COLORS] = { BTN_BLUE, BTN_GREEN, BTN_YELLOW, BTN_RED }; // מערך של פיני הכפתורים לפי צבעים
const int ledPins[NUM_COLORS] = { LED_BLUE, LED_GREEN, LED_YELLOW, LED_RED }; // מערך של פיני הלדים לפי צבעים
const int soundFrequencies[NUM_COLORS] = { NOTE_C4, NOTE_G3, NOTE_A3, NOTE_F2 }; // מערך של תדרים לכל צבע

// משתנים לשמירת מצבי הכפתורים ומידע על זמני לחיצה
int btnStates[NUM_COLORS];             // מצב נוכחי של הכפתורים
int lastBtnStates[NUM_COLORS];         // מצב קודם של הכפתורים
unsigned long lastDebounceTimes[NUM_COLORS]; // זמן לחיצה אחרון לכל כפתור

// מערכים לשמירת הרצף האקראי שהמשחק יוצר והתשובות שהשחקן נותן
int randomSequence[SEQ_LENGTH];  // רצף הלדים האקראי שנוצר
int playerInputs[SEQ_LENGTH];    // הקלטים שהשחקן נותן

// משתנים לניהול זמנים של לחיצות השחקן
unsigned long firstPressTime;    // זמן הלחיצה הראשונה של השחקן
unsigned long lastPressTime;     // זמן הלחיצה האחרונה של השחקן

// משתנים למעקב אחר ההתקדמות במשחק
int correctPresses = 0;  // ספירה של לחיצות נכונות של השחקן
int pressCount = 0;      // ספירה של כמות הלחיצות של השחקן
int gameState;           // משתנה ששומר את מצב המשחק הנוכחי
bool isRestarting = false; // האם המשחק נמצא בתהליך אתחול מחדש

void setup() {
  // אתחול פינים עבור לדים וכפתורים
  for (int i = 0; i < NUM_COLORS; i++) {
    pinMode(ledPins[i], OUTPUT);         // הגדרת הפינים של הלדים כפלט
    pinMode(btnPins[i], INPUT_PULLUP);   // הגדרת הפינים של הכפתורים כקלט עם התנגדות פנימית
  }
  
  pinMode(BUZZER_PIN, OUTPUT); // הגדרת פין הבאזזר כפלט
  
  randomSeed(analogRead(A1)); // יצירת זרעים אקראיים לשימוש בפונקציה random(), באמצעות קריאה מהפין האנלוגי A1
  
  gameState = STATE_START; // קביעת מצב התחלתי של המשחק
}
