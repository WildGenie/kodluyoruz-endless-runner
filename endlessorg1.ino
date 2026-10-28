// LCD1602 to Arduino Uno connection example

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

// Constants
#define BTN_PIN 2

#define S_RUN1 1
#define S_RUN2 2
#define S_JUMP 3
#define S_JUMP_UPPER '.' // Use the '.' character for the head
#define S_JUMP_LOWER 4
#define S_TERRAIN_EMPTY ' ' // User the ' ' character
#define S_TERRAIN_SOLID 5
#define S_TERRAIN_SOLID_RIGHT 6
#define S_TERRAIN_SOLID_LEFT 7

#define HERO_HORIZONTAL_POSITION 1 // Horizontal position of hero on screen

#define TERRAIN_WIDTH 16
#define TERRAIN_EMPTY 0
#define TERRAIN_LOWER_BLOCK 1
#define TERRAIN_UPPER_BLOCK 2

#define HP_OFF 0         // Hero is invisible
#define HP_RUN_LOWER_1 1 // Hero is running on lower row (pose 1)
#define HP_RUN_LOWER_2 2 //                              (pose 2)

#define HP_JUMP_1 3  // Starting a jump
#define HP_JUMP_2 4  // Half-way up
#define HP_JUMP_3 5  // Jump is on upper row
#define HP_JUMP_4 6  // Jump is on upper row
#define HP_JUMP_5 7  // Jump is on upper row
#define HP_JUMP_6 8  // Jump is on upper row
#define HP_JUMP_7 9  // Half-way down
#define HP_JUMP_8 10 // About to land

#define HP_RUN_UPPER_1 11 // Hero is running on upper row (pose 1)
#define HP_RUN_UPPER_2 12

static char terrainUpper[TERRAIN_WIDTH + 1];
static char terrainLower[TERRAIN_WIDTH + 1];
static bool buttonPushed = false;

void setup()
{
    initializeGraphics();
    lcd.begin(16, 2);
    pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop()
{

    static byte heroPos = HP_RUN_LOWER_1;
    static byte newTerrainType = TERRAIN_EMPTY;
    static byte newTerrainDuration = 1;
    static bool playing = false;
    static bool blink = false;
    static unsigned int distance = 0;

    // Check if button is pressed
    if (digitalRead(BTN_PIN) == LOW)
    {
        buttonPushed = true;
    }

    // Show start screen if not currently playing game
    if (!playing)
    {
        drawHero((blink) ? HP_OFF : heroPos, terrainUpper, terrainLower, distance >> 3);
        if (blink)
        {
            lcd.setCursor(0, 0);
            lcd.print("Press Start");
        }
        delay(250);
        blink = !blink;
        if (buttonPushed)
        {
            initializeGraphics();
            heroPos = HP_RUN_LOWER_1;
            playing = true;
            buttonPushed = false;
            distance = 0;
        }
        return;
    }

    // Shift the terrain to the left
    advanceTerrain(terrainLower, newTerrainType == TERRAIN_LOWER_BLOCK ? S_TERRAIN_SOLID : S_TERRAIN_EMPTY);
    advanceTerrain(terrainUpper, newTerrainType == TERRAIN_UPPER_BLOCK ? S_TERRAIN_SOLID : S_TERRAIN_EMPTY);

    // Make new terrain to enter on the right
    if (--newTerrainDuration == 0)
    {
        if (newTerrainType == TERRAIN_EMPTY)
        {
            newTerrainType = (random(3) == 0) ? TERRAIN_UPPER_BLOCK : TERRAIN_LOWER_BLOCK;
            newTerrainDuration = 2 + random(10);
        }
        else
        {
            newTerrainType = TERRAIN_EMPTY;
            newTerrainDuration = 10 + random(10);
        }
    }

    // Jump if button is pressed
    if (buttonPushed)
    {
        if (heroPos <= HP_RUN_LOWER_2)
            heroPos = HP_JUMP_1;
        buttonPushed = false;
    }

    // Draw hero on screen and check for collisions
    if (drawHero(heroPos, terrainUpper, terrainLower, distance >> 3))
    {
        playing = false; // The hero collided with something. Too bad.
    }
    else
    {
        if (heroPos == HP_RUN_LOWER_2 || heroPos == HP_JUMP_8)
        {
            heroPos = HP_RUN_LOWER_1;
        }
        else if ((heroPos >= HP_JUMP_3 && heroPos <= HP_JUMP_5) && terrainLower[HERO_HORIZONTAL_POSITION] != S_TERRAIN_EMPTY)
        {
            heroPos = HP_RUN_UPPER_1;
        }
        else if (heroPos >= HP_RUN_UPPER_1 && terrainLower[HERO_HORIZONTAL_POSITION] == S_TERRAIN_EMPTY)
        {
            heroPos = HP_JUMP_5;
        }
        else if (heroPos == HP_RUN_UPPER_2)
        {
            heroPos = HP_RUN_UPPER_1;
        }
        else
        {
            ++heroPos;
        }
        ++distance;
    }

    delay(100);
}

// Create custom character LCD graphics
void initializeGraphics()
{

    static byte graphics[] = {

        // Run position 1
        B01100,
        B01100,
        B00000,
        B01110,
        B11100,
        B01100,
        B11010,
        B10011,

        // Run position 2
        B01100,
        B01100,
        B00000,
        B01100,
        B01100,
        B01100,
        B01100,
        B01110,

        // Jump
        B01100,
        B01100,
        B00000,
        B11110,
        B01101,
        B11111,
        B10000,
        B00000,

        // Jump lower
        B11110,
        B01101,
        B11111,
        B10000,
        B00000,
        B00000,
        B00000,
        B00000,

        // Ground
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,

        // Ground right
        B00011,
        B00011,
        B00011,
        B00011,
        B00011,
        B00011,
        B00011,
        B00011,

        // Ground left
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
    };
    int i;

    // Skip using character 0, this allows lcd.print() to be used
    // to quickly draw multiple characters
    for (i = 0; i < 7; ++i)
    {
        lcd.createChar(i + 1, &graphics[i * 8]);
    }

    // Fill screen with empty terrain
    for (i = 0; i < TERRAIN_WIDTH; ++i)
    {
        terrainUpper[i] = S_TERRAIN_EMPTY;
        terrainLower[i] = S_TERRAIN_EMPTY;
    }
}

// Slide the terrain to the left in half-character increments
void advanceTerrain(char *terrain, byte newTerrain)
{
    for (int i = 0; i < TERRAIN_WIDTH; ++i)
    {
        char current = terrain[i];
        char next = (i == TERRAIN_WIDTH - 1) ? newTerrain : terrain[i + 1];
        switch (current)
        {
        case S_TERRAIN_EMPTY:
            terrain[i] = (next == S_TERRAIN_SOLID) ? S_TERRAIN_SOLID_RIGHT : S_TERRAIN_EMPTY;
            break;
        case S_TERRAIN_SOLID:
            terrain[i] = (next == S_TERRAIN_EMPTY) ? S_TERRAIN_SOLID_LEFT : S_TERRAIN_SOLID;
            break;
        case S_TERRAIN_SOLID_RIGHT:
            terrain[i] = S_TERRAIN_SOLID;
            break;
        case S_TERRAIN_SOLID_LEFT:
            terrain[i] = S_TERRAIN_EMPTY;
            break;
        }
    }
}

// Draw hero on screen and check for collisions
bool drawHero(byte position, char *terrainUpper, char *terrainLower, unsigned int score)
{

    bool collide = false;
    char upperSave = terrainUpper[HERO_HORIZONTAL_POSITION];
    char lowerSave = terrainLower[HERO_HORIZONTAL_POSITION];
    byte upper, lower;

    // Draw the appropriate sprite for the hero (run or jump)
    switch (position)
    {
    case HP_OFF:
        upper = lower = S_TERRAIN_EMPTY;
        break;
    case HP_RUN_LOWER_1:
        upper = S_TERRAIN_EMPTY;
        lower = S_RUN1;
        break;
    case HP_RUN_LOWER_2:
        upper = S_TERRAIN_EMPTY;
        lower = S_RUN2;
        break;
    case HP_JUMP_1:
    case HP_JUMP_8:
        upper = S_TERRAIN_EMPTY;
        lower = S_JUMP;
        break;
    case HP_JUMP_2:
    case HP_JUMP_7:
        upper = S_JUMP_UPPER;
        lower = S_JUMP_LOWER;
        break;
    case HP_JUMP_3:
    case HP_JUMP_4:
    case HP_JUMP_5:
    case HP_JUMP_6:
        upper = S_JUMP;
        lower = S_TERRAIN_EMPTY;
        break;
    case HP_RUN_UPPER_1:
        upper = S_RUN1;
        lower = S_TERRAIN_EMPTY;
        break;
    case HP_RUN_UPPER_2:
        upper = S_RUN2;
        lower = S_TERRAIN_EMPTY;
        break;
    }

    // Detect collisions with terrain
    if (upper != ' ')
    {
        terrainUpper[HERO_HORIZONTAL_POSITION] = upper;
        collide = (upperSave == S_TERRAIN_EMPTY) ? false : true;
    }
    if (lower != ' ')
    {
        terrainLower[HERO_HORIZONTAL_POSITION] = lower;
        collide |= (lowerSave == S_TERRAIN_EMPTY) ? false : true;
    }

    // Calculate number of digits needed to draw the score
    byte digits = (score > 9999) ? 5 : (score > 999) ? 4
                                   : (score > 99)    ? 3
                                   : (score > 9)     ? 2
                                                     : 1;

    // Draw the scene
    terrainUpper[TERRAIN_WIDTH] = '\0';
    terrainLower[TERRAIN_WIDTH] = '\0';
    char temp = terrainUpper[16 - digits];
    terrainUpper[16 - digits] = '\0';
    lcd.setCursor(0, 0);
    lcd.print(terrainUpper);
    terrainUpper[16 - digits] = temp;
    lcd.setCursor(0, 1);
    lcd.print(terrainLower);

    // Draw score in upper right of screen
    lcd.setCursor(16 - digits, 0);
    lcd.print(score);

    terrainUpper[HERO_HORIZONTAL_POSITION] = upperSave;
    terrainLower[HERO_HORIZONTAL_POSITION] = lowerSave;

    return collide;
}
