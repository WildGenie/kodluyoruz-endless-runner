#include <LiquidCrystal.h>

const int LCD_WIDTH = 16;
const int BUTTON_PIN = 2;
const int MAX_SCORE_DIGITS = 5;
const char EMPTY_CHAR = 0;
const int FRAME_DELAY_MS = 100;
const int INTRO_DELAY_MS = 1000;

enum Pattern
{
  INTRO_1,
  INTRO_2,
  INTRO_PRESS_START,
  PATTERN_1,
  PATTERN_2,
  PATTERN_3,
  PATTERN_4,
  PATTERN_5,
  PATTERN_6,
  PATTERN_7,
  PATTERN_8,
  PATTERN_9
};

LiquidCrystal lcd(12, 11, 5, 4, 3, 7);

char topRow[LCD_WIDTH + 1], bottomRow[LCD_WIDTH + 1];
bool buttonPressed = false, gameStarted = false;
byte currentRow = 1, nextRow1 = EMPTY_CHAR, nextRow2 = EMPTY_CHAR, currentPattern = 0, score = 0;
unsigned long frameCount = 0, lastFrameTime = 0;

void initializeGame()
{
  static byte customChars[] = {
      B01100,
      B01100,
      B00000,
      B01110,
      B11100,
      B01100,
      B11010,
      B10011,
      B01100,
      B01100,
      B00000,
      B01100,
      B01100,
      B01100,
      B01100,
      B01110,
      B01100,
      B01100,
      B00000,
      B11110,
      B01101,
      B11111,
      B10000,
      B00000,
      B11110,
      B01101,
      B11111,
      B10000,
      B00000,
      B00000,
      B00000,
      B00000,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B00011,
      B00011,
      B00011,
      B00011,
      B00011,
      B00011,
      B00011,
      B00011,
      B11000,
      B11000,
      B11000,
      B11000,
      B11000,
      B11000,
      B11000,
      B11000,
  };
  for (int i = 0; i < 7; ++i)
    lcd.createChar(i + 1, &customChars[i * 8]);
  for (int i = 0; i < LCD_WIDTH; ++i)
    topRow[i] = 0, bottomRow[i] = 0;
}

void advanceRow(char *row, byte nextChar)
{
  for (int i = 0; i < LCD_WIDTH; ++i)
  {
    char currentChar = row[i];
    char next = (i == LCD_WIDTH - 1) ? nextChar : row[i + 1];
    switch (currentChar)
    {
    case EMPTY_CHAR:
      row[i] = (next == 2) ? 3 : 0;
      break;
    case 1:
      row[i] = (next == 0) ? 2 : 1;
      break;
    case 3:
      row[i] = 1;
      break;
    case 2:
      row[i] = 0;
      break;
    }
  }
}

bool updateDisplay(byte pattern, char *topRow, char *bottomRow, unsigned int score)
{
  bool collision = false;
  char topChar = topRow[2], bottomChar = bottomRow[2];
  byte topCharIndex = 0, bottomCharIndex = 0;
  switch (pattern)
  {
  case PATTERN_1:
    bottomCharIndex = 1;
    break;
  case PATTERN_2:
    bottomCharIndex = 2;
    break;
  case PATTERN_3:
  case PATTERN_8:
    bottomCharIndex = 3;
    break;
  case PATTERN_4:
  case PATTERN_9:
    topCharIndex = 3;
    bottomCharIndex = 3;
    break;
  case PATTERN_5:
  case PATTERN_6:
  case PATTERN_7:
    topCharIndex = 3;
    break;
  }
  if (topCharIndex != 0)
  {
    topRow[2] = topCharIndex;
    collision = (topChar == 0) ? false : true;
  }
  if (bottomCharIndex != 0)
  {
    bottomRow[2] = bottomCharIndex;
    collision |= (bottomChar == 0) ? false : true;
  }
  byte digits = (score > 9999) ? MAX_SCORE_DIGITS : (score > 999) ? 4
                                                : (score > 99)    ? 3
                                                : (score > 9)     ? 2
                                                                  : 1;
  topRow[LCD_WIDTH] = '\0';
  bottomRow[LCD_WIDTH] = '\0';
  char temp = topRow[LCD_WIDTH - digits];
  topRow[LCD_WIDTH - digits] = '\0';
  lcd.setCursor(0, 0);
  lcd.print(topRow);
  topRow[LCD_WIDTH - digits] = temp;
  lcd.setCursor(0, 1);
  lcd.print(bottomRow);
  lcd.setCursor(LCD_WIDTH - digits, 0);
  lcd.print(score);
  topRow[2] = topChar;
  bottomRow[2] = bottomChar;
  return collision;
}

void setup()
{
  lcd.begin(LCD_WIDTH, 2);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void intro()
{
  updateDisplay(currentPattern, topRow, bottomRow, frameCount / 10);
  if (currentPattern == INTRO_PRESS_START)
  {
    lcd.setCursor(0, 0);
    lcd.print("Press Start");
  }
  delay(INTRO_DELAY_MS);
  currentPattern = (currentPattern == INTRO_1) ? INTRO_2 : INTRO_1;
}

void game()
{
  advanceRow(bottomRow, currentRow == 2 ? 2 : 0);
  advanceRow(topRow, currentRow == 1 ? 2 : 0);
  if (--currentRow == 0)
  {
    currentRow = random(3) == 0 ? 1 : 2;
    currentRow = currentRow == 1 ? 2 + random(10) : 10 + random(10);
  }
  if (buttonPressed && currentPattern <= INTRO_PRESS_START)
  {
    currentPattern = PATTERN_3;
    buttonPressed = false;
  }
  if (updateDisplay(currentPattern, topRow, bottomRow, score))
  {
    gameStarted = false;
  }
  else
  {
    switch (currentPattern)
    {
    case PATTERN_4:
    case PATTERN_9:
      currentPattern = PATTERN_1;
      break;
    case PATTERN_5:
    case PATTERN_6:
    case PATTERN_7:
      if (bottomRow[2] != 0)
        currentPattern = PATTERN_3;
      break;
    case PATTERN_3:
      if (bottomRow[2] == 0)
        currentPattern = PATTERN_7;
      break;
    case PATTERN_8:
      currentPattern = PATTERN_5;
      break;
    default:
      ++currentPattern;
      break;
    }
    ++score;
    ++frameCount;
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastFrameTime;
    if (elapsedTime < FRAME_DELAY_MS)
    {
      delay(FRAME_DELAY_MS - elapsedTime);
    }
    lastFrameTime = currentTime;
  }
}

void loop()
{
  if (!gameStarted)
  {
    intro();
    if (buttonPressed)
    {
      initializeGame();
      currentPattern = PATTERN_1;
      gameStarted = true;
      buttonPressed = false;
      frameCount = 0;
      lastFrameTime = millis();
    }
  }
  else
  {
    game();
  }
}
