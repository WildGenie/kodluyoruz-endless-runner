#include <Adafruit_LiquidCrystal.h>
#include <Wire.h>

#define PIN_BUTTON 2
#define PIN_AUTOPLAY 1
#define PIN_READWRITE 10
#define PIN_CONTRAST 7
#define SPR_RUN1 1
#define SPR_RUN2 2
#define SPR_JUMP 3
#define SPR_JUMP_UPPER '.'
#define SPR_JUMP_LOWER 4
#define SPR_TRN_EMPTY ' '
#define SPR_TRN_SOLID 5
#define SPR_TRN_SOLID_RIGHT 6
#define SPR_TRN_SOLID_LEFT 7
#define HERO_HORIZONTAL_POSITION 1
#define TRN_WIDTH 16
#define TRN_EMPTY 0
#define TRN_LOWER_BLOCK 1
#define TRN_UPPER_BLOCK 2

Adafruit_LiquidCrystal lcd(0);

static char t_upper[TRN_WIDTH + 1];
static char t_lower[TRN_WIDTH + 1];
static bool button = false;

void init_graphics()
{
    static byte graphics[] = {
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
    int i;
    for (i = 0; i < 7; ++i)
    {
        lcd.createChar(i + 1, &graphics[i * 8]);
    }
    for (i = 0; i < TRN_WIDTH; ++i)
    {
        t_upper[i] = SPR_TRN_EMPTY;
        t_lower[i] = SPR_TRN_EMPTY;
    }
}
void advance_terrain(char *terrain, byte newTerrain)
{
    for (int i = 0; i < TRN_WIDTH; ++i)
    {
        char current = terrain[i];
        char next = (i == TRN_WIDTH - 1) ? newTerrain : terrain[i + 1];
        switch (current)
        {
        case SPR_TRN_EMPTY:
            terrain[i] = (next == SPR_TRN_SOLID) ? SPR_TRN_SOLID_RIGHT : SPR_TRN_EMPTY;
            break;
        case SPR_TRN_SOLID:
            terrain[i] = (next == SPR_TRN_EMPTY) ? SPR_TRN_SOLID_LEFT : SPR_TRN_SOLID;
            break;
        case SPR_TRN_SOLID_RIGHT:
            terrain[i] = SPR_TRN_SOLID;
            break;
        case SPR_TRN_SOLID_LEFT:
            terrain[i] = SPR_TRN_EMPTY;
            break;
        }
    }
}
bool draw_hero(byte position, char *terrainUpper, char *terrainLower, unsigned int score)
{
    bool collide = false;
    char upperSave = terrainUpper[HERO_HORIZONTAL_POSITION];
    char lowerSave = terrainLower[HERO_HORIZONTAL_POSITION];
    byte upper, lower;
    switch (position)
    {
    case H_POS_OFF:
        upper = lower = SPR_TRN_EMPTY;
        break;
    case H_RUN_LOWER_1:
        upper = SPR_TRN_EMPTY;
        lower = SPR_RUN1;
        break;
    case H_RUN_LOWER_2:
        upper = SPR_TRN_EMPTY;
        lower = SPR_RUN2;
        break;
    case H_JUMP_1:
    case H_JUMP_8:
        upper = SPR_TRN_EMPTY;
        lower = SPR_JUMP;
        break;
    case H_JUMP_2:
    case H_JUMP_7:
        upper = SPR_JUMP_UPPER;
        lower = SPR_JUMP_LOWER;
        break;
    case H_JUMP_3:
    case H_JUMP_4:
    case H_JUMP_5:
    case H_JUMP_6:
        upper = SPR_JUMP;
        lower = SPR_TRN_EMPTY;
        break;
    case H_RUN_UPPER_1:
        upper = SPR_RUN1;
        lower = SPR_TRN_EMPTY;
        break;
    case H_RUN_UPPER_2:
        upper = SPR_RUN2;
        lower = SPR_TRN_EMPTY;
        break;
    }
    if (upper != ' ')
    {
        terrainUpper[HERO_HORIZONTAL_POSITION] = upper;
        collide = (upperSave == SPR_TRN_EMPTY) ? false : true;
    }
    if (lower != ' ')
    {
        terrainLower[HERO_HORIZONTAL_POSITION] = lower;
        collide |= (lowerSave == SPR_TRN_EMPTY) ? false : true;
    }
    byte digits = (score > 9999) ? 5 : (score > 999) ? 4
                                   : (score > 99)    ? 3
                                   : (score > 9)     ? 2
                                                     : 1;
    terrainUpper[TRN_WIDTH] = '\0';
    terrainLower[TRN_WIDTH] = '\0';
    char temp = terrainUpper[16 - digits];
    terrainUpper[16 - digits] = '\0';
    lcd.setCursor(0, 0);
    lcd.print(terrainUpper);
    terrainUpper[16 - digits] = temp;
    lcd.setCursor(0, 1);
    lcd.print(terrainLower);
    lcd.setCursor(16 - digits, 0);
    lcd.print(score);
    terrainUpper[HERO_HORIZONTAL_POSITION] = upperSave;
    terrainLower[HERO_HORIZONTAL_POSITION] = lowerSave;
    return collide;
}

void button_push() { button = true; }

void setup()
{
    pinMode(PIN_BUTTON, INPUT);
    digitalWrite(PIN_BUTTON, HIGH);
    pinMode(PIN_AUTOPLAY, OUTPUT);
    digitalWrite(PIN_AUTOPLAY, HIGH);
    attachInterrupt(0, button_push, FALLING);
    init_graphics();
    lcd.begin(16, 2);
}

void loop()
{
    const byte H_POS_OFF = 0;
    const byte H_RUN_LOWER_1 = 1;
    const byte H_RUN_LOWER_2 = 2;
    const byte H_JUMP_1 = 3;
    const byte H_JUMP_8 = 10;
    const byte H_JUMP_3 = 5;
    const byte H_JUMP_5 = 7;
    const byte H_RUN_UPPER_1 = 11;
    const byte H_RUN_UPPER_2 = 12;

    static bool playing = false;
    static bool blink = false;
    static unsigned int distance = 0;

    if (!playing)
    {
        draw_hero((blink) ? H_POS_OFF : H_RUN_LOWER_1, t_upper, t_lower, distance >> 3);
        if (blink)
        {
            lcd.setCursor(0, 0);
            lcd.print("Press Start");
        }
        delay(100);
        blink = !blink;

        if (button)
        {
            init_graphics();
            playing = true;
            button = false;
            distance = 0;
        }
        return;
    }

    static byte heroPos = H_RUN_LOWER_1;
    static byte newTerrainType = TRN_EMPTY;
    static byte newTerrainDuration = 1;

    advance_terrain(t_lower, newTerrainType == TRN_LOWER_BLOCK ? SPR_TRN_SOLID : SPR_TRN_EMPTY);
    advance_terrain(t_upper, newTerrainType == TRN_UPPER_BLOCK ? SPR_TRN_SOLID : SPR_TRN_EMPTY);

    if (--newTerrainDuration == 0)
    {
        if (newTerrainType == TRN_EMPTY)
        {
            newTerrainType = (random(3) == 0) ? TRN_UPPER_BLOCK : TRN_LOWER_BLOCK;
            newTerrainDuration = 2 + random(10);
        }
        else
        {
            newTerrainType = TRN_EMPTY;
            newTerrainDuration = 10 + random(10);
        }
    }

    if (button)
    {
        if (heroPos <= H_RUN_LOWER_2)
            heroPos = H_JUMP_1;
        button = false;
    }

    if (draw_hero(heroPos, t_upper, t_lower, distance >> 3))
    {
        playing = false;
    }
    else
    {
        if (heroPos == H_RUN_LOWER_2 || heroPos == H_JUMP_8)
        {
            heroPos = H_RUN_LOWER_1;
        }
        else if ((heroPos >= H_JUMP_3 && heroPos <= H_JUMP_5) && t_lower[HERO_HORIZONTAL_POSITION] != SPR_TRN_EMPTY)
        {
            heroPos = H_RUN_UPPER_1;
        }
        else if (heroPos >= H_RUN_UPPER_1 && t_lower[HERO_HORIZONTAL_POSITION] == SPR_TRN_EMPTY)
        {
            heroPos = H_JUMP_5;
        }
        else if (heroPos == H_RUN_UPPER_2)
        {
            heroPos = H_RUN_UPPER_1;
        }
        else
        {
            ++heroPos;
        }

        ++distance;
        digitalWrite(PIN_AUTOPLAY, t_lower[HERO_HORIZONTAL_POSITION + 2] == SPR_TRN_EMPTY ? HIGH : LOW);
    }

    delay(50);
}