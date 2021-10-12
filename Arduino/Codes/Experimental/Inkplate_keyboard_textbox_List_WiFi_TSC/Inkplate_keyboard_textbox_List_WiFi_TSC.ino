#include "Inkplate.h"
#include "WiFi.h"
#include "TSC2046E_Inkplate.h"

Inkplate display(INKPLATE_1BIT);
TSC2046E_Inkplate ts;
SPIClass *mySpi = NULL;

int tsX, tsY;

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
  uint8_t startElement = 0;
} list;

void setup() {
  Serial.begin(115200);
  display.begin();
  
  mySpi = display.getSPIptr();
  mySpi->begin(14, 12, 13, 15);
  ts.begin(mySpi, &display, 13, 14);
  ts.calibrate(800, 3420, 3553, 317, 0, 799, 0, 599);
 
  display.setTextSize(3);
  display.println("Trazenje WiFia...");
  display.display();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  list.maxElements = WiFi.scanNetworks();
  display.print("pronadjeno:");

  list.fontScale = 3;
  list.nMax = 25;
  list.x = 100;
  list.y = 100;
  for (int i = 0; i < list.maxElements; i++)
  {
    char tmp[40][40];
    WiFi.SSID(i).toCharArray(tmp[i], 32);
    list.list[i] = tmp[i];
  }
  listBox(&list, -1, -1);
  display.partialUpdate(false, true);

  int selectedWiFi = -1;
  while (selectedWiFi < 0)
  {
    if (ts.available(&tsX, &tsY))
    {
      selectedWiFi = listBox(&list, tsX, tsY);
      Serial.println(selectedWiFi, DEC);
      if (selectedWiFi == -2) display.partialUpdate(false, true);
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Unesi lozinku za ");
  display.println(WiFi.SSID(selectedWiFi));
  char c = -1;
  text.fontScale = 3;
  text.x = 0;
  text.y = 372;
  keyboard(-1, -1);
  textBox(&text, -1, -1, -1);
  display.partialUpdate();
  while (c != 6 && c != 24)
  {
    if (ts.available(&tsX, &tsY))
    {
      c = keyboard(tsX, tsY);
      textBox(&text, c, tsX, tsY);
      display.partialUpdate(false, true);
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  if (c == 6)
  {
    display.print("Spajanje u tijeku");
    WiFi.begin(WiFi.SSID(selectedWiFi).c_str(), text.text);
    while (WiFi.status() != WL_CONNECTED) {
      display.print('.');
      display.partialUpdate(false, true);
      delay(1000);
    }
    display.println("Spojeno!");
    display.display();
  }

  if (c == 24)
  {
    display.println("Spajanje na WiFi ponisteno");
    display.partialUpdate();
  }
}

void loop() {

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
      if (_x > 66 && _x < 132)
      {
        shiftKey ^= 1;   // Shift Key
        // Redraw whole keyboard
        display.fillRect(0, 400, 800, 200, WHITE);
      }
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

int listBox(struct listBoxHandle *s, int _x, int _y)
{
  // Return variable (selected element in the list, -1 if select is invalid or it's not selected at all)
  int _selected = -1;

  // Calculate dimensions of list box itself
  int16_t _boxW = (s->fontScale) * 6 * (s->nMax) + 6;
  int16_t _boxH = ((s->fontScale) * 8 * (s->showElements)) + ((s->showElements) * 6);

  display.fillRect(s->x, s->y, _boxW + 30, _boxH, WHITE);

  // Check touch boundaries for list
  if ((_x > s->x) && (_x < (s->x + _boxW)) && (_y > s->y) && (_y < (s->y + _boxH)))
  {
    _selected = ((_y - s->y) / (_boxH / s->showElements)) + s->startElement;
    if (_selected > s->maxElements - 1) _selected = -1;
  }

  // Check touch boundaries for scroll
  if ((_x > (s->x + _boxW)) && (_x < (s->x + _boxW + 30)))
  {
    if (_y > (s->y) && _y < (s->y + 30) && s->startElement > 0)
    {
      s->startElement--;
      _selected = -2;
    }
    if (_y > (s->y + _boxH - 30) && _y < (s->y + _boxH))
    {
      s->startElement++;
      _selected = -2;
    }
  }

  // Check if you call actually use scroll
  if (s->showElements > s->maxElements)
  {
    s->startElement = 0;
  } else if ((s->startElement + s->showElements) > s->maxElements)
  {
    // Check if start element is not bigger then the list
    s->startElement =  s->maxElements - s->showElements;
  }

  // Draw a box slider
  display.drawRect((s->x) + _boxW - 1, (s->y), 30, _boxH, BLACK);

  // Draw two arrows (one facing up and otherone down) at the middle of the slider box. Watchout! in order to arrow to be centered, you have substract two pixels (one for width and one for offset of one pixel while drawing rect)
  display.fillTriangle((s->x) + _boxW + 13, (s->y) + 5, (s->x) + _boxW + 3, (s->y) + 20, (s->x) + _boxW + 23, (s->y) + 20, BLACK);
  display.fillTriangle((s->x) + _boxW + 13, (s->y) + _boxH - 5, (s->x) + _boxW + 3, (s->y) + _boxH - 20, (s->x) + _boxW + 23, (s->y) + _boxH - 20, BLACK);

  // Calculate the size of the scroll wheel (accroding to the number of showed elemets vs all elemets)
  float _scrollScale = float(s->showElements) / (s->maxElements);

  // Do not allow bigger than max size if list box is bigger than nubmer of all elements
  if (_scrollScale > 1) _scrollScale = 1;

  // Calculate the height of rect of scroll wheel
  int16_t _scrollSize = (_boxH - 50.0) * _scrollScale;

  // Now draw scroll wheel itself
  int _scrollOffset = ((_boxH - 50) - _scrollSize) * (float(s->startElement) / (s->maxElements - s->showElements));
  display.fillRect((s->x) + _boxW + 4, (s->y) + 25 + _scrollOffset, 20, _scrollSize, BLACK);

  // Set size of fonts (it's linear scaling, that means scaling is the same in x and y direction)
  display.setTextSize(s->fontScale);

  // Draw just a box that contains list (list box itself)
  display.drawRect(s->x, s->y, _boxW, _boxH, BLACK);

  // Draw elements of list with offset of the box
  for (int i = 0; i < s->showElements; i++)
  {
    // Draw horizontal lines to create "box" with elements in
    display.drawFastHLine(s->x, (s->y) + ((s->fontScale) * 8 * i) + (i * 6), (s->fontScale) * 6 * (s->nMax) + 6, BLACK);
    if (s->list[i + s->startElement] != NULL)
    {
      // Now draw each character (because it's fixed number of letters in each element)
      display.setCursor(s->x + 4, (s->y) + ((s->fontScale) * 8 * i) + (i * 6) + 4);
      for (int j = 0; (j < s->nMax) && (s->list[i + s->startElement][j] != NULL); j++)
      {
        display.write(s->list[i + s->startElement][j]);
      }
    }
  }

  return _selected;
}
