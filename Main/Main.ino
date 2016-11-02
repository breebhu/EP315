#include<openGLCD.h>
#include<LiquidCrystal.h>
#include<math.h>

LiquidCrystal lcd(38, 39, 40, 42, 44, 46);

uint8_t Lx[128] = {0}; //stores x position of line
uint8_t Ly[128] = {0}; //stores y position of line

uint8_t Cx[1000] = {0}; //stores x position of curve:circle/rectangle
uint8_t Cy[1000] = {0}; //stores y position of curve:circle/rectangle

bool L[128] = {false}; //stores real image at Lx,Ly co-ordinate
bool C[1000] = {false};//stores real image at Cx,Cy co-ordinate

volatile int LCount = -1; //stores length of line drawn(more accurately,L+1), initially set to -1
volatile int curveCount = -1; //stores length of curve drawn(more accurately,L+1), initially set to -1

//mode variable declarations

volatile bool penLift = false;
volatile bool eraseMode = false;
volatile bool clearScreen = false;
volatile bool rectMode = false, nextRectMode = false, prevRectMode = false;
//rectMode indicates if current mode is rectangle drawing mode
//prevRectMode indicates if in the last loop it was RectMode, it being false indicates that user just entered rectMode and initial point has to be set
//nextRectMode indicates whether in the next loop one has to exit rectangle mode after drawing rectangle

volatile bool circMode = false, nextCircMode = false, prevCircMode = false; //same convention for circle as above for rectangle

//pin declarations
const int joyPin2 = A5;
const int joyPin1 = A4;
const int PenLiftPin = 2;
const int ErasePin = 3;
const int ClearPin = 18;
const int BrushSizeChangePin = 19;
const int RectPin = 20;
const int CircPin = 21;
//const int OnOffPin = 4;

int value2 = 0; // variable to read the value from the joystick X pin
int value1 = 0; // variable to read the value from the joystick Y pin

bool lastClearScreen = false;
volatile int brushType = 0;

int x, y; //stores current position of cursor

int rx1, rx2, ry1, ry2; //stores position of intial and final position while in rectangle/circle mode

//variables for purpose of debouncing, stores the previous time button was pressed
volatile long penLiftLast = 0;
volatile long eraseModeLast = 0;
volatile long clearScreenLast = 0;
volatile long brushTypeLast = 0;
volatile long lastRectMode = 0;
volatile long lastCircMode = 0;

const long debounceTime = 1000;
const int thresholdL = 100;
const int thresholdH = 900;
const long brushSizeChangeTime = 2000;

int real_image[5][5] = {0};

const int initDelay = 5000;

void setup()
{
  InitPins();
  InitLCDs(initDelay);
}

void loop() {
  //  if (digitalRead(OnOffPin)) {
  delay(150);
  value2 = analogRead(joyPin2); //read value of joyPin2
  delay(10);
  value1 = analogRead(joyPin1); //read value of joyPin1
  if (!lastClearScreen) //since clear screen command was not given, print real image
  {
    for (int i = -2; i < 3; i++) {
      for (int j = -2; j < 3; j++) {
        GLCD.SetDot(x + j, y + i, real_image[2 + j][2 + i]);
      }
    }
  }
  else  //since the screen was cleared in last loop,don't print anything here
  {
    lastClearScreen = false; //now we have cleared the screen
  }

  if (value1 > thresholdH) //check if user has moved joystick to right
  {
    x = x + 1;
    if (x > 127)x = 0;  //since screen width is 128, wrap around
  }
  else if (value1 < thresholdL) //check if user has moved joystick to left
  {
    x = x - 1;
    if (x < 0)x = 127; //since screen width is 128, wrap around
  }
  if (value2 < thresholdL) //check if user has moved joystick up
  {
    y = y + 1;
    if (y > 63)y = 0; //since screen height is 64, wrap around
  }
  else if (value2 > thresholdH) //check if user has moved joystick up
  {
    y = y - 1;
    if (y < 0)y = 63; //since screen height is 64, wrap around
  }

  if (rectMode) //check if currently user is in rectangle mode
  {
    lcd.setCursor(0, 0);
    lcd.print("Brush Size: 1x1");
    lcd.setCursor(0, 1);
    lcd.print("Mode: Rectangle");
    for (int i = 0; i < curveCount; i++) //print real image corresponding to points of the previous (temporary) rectangle
    {
      int x0 = Cx[i];
      int y0 = Cy[i];
      if (C[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
    }
    if (nextRectMode) //check if user wants to draw the rectangle at current position and exit rectangle mode
    {
      //find out height and width of rectangle from current cursor position
      int w = absDiff(rx1, rx2) + 1;
      int h = absDiff(ry1, ry2) + 1;

      //check which coordinates correspond to top left corner of rectangle
      if (rx2 == rx1 || ry2 == ry1) GLCD.DrawLine(rx1, ry1, rx2, ry2);
      else if (rx2 > rx1 && ry2 > ry1)GLCD.DrawRect(rx1, ry1, w, h);
      else if (rx2 < rx1 && ry2 > ry1)GLCD.DrawRect(rx2, ry1, w, h);
      else if (rx2 > rx1 && ry2 < ry1)GLCD.DrawRect(rx1, ry2, w, h);
      else if (rx2 < rx1 && ry2 < ry1)GLCD.DrawRect(rx2, ry2, w, h);

      //while exitig rectMode, adjust status variables accordingly
      rectMode = false;
      nextRectMode = false;
      prevRectMode = true;
    }
    else  //user is still adjusting rectangle size
    {

      if (prevRectMode == false) //while entering rectangle mode for first time, set current position as starting point
      {
        rx1 = x;
        ry1 = y;
        prevRectMode = true;
      }

      //current position is ending point
      rx2 = x;
      ry2 = y;

      //calculate width and height for drawing the (current) rectangle
      int w = absDiff(rx1, rx2) + 1 ;
      int h = absDiff(ry1, ry2) + 1;

      //depending on which is upper left corner, store the rectangle points in the array

      if (rx2 == rx1)
      {
        if (ry2 >= ry1)storeRectArray(rx1, ry1, w, h);
        else storeRectArray(rx1, ry2, w, h);
      }
      else if (ry2 == ry1)
      {
        if (rx2 >= rx1)storeRectArray(rx1, ry1, w, h);
        else storeRectArray(rx2, ry1, w, h);
      }
      else if (rx2 > rx1 && ry2 > ry1)storeRectArray(rx1, ry1, w, h);
      else if (rx2 < rx1 && ry2 > ry1)storeRectArray(rx2, ry1, w, h);
      else if (rx2 > rx1 && ry2 < ry1)storeRectArray(rx1, ry2, w, h);
      else if (rx2 < rx1 && ry2 < ry1)storeRectArray(rx2, ry2, w, h);

      //store the real image at this point of time corresponding to rectangle points
      for (int i = 0; i < curveCount; i++)
      {
        int x0 = Cx[i];
        int y0 = Cy[i];
        if (ActualReadData(x0, y0) == BLACK) C[i] = true; else C[i] = false;
      }

      //draw the rectangle
      if (rx2 == rx1 || ry2 == ry1) GLCD.DrawLine(rx1, ry1, rx2, ry2);
      else if (rx2 > rx1 && ry2 > ry1)GLCD.DrawRect(rx1, ry1, w, h);
      else if (rx2 < rx1 && ry2 > ry1)GLCD.DrawRect(rx2, ry1, w, h);
      else if (rx2 > rx1 && ry2 < ry1)GLCD.DrawRect(rx1, ry2, w, h);
      else if (rx2 < rx1 && ry2 < ry1)GLCD.DrawRect(rx2, ry2, w, h);
    }
  }
  else if (circMode) //check if currently user is in circle mode
  {
    lcd.setCursor(0, 0);
    lcd.print("Brush Size: 1x1");
    lcd.setCursor(0, 1);
    lcd.print("Mode: Circle");
    for (int i = 0; i < LCount; i++) //print real image corresponding to points of the previous (temporary) radius
    {
      int x0 = Lx[i] % 128;
      int y0 = Ly[i] % 64;
      if (L[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
    }
    for (int i = 0; i < curveCount; i++) ////print real image corresponding to points of the previous (temporary) circle
    {
      int x0 = Cx[i] % 128;
      int y0 = Cy[i] % 64;
      if (C[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
    }
    if (nextCircMode) //check if user wants to draw the circle at current position and exit circle mode
    {
      //calculate radius of circle from current position of cursor
      int w = absDiff(rx1, rx2) + 1;
      int h = absDiff(ry1, ry2) + 1;

      //if current position is same as starting point, draw a dot
      if (w == 1 && h == 1) GLCD.SetDot(rx1, ry1, BLACK);
      else
      {
        int rad = (int)(sqrt(w * w + h * h));
        drawCirc(rx1, ry1, rad);
      }

      //reset cursor position to centre of circle
      x = rx1;
      y = ry1;

      //adjust status variables before exiting circMode
      circMode = false;
      nextCircMode = false;
      prevCircMode = true;
    }
    else //user is still adjusting circle size
    {
      if (prevCircMode == false) //while entering circle mode for first time, set current position as starting point
      {
        rx1 = x;
        ry1 = y;
        prevCircMode = true;
      }

      //set current position as ending point of radius line
      rx2 = x;
      ry2 = y;

      //calculate radius from starting and ending points
      int w = absDiff(rx1, rx2) + 1;
      int h = absDiff(ry1, ry2) + 1;
      int rad = (int)(sqrt(w * w + h * h));

      //store coordinates of points on circle and radius line
      storeLineArray(rx1, ry1, rx2, ry2);
      storeCircArray(rx1, ry1, rad);

      //store real image corresponding to circle and radius line
      for (int i = 0; i < LCount; i++)
      {
        int x0 = Lx[i];
        int y0 = Ly[i];
        if (ActualReadData(x0, y0) == BLACK) L[i] = true; else L[i] = false;
      }

      for (int i = 0; i < curveCount; i++)
      {
        int x0 = Cx[i] % 128;
        int y0 = Cy[i] % 64;
        if (ActualReadData(x0, y0) == BLACK) C[i] = true; else C[i] = false;
      }

      //draw circle and radius line
      GLCD.DrawLine(rx1, ry1, rx2, ry2);
      drawCirc(rx1, ry1, rad);
    }
  }
  //If the user is not in one of the "special" Modes
  else if (!penLift) {//Pen is Down on Canvas
    if (!eraseMode) {//User is not erasing - Erase Mode allows only one Brush Size
      setBrush(brushType, brushSizeChangeTime); //Set the brush Size for Painting
    }
    else {
      GLCD.SetDot(x, y, WHITE);//Erasing

      //LCD updated to show Erase mode
      lcd.setCursor(0, 0);
      lcd.print("Brush Size: 1x1 ");
      lcd.setCursor(0, 1);
      lcd.print("Mode: Erase     ");
    }
  } else {
    //LCD updated to show PenLift Mode
    lcd.setCursor(0, 0);
    lcd.print("Brush Size: None");
    lcd.setCursor(0, 1);
    lcd.print("Mode: Pen Lift");
  }

  //Store the real image so that the cursor can be put
  for (int i = -2; i < 3; i++) {
    for (int j = -2; j < 3; j++) {
      real_image[2 + j][2 + i] = ActualReadData((x + j) % 128, (y + i) % 64);
    }
  }

  //Putting Cursor
  if (!(rectMode || circMode)) {//Brush Size changes only if not in special modes
    setcursor(brushType);
  }
  else {//Smallest Brush Size in special Modes
    setcursor(0);
  }

  //Command Clears screen
  if (clearScreen) clearScreenFunc();
  //  else {
  //    InitScreen(1000);
  //  }
}

//ISRs: Notify the Main program about inputs through boolean variables
//Debouncing Implemented in Software
void penLiftISR() {
  if (millis() - penLiftLast < debounceTime) {
    return;
  }
  penLift = !(penLift);
  penLiftLast = millis();
}

void eraseModeISR() {
  if (millis() - eraseModeLast < debounceTime) {
    return;
  }
  eraseMode = !(eraseMode);
  eraseModeLast = millis();
}

void clearScreenISR() {
  if (millis() - clearScreenLast < debounceTime) {
    return;
  }
  clearScreen = true;
  clearScreenLast = millis();
}

void brushSizeChangeISR() {
  if (millis() - brushTypeLast > debounceTime) {
    if (brushType < 2) {
      brushType++;
    }
    else {
      brushType = 0;
    }
    brushTypeLast = millis();
  }
}

void RectPinISR()
{
  if (circMode) return;
  if (millis() - lastRectMode > debounceTime)
  {
    if (rectMode)
    {
      nextRectMode = true;
    }
    else
    {
      rectMode = true;
      nextRectMode = false;
      prevRectMode = false;
      LCount = -1;
      curveCount = -1;
    }
    lastRectMode = millis();
  }
}

void CircPinISR()
{
  if (rectMode) return;
  if (millis() - lastCircMode > debounceTime)
  {
    if (circMode)
    {
      nextCircMode = true;
    }
    else
    {
      circMode = true;
      nextCircMode = false;
      prevCircMode = false;
      LCount = -1;
      curveCount = -1;
    }
    lastCircMode = millis();
  }
}

//Sets the Cursor according to Brush Size
int setcursor(int BrushType) {
  int color = BLACK;
  if (BrushType == 0) {
    GLCD.SetDot(x, y, color);
    GLCD.SetDot(x + 1, y, color);
    GLCD.SetDot(x - 1, y, color);
    GLCD.SetDot(x, y + 1, color);
    GLCD.SetDot(x, y - 1, color);
    return 0;
  }
  if (BrushType == 1) {
    GLCD.SetDot(x, y, color);
    GLCD.SetDot(x + 1, y, color);
    GLCD.SetDot(x, y - 1, color);
    GLCD.SetDot(x + 1, y - 1, color);
    return 0;
  }
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      GLCD.SetDot(x + i, y + j, color);
    }
  }
  GLCD.SetDot(x + 2, y, color);
  GLCD.SetDot(x - 2, y, color);
  GLCD.SetDot(x, y + 2, color);
  GLCD.SetDot(x, y - 2, color);
  return 0;
}

//Returns the pixel value at coordinate (x,y)
int ActualReadData(int x, int y) {
  uint8_t bitarray[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
  GLCD.GotoXY(x, y);
  uint8_t data = GLCD.ReadData();
  if (data & bitarray[y % 8]) return BLACK;
  else return WHITE;
}

//Function to clear screen and reset back to initial state
void clearScreenFunc() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      real_image[i][j] = 0;
    }
  }
  x = GLCD.CenterX; y = GLCD.CenterY;
  real_image[1][1] = 1;
  GLCD.ClearScreen();
  GLCD.CursorTo(x, y);
  clearScreen = false;
  lastClearScreen = true;
  //indicate to next loop that the screen has been cleared and hence real image must be set to zero
}

//Setting Brush size with designated waiting period for confirming decision
void setBrush(int BrushType, long waitPeriod) {
 // while (true) {
    long start =  millis();
    if (BrushType == 0) {
      GLCD.SetDot(x, y, BLACK);
      return;
    }
    if (BrushType == 1) {
      GLCD.SetDot(x, y, BLACK);
      GLCD.SetDot(x + 1, y, BLACK);
      GLCD.SetDot(x, y - 1, BLACK);
      GLCD.SetDot(x + 1, y - 1, BLACK);
      return;
    }
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        GLCD.SetDot(x + i, y + j, BLACK);
      }
    }
   // while (millis() - start < waitPeriod) {}
   // if (brushType == BrushType) break;
   // else BrushType = brushType;
  //}
  //LCD updated for current Brush Size
  lcd.setCursor(0, 0);
  if (BrushType == 0) {
    lcd.print("Brush Size: 1x1 ");
  }
  if (BrushType == 1) {
    lcd.print("Brush Size: 2x2 ");
  }
  if (BrushType == 2) {
    lcd.print("Brush Size: 3x3 ");
  }
}

int absDiff(int a, int b)
{
  return a > b ? a - b : b - a;
}

//Stores the coordinates of the line from (x1,y1) to (x2,y2)
void storeLineArray(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{

  LCount = 0;
  uint8_t deltax, deltay, X, Y, steep;
  int8_t error, ystep;
  if (x1 >= 128) x1 = x1 % 128;
  if (x2 >= 128) x2 = x2 % 128;
  if (y1 >= 64) y1 = y1 % 64;
  if (y2 >= 64) y1 = y1 % 64;

  steep = absDiff(y1, y2) > absDiff(x1, x2);

  if (steep)
  {
    //swap(x1, y1)
    int t = x1;
    x1 = y1;
    y1 = t;
    //swap(x2, y2)
    t = x2;
    x2 = y2;
    y2 = t;
  }

  if (x1 > x2)
  {
    //swap(x1, x2)
    int t = x1;
    x1 = x2;
    x2 = t;
    //swap(y1, y2)
    t = y1;
    y1 = y2;
    y2 = t;
  }

  deltax = x2 - x1;
  deltay = absDiff(y2, y1);
  error = deltax / 2;
  Y = y1;
  if (y1 < y2) ystep = 1;  else ystep = -1;

  for (X = x1; X <= x2; X++)
  {
    if (steep)
    {
      Lx[LCount] = Y;
      Ly[LCount] = X;
    }
    else
    {
      Lx[LCount] = X;
      Ly[LCount] = Y;
    }
    LCount = LCount + 1;
    error = error - deltay;
    if (error < 0)
    {
      Y = Y + ystep;
      error = error + deltax;
    }
  }
}

//Stores the coordinates of the circle with center - (centerX,centerY) and radius - radius
void storeCircArray(int centreX, int centreY, int radius)
{
  curveCount = 0;
  int x2 = centreX - radius;
  int y2 = centreY - radius;
  int h = 2 * radius + 1;
  int w = 2 * radius + 1;
  int16_t tSwitch;
  uint8_t x1 = 0, y1 = radius;
  tSwitch = 3 - 2 * radius;

  while (x1 <= y1)
  {
    // upper left corner
    Cx[curveCount] = x2 + radius - x1;
    Cy[curveCount] = y2 + radius - y1;
    curveCount++;

    Cx[curveCount] = x2 + radius - y1;
    Cy[curveCount] = y2 + radius - x1;
    curveCount++;

    // upper right corner
    Cx[curveCount] = x2 + w - radius - 1 + x1;
    Cy[curveCount] = y2 + radius - y1;
    curveCount++;

    Cx[curveCount] = x2 + w - radius - 1 + y1;
    Cy[curveCount] = y2 + radius - x1;
    curveCount++;

    // lower right corner
    Cx[curveCount] = x2 + w - radius - 1 + x1;
    Cy[curveCount] = y2 + h - radius - 1 + y1;
    curveCount++;

    Cx[curveCount] = x2 + w - radius - 1 + y1;
    Cy[curveCount] = y2 + h - radius - 1 + x1;
    curveCount++;

    // lower left corner
    Cx[curveCount] = x2 + radius - x1;
    Cy[curveCount] = y2 + h - radius - 1 + y1;
    curveCount++;

    Cx[curveCount] = x2 + radius - y1;
    Cy[curveCount] = y2 + h - radius - 1 + x1;
    curveCount++;
    if (tSwitch < 0)
    {
      tSwitch += (4 * x1 + 6);
    }
    else
    {
      tSwitch += (4 * (x1 - y1) + 10);
      y1--;
    }
    x1++;
  }
  for (int i = 0; i < w - (2 * radius); i++)
  {
    Cx[curveCount] = x2 + radius + i;
    Cy[curveCount] = y2;
    curveCount++;
  }
  for (int i = 0; i < w - (2 * radius); i++)
  {
    Cx[curveCount] = x2 + radius + i;
    Cy[curveCount] = y2 + h - 1;
    curveCount++;
  }
  for (int i = 0; i < h - (2 * radius); i++)
  {
    Cx[curveCount] = x2;
    Cy[curveCount] = y2 + radius + i;
    curveCount++;
  }
  for (int i = 0; i < h - (2 * radius); i++)
  {
    Cx[curveCount] = x2 + w - 1;
    Cy[curveCount] = y2 + radius + i;
    curveCount++;
  }
}

//Stores the coordinates of the rectange with a point at (x0,y0) and of width w, height h
void storeRectArray(int x0, int y0, int w, int h)
{
  curveCount = 0;
  for (int i = 0; i < w; i++)
  {
    Cx[curveCount] = x0 + i;
    Cy[curveCount] = y0;
    curveCount++;
  }
  for (int i = 0; i < w; i++)
  {
    Cx[curveCount] = x0 + i;
    Cy[curveCount] = y0 + h - 1;
    curveCount++;
  }
  for (int i = 0; i < h; i++)
  {
    Cx[curveCount] = x0;
    Cy[curveCount] = y0 + i;
    curveCount++;
  }
  for (int i = 0; i < h; i++)
  {
    Cx[curveCount] = x0 + w - 1;
    Cy[curveCount] = y0 + i;
    curveCount++;
  }
}

//Draws circle with centre (centreX, centreY) and radius = radius
void drawCirc(int centreX, int centreY, int radius)
{
  int x2 = centreX - radius;
  int y2 = centreY - radius;
  int h = 2 * radius + 1;
  int w = 2 * radius + 1;
  int16_t tSwitch;
  uint8_t x1 = 0, y1 = radius;
  tSwitch = 3 - 2 * radius;

  while (x1 <= y1)
  {
    // upper left corner
    GLCD.SetDot(x2 + radius - x1, y2 + radius - y1, BLACK);
    GLCD.SetDot(x2 + radius - y1, y2 + radius - x1, BLACK);

    // upper right corner
    GLCD.SetDot(x2 + w - radius - 1 + x1, y2 + radius - y1, BLACK);
    GLCD.SetDot(x2 + w - radius - 1 + y1, y2 + radius - x1, BLACK);

    // lower right corner
    GLCD.SetDot(x2 + w - radius - 1 + x1, y2 + h - radius - 1 + y1, BLACK);
    GLCD.SetDot(x2 + w - radius - 1 + y1, y2 + h - radius - 1 + x1, BLACK);

    // lower left corner
    GLCD.SetDot(x2 + radius - x1, y2 + h - radius - 1 + y1, BLACK);
    GLCD.SetDot(x2 + radius - y1, y2 + h - radius - 1 + x1, BLACK);
    if (tSwitch < 0)
    {
      tSwitch += (4 * x1 + 6);
    }
    else
    {
      tSwitch += (4 * (x1 - y1) + 10);
      y1--;
    }
    x1++;
  }
  for (int i = 0; i < w - (2 * radius); i++)
  {
    GLCD.SetDot(x2 + radius + i, y2, BLACK);
  }
  for (int i = 0; i < w - (2 * radius); i++)
  {
    GLCD.SetDot(x2 + radius + i, y2 + h - 1, BLACK);
  }
  for (int i = 0; i < h - (2 * radius); i++)
  {
    GLCD.SetDot(x2 , y2 + radius + i, BLACK);
  }
  for (int i = 0; i < h - (2 * radius); i++)
  {
    GLCD.SetDot(x2 + w - 1 , y2 + radius + i, BLACK);
  }
}

//Initialises the LCDs and their Corresponding libraries
void InitLCDs(long delaytime) {
  GLCD.Init();
  lcd.begin(16, 2);

  InitScreen(delaytime);

  x = GLCD.CenterX; y = GLCD.CenterY;
  real_image[1][1] = 1;
  GLCD.ClearScreen();
  lcd.clear();

  lcd.print("Brush Size: 1x1 ");
  lcd.setCursor(0, 1);
  lcd.print("Mode: Default   ");
  GLCD.CursorTo(x, y);
}

//Creates the Welcome Screen for specified time (same meaning of the argument in InitLCDs function)
void InitScreen(long delaytime) {
  GLCD.DefineArea(textAreaTOP, TimesNewRoman16_bold);
  GLCD.DrawString("LCD Painter", gTextfmt_center, gTextfmt_center);
  GLCD.DefineArea(textAreaBOTTOM, Callibri10);
  GLCD.DrawString("Created by\n Reebhu Bhattacharyya\n Archit Bhatnagar", gTextfmt_left, gTextfmt_center);
  lcd.setCursor(0, 0);
  lcd.print("Welcome!!");
  lcd.setCursor(0, 1);
  lcd.print("Let's Paint");
  delay(delaytime);
}

//Sets the modes of all the pins and attachs the interrupts
void InitPins() {
  //pinMode(OnOffPin, INPUT);
  pinMode(PenLiftPin, INPUT);
  digitalWrite(PenLiftPin, HIGH);

  attachInterrupt(digitalPinToInterrupt(PenLiftPin), penLiftISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ErasePin), eraseModeISR, RISING);
  attachInterrupt(digitalPinToInterrupt(ClearPin), clearScreenISR, RISING);
  attachInterrupt(digitalPinToInterrupt(BrushSizeChangePin), brushSizeChangeISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RectPin), RectPinISR, RISING);
  attachInterrupt(digitalPinToInterrupt(CircPin), CircPinISR, RISING);
}
