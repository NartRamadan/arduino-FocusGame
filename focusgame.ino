#include "pitches.h"  // כוללת את ספריית הצלילים, שמכילה את התדרים עבור התווים המוזיקליים השונים

// הגדרת משתנים עבור כמות הצבעים, אורך הרצף, ופין הבאזזר
#define NUM_COLORS 4    // מספר הלדים/צבעים במשחק
#define SEQ_LENGTH 3    // אורך רצף הלדים שצריך לשחזר
#define BUZZER_PIN 5    // פין הבאזזר מחובר לפין 5

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

// הגדרת מצבי המשחק
#define STATE_START 10  // מצב התחלתי של המשחק
#define STATE_PLAY 20   // מצב משחק פעיל
#define STATE_WIN 30    // מצב זכייה במשחק
#define STATE_LOSE 40   // מצב הפסד במשחק

#define WINNING_TONE NOTE_C5   // צליל ניצחון
#define LOSING_TONE NOTE_B3    // צליל הפסד


const int btnPins[NUM_COLORS] = { BTN_BLUE, BTN_GREEN, BTN_YELLOW, BTN_RED }; // מערך של פיני הכפתורים לפי צבעים
const int ledPins[NUM_COLORS] = { LED_BLUE, LED_GREEN, LED_YELLOW, LED_RED }; // מערך של פיני הלדים לפי צבעים
const int soundFrequencies[NUM_COLORS] = { NOTE_D4, NOTE_E4, NOTE_G4, NOTE_A4 }; // מערך תדרים חדש לכל צבע

// משתנים לשמירת מצבי הכפתורים ומידע על זמני לחיצה
int btnStates[NUM_COLORS];             // מצב נוכחי של הכפתורים
int lastBtnStates[NUM_COLORS];         // מצב קודם של הכפתורים
unsigned long lastDebounceTimes[NUM_COLORS]; // זמן לחיצה אחרון לכל כפתור

// משתנים למעקב אחר ההתקדמות במשחק
int correctPresses = 0;  // ספירה של לחיצות נכונות של השחקן
int pressCount = 0;      // ספירה של כמות הלחיצות של השחקן
int gameState;           // משתנה ששומר את מצב המשחק הנוכחי
bool isRestarting = false; // האם המשחק נמצא בתהליך אתחול מחדש

// משתנים לניהול זמנים של לחיצות השחקן
unsigned long firstPressTime;    // זמן הלחיצה הראשונה של השחקן
unsigned long lastPressTime;     // זמן הלחיצה האחרונה של השחקן

// מערכים לשמירת הרצף האקראי שהמשחק יוצר והתשובות שהשחקן נותן
int randomSequence[SEQ_LENGTH];  // רצף הלדים האקראי שנוצר
int playerInputs[SEQ_LENGTH];    // הקלטים שהשחקן נותן

void initializeGame() {
  for (int i = 0; i < NUM_COLORS; i++) {
    lastBtnStates[i] = digitalRead(btnPins[i]); // שמירת המצב הנוכחי של כל כפתור
    lastDebounceTimes[i] = millis(); // אתחול זמן הדחייה של כל כפתור
    turnOffLed(i); // כיבוי כל הלדים
  }
}

void selectRandomSequence() {
  int randomNum;
  for (int i = 0; i < SEQ_LENGTH; i++) {
    randomNum = random(0, NUM_COLORS); // קבלת אינדקס אקראי
    if (!isInSequence(randomNum)) {    // בדיקה אם האינדקס כבר נמצא ברצף
      randomSequence[i] = randomNum;   // אם לא, הכנס את האינדקס לרצף
    } else {
      i--; // אם האינדקס כבר קיים, חזור אחורה וחזור על הבחירה
    }
  }
}

void setup() {
  for (int i = 0; i < NUM_COLORS; i++) {
    pinMode(ledPins[i], OUTPUT);         // הגדרת הפינים של הלדים כפלט
    pinMode(btnPins[i], INPUT_PULLUP);   // הגדרת הפינים של הכפתורים כקלט עם התנגדות פנימית
  }
  
  pinMode(BUZZER_PIN, OUTPUT); // הגדרת פין הבאזזר כפלט
  
  randomSeed(analogRead(A1)); // יצירת זרעים אקראיים לשימוש בפונקציה random(), באמצעות קריאה מהפין האנלוגי A1
  
  gameState = STATE_START; // קביעת מצב התחלתי של המשחק
}

void startGame() {
  noTone(BUZZER_PIN); // כיבוי הבאזר
  initializeGame(); // קריאה לפונקציית אתחול המשחק
  for (int i = 0; i < SEQ_LENGTH; i++) {
    randomSequence[i] = -1; // איפוס רצף הלדים האקראי
    playerInputs[i] = -1;   // איפוס קלטי השחקן
  }
  selectRandomSequence(); // בחירת רצף אקראי
  if (isRestarting) {
    delay(200); // השהייה קצרה כאשר המשחק מתחיל מחדש
  }
  displaySequence(); // הצגת הרצף לשחקן
  correctPresses = 0; // איפוס מונה הלחיצות הנכונות
  pressCount = 0;     // איפוס מונה הלחיצות
  gameState = STATE_PLAY; // מעבר למצב משחק פעיל
}

int checkButtonPress(bool isRestarting) {
  int pressedButton = -1;
  for (int i = 0; i < NUM_COLORS; i++) {
    btnStates[i] = digitalRead(btnPins[i]); // קריאת המצב הנוכחי של הכפתור
    if ((btnStates[i] == LOW) && (lastBtnStates[i] == HIGH) && (millis() - lastDebounceTimes[i] > 150)) {
      lastDebounceTimes[i] = millis(); // עדכון זמן הדחייה
      pressedButton = i; // שמירת האינדקס של הכפתור שנלחץ
      if (!isRestarting) { // אם המשחק לא נמצא באתחול מחדש
        playerInputs[pressCount] = i; // שמירת לחיצת השחקן במערך
        pressCount++; // הגדלת מונה הלחיצות
      }
    }
    lastBtnStates[i] = btnStates[i]; // שמירת המצב הנוכחי של הכפתור למעקב
  }
  return pressedButton;
}
/*
void displaySequence() {
  for (int i = 0; i < SEQ_LENGTH; i++) {
    turnOnLed(randomSequence[i]); // הדלקת כל לד ברצף
  }
  delay(1000); // השהייה של שנייה עם הלדים דולקים
  for (int i = 0; i < SEQ_LENGTH; i++) {
    turnOffLed(randomSequence[i]); // כיבוי כל הלדים
  }
  delay(500); // השהייה נוספת של חצי שנייה
  tone(BUZZER_PIN, soundFrequencies[0], 300); // הפעלת באזזר כדי להודיע על סיום הרצף
}
*/
/*
void displaySequence() {
  for (int i = 0; i < SEQ_LENGTH; i++) {
    turnOnLed(randomSequence[i]); // הדלקת כל לד ברצף
    delay(500); // השהייה בין כל לד
    turnOffLed(randomSequence[i]); // כיבוי כל לד
  }
  delay(500); // השהייה נוספת לאחר כיבוי כל הלדים
  noTone(BUZZER_PIN); // כיבוי הצליל
}
*/
void displaySequence() {
  // הדלקת כל הלדים בו-זמנית
  for (int i = 0; i < SEQ_LENGTH; i++) {
    digitalWrite(ledPins[randomSequence[i]], HIGH);
  }
  
  delay(1000); // השהייה של שנייה עם כל הלדים דולקים
  
  // כיבוי כל הלדים
  for (int i = 0; i < SEQ_LENGTH; i++) {
    digitalWrite(ledPins[randomSequence[i]], LOW);
  }
  
  delay(500); // השהייה נוספת של חצי שנייה לאחר כיבוי כל הלדים
  noTone(BUZZER_PIN); // כיבוי הצליל
}


bool isInSequence(int number) {
  for (int i = 0; i < SEQ_LENGTH; i++) {
    if (randomSequence[i] == number) {
      return true; // אם המספר נמצא ברצף, החזר true
    }
  }
  return false; // אם לא, החזר false
}

void playGame() {
  int btn = checkButtonPress(false); // בדיקת לחיצות כפתורים
  if (btn != -1) {
    if (pressCount == 1) {
      firstPressTime = millis(); // התחלת מדידת זמן הלחיצות של השחקן
    }
  }
  if (pressCount == SEQ_LENGTH) { // אם השחקן לחץ על כל הכפתורים הנדרשים
    lastPressTime = millis(); // שמירת זמן הלחיצה האחרונה
    if (playerInputs[0] != playerInputs[1] || playerInputs[0] != playerInputs[2] || playerInputs[1] != playerInputs[2]) {
      int i = 0;
      while (i < pressCount) {
        correctPresses += isInSequence(playerInputs[i++]); // בדיקה אם הלחיצות תואמות לרצף הנכון
      }
      if (correctPresses == SEQ_LENGTH && lastPressTime - firstPressTime <= 3000) {
        gameState = STATE_WIN; // אם כל הלחיצות נכונות ובוצעו תוך 3 שניות, השחקן זכה
      } else {
        gameState = STATE_LOSE; // אחרת, השחקן הפסיד
      }
    } else {
      gameState = STATE_LOSE; // אם הלחיצות כוללות תווים חוזרים, השחקן הפסיד
    }
  }
}

void endGame() {
  if (gameState == STATE_WIN) {
    digitalWrite(LED_GREEN, HIGH);   // הדלקת לד ירוק
    tone(BUZZER_PIN, WINNING_TONE, 1000); // צליל ניצחון
  } else {
    digitalWrite(LED_RED, HIGH);     // הדלקת לד אדום
    tone(BUZZER_PIN, LOSING_TONE, 1000);  // צליל הפסד
  }
  delay(2000); // השהייה של 2 שניות
  digitalWrite(LED_GREEN, LOW);    // כיבוי לד ירוק
  digitalWrite(LED_RED, LOW);      // כיבוי לד אדום
  noTone(BUZZER_PIN);              // כיבוי הבאזר
  gameState = STATE_START; // אתחול מצב המשחק להתחלה מחדש
  isRestarting = true; // מציין שהמשחק באתחול מחדש
  startGame(); // התחלת המשחק מחדש
}


void loop() {
  switch (gameState) {
    case STATE_START:
      startGame(); // מעבר למצב משחק פעיל
      break;
    case STATE_PLAY:
      playGame(); // הפעלת המשחק
      break;
    case STATE_WIN:
    case STATE_LOSE:
      endGame(); // סיום המשחק והצגת התוצאה
      break;
  }
}

void turnOnLed(int ledIndex) {
  digitalWrite(ledPins[ledIndex], HIGH); // הדלקת הלד
  tone(BUZZER_PIN, soundFrequencies[ledIndex], 300); // הפעלת באזזר תואם לצבע הלד
  delay(300); // השהייה קצרה
}


void turnOffLed(int ledIndex) {
  digitalWrite(ledPins[ledIndex], LOW); // כיבוי הלד
  delay(150); // השהייה קצרה נוספת
}
