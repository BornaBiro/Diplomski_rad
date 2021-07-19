#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;
#define X0 11
#define Y0 10
#define X1 9
#define Y1 8
#define AX 34
#define AY 35

int ts[2];

int shiftKey = 0;
struct textBoxHandle
{
  static const uint8_t maxSize = 50;
  char text[maxSize + 1];
  uint8_t size = 20;
  uint8_t n = 0;
  uint8_t selected = 0;
  uint8_t fontScale = 1;
  int16_t x = 0;
  int16_t y = 0;
} text;

struct listBoxHandle
{
  static const uint8_t maxSize = 20;
  char *list[maxSize];
  int16_t x = 0;
  int16_t y = 0;
  uint8_t maxElements = 0;
  uint8_t showElements = 5;
  uint8_t nMax = 10;
  uint8_t fontScale = 1;
} list;

void setup() {
  display.begin();
  //keyboard(-1, -1);
  //text.x = 0;
  //text.y = 372;
  //text.size = 30;
  //text.fontScale = 3;
  //textBox(&text, 0, -1, -1);
  //display.display();
  //initTouch();

  list.fontScale = 3;
  list.x = 100;
  list.y = 100;
  list.maxElements = 7;
  list.showElements = 5;
  list.list[0] = "WiFi1";
  list.list[1] = "MyWiFi123456";
  list.list[2] = "do_not_connect_to_it";
  list.list[3] = "tryme_xD";
  list.list[4] = "hell_yeah112233";
  list.list[5] = "mujo_i_haso";
  list.list[6] = "WiFi987654321";
  listBox(&list);
  //display.setTextSize(4);
  //display.write((char)list.list[0][0]);
  display.display();
}

void loop() {
  // Keyboard & textbox stuff
  //touchSleep();
  //if (touchAvailable()) {
  //  readTouch(ts);
  //  display.setTextColor(BLACK, WHITE);
  //  ts[0] = map(ts[0], 2846, 628, 0, 799);
  //  ts[1] = map(ts[1], 2677, 391, 0, 599);
  //  char c = keyboard(ts[0], ts[1]);
  //  if (c == 24)
  //  {
  //    display.clearDisplay();
  //    display.println("Input canceled");
  //    display.display();
  //  }
  //  textBox(&text, c, ts[0], ts[1]);
  //  display.partialUpdate(true, true);
  //}
}

void initTouch() {
  //mcp.begin(0);
  pinMode(AY, INPUT);
  pinMode(AX, INPUT);
}

bool touchAvailable() {
  mcp.pinMode(X0, OUTPUT);
  mcp.pullUp(Y1, HIGH);
  mcp.pinMode(X1, INPUT);
  mcp.pinMode(Y0, INPUT);
  mcp.digitalWrite(X0, LOW);
  delay(1);
  return !mcp.digitalRead(Y1);
}

void touchSleep() {
  mcp.pullUp(Y1, HIGH);
  mcp.pinMode(X0, OUTPUT);
  mcp.digitalWrite(X0, LOW);
  mcp.pinMode(Y0, INPUT);
  mcp.pinMode(X1, INPUT);
  mcp.pinMode(Y1, INPUT);
}

bool readTouch(int *t) {
  int z1, i = 0;
  mcp.pinMode(Y0, OUTPUT);
  mcp.pinMode(Y1, OUTPUT);
  mcp.pinMode(X0, INPUT);
  mcp.pinMode(X1, INPUT);
  mcp.digitalWrite(Y0, LOW);
  mcp.digitalWrite(Y1, HIGH);
  delay(1);
  t[1] = analogRead(AX);
  mcp.pinMode(Y0, INPUT);
  mcp.pinMode(Y1, INPUT);
  mcp.pinMode(X0, OUTPUT);
  mcp.pinMode(X1, OUTPUT);
  mcp.digitalWrite(X0, HIGH);
  mcp.digitalWrite(X1, LOW);
  delay(1);
  t[0] = analogRead(AY);
  mcp.pinMode(X0, OUTPUT);
  mcp.pinMode(Y0, OUTPUT);
  mcp.pinMode(X1, INPUT);
  mcp.pinMode(Y1, INPUT);
  mcp.digitalWrite(X0, HIGH);
  mcp.digitalWrite(Y0, LOW);
  delay(1);
  z1 = analogRead(AY);
  if (z1 > 20) {
    return 1;
  }
  return 0;
}

void textBox(struct textBoxHandle *s, char _c, int _tsx, int _tsy)
{
  int k = 0;
  int _h = (s->fontScale * 8) + 4;
  int _w = (s->fontScale * 6 * s->size) + 4;
  int _startPos = 0;
  int i = 0;

  //Alter string with new char
  //Check if its new char
  if (_c != -1)
  {
    //If you need to add new char, check if is valid char, if there is room inside string for new char and if selected place is vaild (if its not out of string)
    //If it's everything ok, put new char in selected place, increment number of chars in string (variable n in struct) and selected place in string
    if (_c > 31 && _c < 127 && s->n < s->maxSize && s->selected <= s->n)
    {
      memmove(s->text + s->selected + 1, s->text + s->selected, s->n - s->selected);
      s->text[s->selected++] = _c;
      s->n++;
    }
    //If you need to remove char, on selected place move whole string from that place to end of string to one place to the left
    else if (_c == 8) //Clean string by moving rest of array
    {
      if (s->n > 0)
      {
        memmove(s->text + s->selected - 1, s->text + s->selected, s->n - s->selected);
        if (s->selected > 0) s->selected--;
        s->n--;
        s->text[s->n] = 0;
      }
    }
  }

  //Redraw textbox on screen. Only 5x7 fonts are allowed in textbox for now!
  display.setFont(NULL);
  display.setTextSize(s->fontScale);
  display.drawRect(s->x, s->y, _w, _h, BLACK);                    //Make a text box border
  display.fillRect(s->x + 1, s->y + 1, _w - 2, _h - 2, WHITE);    //Clear prevoius text
  display.setCursor(s->x + 3, s->y + 2);                          //Set cursor on vaild position (start of the textbox)

  //Calculate from what element string should be printed
  if (s->n > s->size)
  {
    _startPos = s->n - s->size;
  }

  //Start printing string
  for (i = 0; (s->text[i + _startPos] != 0) && (i < s->size); i++)
  {
    display.write(s->text[i + _startPos]);
  }

  //Check if there is touch on textbox. If it is, calculate new selected index of string where all new chars will be inserted.
  if (_tsx > s->x && _tsx < (s->x + _w) && _tsy > s->y && _tsy < (s->y + _h))
  {
    int j = (_tsx - s->x) / 6 / s->fontScale;
    s->selected = j + _startPos;
    if (s->selected > s->n) s->selected = s->n;
  }

  //Draw cursor on screen
  display.drawFastVLine(((s->selected - _startPos) * 6 * s->fontScale) + s->x + 1, s->y + 2, s->fontScale * 8, BLACK);
  //You can do this way, because, you will or alter the string by adding new letters or change cursor position, but not both in the same time!
}

char keyboard(int _x, int _y)
{
  // Keymap for SHIFT key pressed
  static const char key[4][12] = {
    {'!', '"', '#', '$', '%', '&', '/', '(', ')', '=', '?', '*'},
    {'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', '[', ']'},
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\\', '|', ' '},
    {'>', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', ' '},
  };

  // Keymap for non pressed SHIFT key
  static const char key2[4][12] = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', char(39), '+'},
    {'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', '{', '}'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '@', ' ', ' '},
    {'<', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', ' '},
  };

  // Symbols for non-standard keys (like backspace, shift, enter and cancel)
  const uint8_t shiftKeySymbol[] PROGMEM = {
    0x00, 0xc0, 0x00, 0x01, 0xe0, 0x00, 0x01, 0xf0, 0x00, 0x03, 0xf0, 0x00, 0x07, 0xf8, 0x00, 0x0f, 0x3c, 0x00, 0x0e, 0x1c, 0x00, 0x1e, 0x1e, 0x00, 0x3c, 0x0f, 0x00, 0x38, 0x07, 0x80, 0x78, 0x07, 0x80, 0xfe, 0x1f, 0xc0, 0xfe, 0x1f, 0xc0, 0x0e, 0x1c, 0x00, 0x0e, 0x1c, 0x00, 0x0e, 0x1c, 0x00, 0x0e, 0x1c, 0x00, 0x0e, 0x1c, 0x00, 0x0e, 0x1c, 0x00, 0x0e, 0x1c, 0x00, 0x0e, 0x1c, 0x00, 0x0f, 0xfc, 0x00, 0x0f, 0xfc, 0x00, 0x0f, 0xfc, 0x00
  };
  const uint8_t backKeySymbol[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x0f, 0xfc, 0x00, 0x1c, 0x0e, 0x00, 0x3e, 0x0f, 0x00, 0x77, 0x1f, 0x80, 0x63, 0xb9, 0x80, 0x61, 0xf1, 0x80, 0x60, 0xe1, 0x80, 0x61, 0xf1, 0x80, 0x63, 0xb9, 0x80, 0x67, 0x1d, 0x80, 0x7e, 0x0f, 0x80, 0x3c, 0x07, 0x00, 0x1c, 0x0e, 0x00, 0x0f, 0xfc, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  const uint8_t enterKeySymbol[] = {
    0x00, 0x00, 0x80, 0x00, 0x01, 0x80, 0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x07, 0x80, 0x00, 0x07, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1c, 0x00, 0x20, 0x3c, 0x00, 0x70, 0x38, 0x00, 0x78, 0x38, 0x00, 0x38, 0x70, 0x00, 0x3c, 0x70, 0x00, 0x1c, 0xf0, 0x00, 0x1e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0f, 0xc0, 0x00, 0x07, 0xc0, 0x00, 0x07, 0xc0, 0x00, 0x03, 0x80, 0x00
  };
  const uint8_t backspaceKeySymbol[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x07, 0x80, 0x00, 0x0f, 0x80, 0x00, 0x1f, 0x80, 0x00, 0x3f, 0x80, 0x00, 0x7f, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0x7f, 0xff, 0xc0, 0x3f, 0x80, 0x00, 0x1f, 0x80, 0x00, 0x0f, 0x80, 0x00, 0x07, 0x80, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  // Size in px for symbols
  const int keySymbol_w = 18;
  const int keySymbol_h = 24;

  // Key that has been pressed (if it's invalid, send -1 instead of char)
  char _c = -1;

  // Check touch boundaries (keyboard is alwasy at the bottom, X = 0...800, Y = 400...600)
  if (_y > 400 && _y < 600 && _x > 0 && _x < 800)
  {
    if (_y >= 570) // Non-standard key is pressed?
    {
      if (_x < 66) _c = 24;  // Cancel Key
      if (_x > 66 && _x < 132) shiftKey ^= 1;   // Shift Key
      if (_x > 132 && _x < 660) _c = ' ';    // Spacebar
      if (_x > 660 && _x < 726) _c = 8;      // Backspace
      if (_x > 726 && _x < 792) _c = 6;     // Confirm key
    }
    else  //Standard key is pressed?
    {
      _c = shiftKey ? key[(_y - 400) / 40][_x / 66] : key2[(_y - 400) / 40][_x / 66];
      if (_c == ' ') _c = -1; // If it's empty space on keyboard, return invalid press
    }
  }

  // Draw keyboard
  display.setTextSize(3);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 12; j++) {
      display.setCursor(33 + (j * 66) - 9, 400 + (40 * i) + 10);
      display.print(shiftKey ? key[i][j] : key2[i][j]);
    }
  }
  display.setCursor(355, 570);
  display.print("SPACE");
  display.drawBitmap(90, 570, shiftKeySymbol, keySymbol_w, keySymbol_h, BLACK);
  display.drawBitmap(24, 570, backKeySymbol, keySymbol_w, keySymbol_h, BLACK);
  display.drawBitmap(750, 570, enterKeySymbol, keySymbol_w, keySymbol_h, BLACK);
  display.drawBitmap(684, 570, backspaceKeySymbol, keySymbol_w, keySymbol_h, BLACK);

  // Draw lines of each key
  for (int i = 0; i < 5; i++)
  {
    display.drawFastHLine(0, 400 + 40 * i, 800, BLACK);
  }
  for (int i = 1; i < 12; i++)
  {
    int _l = (i > 2) && (i < 10) ? 160 : 200;
    display.drawFastVLine(i * 66, 400, _l, BLACK);
  }

  // Return what has been pressed
  return _c;
}

int listBox(struct listBoxHandle *s)
{
  display.setTextSize(s->fontScale);
  display.drawRect(s->x, s->y, (s->fontScale) * 6 * (s->nMax) + 6, ((s->fontScale) * 8 * (s->showElements)) + ((s->showElements) * 6), BLACK);
  for (int i = 0; i < (s->showElements); i++)
  {
    display.drawFastHLine(s->x, (s->y) + ((s->fontScale) * 8 * i) + (i * 6), (s->fontScale) * 6 * (s->nMax) + 6, BLACK);
    if (s->list[i] != NULL)
    {
      display.setCursor(s->x + 4, (s->y) + ((s->fontScale) * 8 * i) + (i * 6) + 4);
      for (int j = 0; (j < s->nMax) && (s->list[i][j] != NULL); j++)
      {
        display.write(s->list[i][j]);
      }
    }
  }

  return 0;
}
