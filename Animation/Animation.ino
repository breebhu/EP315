#include<openGLCD.h>
uint8_t Lx[128] = {0};
uint8_t Ly[128] = {0};

uint8_t Cx[390] = {0};
uint8_t Cy[390] = {0};

bool L[128] = {false};
bool C[390] = {false};

volatile int LCount = -1;
volatile int curveCount = -1;
volatile bool rectMode = false, nextRectMode = false, prevRectMode = false;
volatile bool circMode = false, nextCircMode = false, prevCircMode = false;
bool isSteep = false;
int joyPin1 = A5;                 // slider variable connecetd to analog pin 0
int joyPin2 = A4;
int SWPin = 19; // interchanged with BSCP=2
int ErasePin = 3;
int ClearPin = 18;
int BrushSizeChangePin = 2;
int RectPin = 20;
int CircPin = 21;
// slider variable connecetd to analog pin 1
int value1 = 0;                  // variable to read the value from the analog pin 0
int value2 = 0;
volatile bool penLift = false;
volatile bool eraseMode = false;
volatile bool clearScreen = false;
volatile int brushType = 0;
// variable to read the value from the analog pin 1
int x, y;
int rx1, rx2, ry1, ry2;
const int debounceTime = 1000;
volatile long lastRectMode = 0; //
volatile long lastCircMode = 0;
int real_image[5][5] = {0};
void setup() {
  GLCD.Init();
  pinMode(joyPin1, INPUT);
  pinMode(joyPin2, INPUT);
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);
  pinMode(ErasePin, INPUT);

  // attachInterrupt(digitalPinToInterrupt(SWPin), penLiftISR, FALLING);
  //attachInterrupt(digitalPinToInterrupt(ErasePin), eraseModeISR, RISING);
  //attachInterrupt(digitalPinToInterrupt(ClearPin), clearScreenISR, RISING);
  //attachInterrupt(digitalPinToInterrupt(BrushSizeChangePin), brushSizeChangeISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RectPin), RectPinISR, RISING);
  attachInterrupt(digitalPinToInterrupt(CircPin), CircPinISR, RISING);
  x = GLCD.CenterX; y = GLCD.CenterY;
  real_image[1][1] = 1;
  GLCD.ClearScreen();
  GLCD.CursorTo(x, y);

  Serial.begin(9600);
}

void loop() {
  delay(150);
  value1 = analogRead(joyPin1);
  delay(10);
  value2 = analogRead(joyPin2);

  if (value2 > 900)
  {
    x = x + 1;
    if (x > 127)x = 0;
  }
  else if (value2 < 100)
  {
    x = x - 1;
    if (x < 0)x = 127;
  }
  if (value1 < 100)
  {
    y = y + 1;
    if (y > 63)y = 0;
  }
  else if (value1 > 900)
  {
    y = y - 1;
    if (y < 0)y = 63;
  }
  // put your main code here, to run repeatedly:
  //  Serial.print(rectMode);
  //  Serial.print(" ");
  //  Serial.println(nextRectMode);
  //  Serial.print(digitalRead(20));
  //  Serial.print(":");
  //  Serial.println(millis());
  if (rectMode)
  {
    //print real image
    //    for (int i = 0; i < LCount; i++)
    //    {
    //      int x0 = Lx[i];
    //      int y0 = Ly[i];
    //      if (L[i] == true) GLCD.SetDot(x0, y0, BLACK);
    //      else GLCD.SetDot(x0, y0, WHITE);
    //    }
    for (int i = 0; i < curveCount; i++)
    {
      int x0 = Cx[i];
      int y0 = Cy[i];
      if (C[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
    }
    if (nextRectMode)
    {
      int w = absDiff(rx1, rx2) + 1;
      int h = absDiff(ry1, ry2) + 1;
      if (rx2 == rx1 || ry2 == ry1) GLCD.DrawLine(rx1, ry1, rx2, ry2);
      else if (rx2 > rx1 && ry2 > ry1)GLCD.DrawRect(rx1, ry1, w, h);
      else if (rx2 < rx1 && ry2 > ry1)GLCD.DrawRect(rx2, ry1, w, h);
      else if (rx2 > rx1 && ry2 < ry1)GLCD.DrawRect(rx1, ry2, w, h);
      else if (rx2 < rx1 && ry2 < ry1)GLCD.DrawRect(rx2, ry2, w, h);
      rectMode = false;
      nextRectMode = false;
      prevRectMode = true;
    }
    else
    {
      if (prevRectMode == false)
      {
        rx1 = x;
        ry1 = y;
        prevRectMode = true;
      }
      rx2 = x;
      ry2 = y;
      int w = absDiff(rx1, rx2) + 1 ;
      int h = absDiff(ry1, ry2) + 1;
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
      //      LCount = storeArray(rx1, ry1, rx2, ry2);
      //      for (int i = 0; i < LCount; i++)
      //      {
      //        int x0 = Lx[i];
      //        int y0 = Ly[i];
      //        if (ActualReadData(x0, y0) == BLACK) L[i] = true; else L[i] = false;
      //      }
      for (int i = 0; i < curveCount; i++)
      {
        int x0 = Cx[i];
        int y0 = Cy[i];
        if (ActualReadData(x0, y0) == BLACK) C[i] = true; else C[i] = false;
      }
      //      GLCD.DrawLine(rx1, ry1, rx2, ry2);
      if (rx2 == rx1 || ry2 == ry1) GLCD.DrawLine(rx1, ry1, rx2, ry2);
      else if (rx2 > rx1 && ry2 > ry1)GLCD.DrawRect(rx1, ry1, w, h);
      else if (rx2 < rx1 && ry2 > ry1)GLCD.DrawRect(rx2, ry1, w, h);
      else if (rx2 > rx1 && ry2 < ry1)GLCD.DrawRect(rx1, ry2, w, h);
      else if (rx2 < rx1 && ry2 < ry1)GLCD.DrawRect(rx2, ry2, w, h);
    }
  }
  Serial.print(rectMode);
  Serial.print(",");
  Serial.println(circMode);
  if (circMode)
  {

    //print real image
    for (int i = 0; i < LCount; i++)
    {
      int x0 = Lx[i] % 128;
      int y0 = Ly[i] % 64;
      if (L[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
    }
    for (int i = 0; i < curveCount; i++)
    {
      int x0 = Cx[i] % 128;
      int y0 = Cy[i] % 64;
      if (C[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
    }
    if (nextCircMode)
    {
      int w = absDiff(rx1, rx2) + 1;
      int h = absDiff(ry1, ry2) + 1;
      if (w == 1 && h == 1) GLCD.SetDot(rx1, ry1, BLACK);
      else
      {
        int rad = (int)(sqrt(w * w + h * h));
        drawCirc(rx1, ry1, rad);
      }
      x = rx1;
      y = ry1;
      circMode = false;
      nextCircMode = false;
      prevCircMode = true;
    }
    else
    {
      if (prevCircMode == false)
      {
        rx1 = x;
        ry1 = y;
        prevCircMode = true;
      }
      rx2 = x;
      ry2 = y;
      int w = absDiff(rx1, rx2) + 1;
      int h = absDiff(ry1, ry2) + 1;
      int rad = (int)(sqrt(w * w + h * h));
      LCount = storeArray(rx1, ry1, rx2, ry2);
      storeCircArray(rx1, ry1, rad);

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
      GLCD.DrawLine(rx1, ry1, rx2, ry2);
      drawCirc(rx1, ry1, rad);
    }
  }
}
int absDiff(int a, int b)
{
  return a > b ? a - b : b - a;
}

int storeArray(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{

  int lCount = 0;
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
      Lx[lCount] = Y;
      Ly[lCount] = X;
    }
    else
    {
      Lx[lCount] = X;
      Ly[lCount] = Y;
    }
    lCount = lCount + 1;
    error = error - deltay;
    if (error < 0)
    {
      Y = Y + ystep;
      error = error + deltax;
    }
  }
  //  Serial.print("Entered store");
  //  Serial.print(x1);
  //  Serial.print(",");
  //  Serial.print(y1);
  //  Serial.print(",");
  //  Serial.print(x2);
  //  Serial.print(",");
  //  Serial.print(y2);
  //  Serial.print(",");
  //  Serial.println(lCount);
  return lCount;
}

void RectPinISR()
{
  if (circMode) return;
  if (millis() - lastRectMode > debounceTime)
  {
    //    Serial.print("ISR:");
    //    Serial.print(millis());
    //    Serial.print(",");
    //    Serial.println(lastRectMode);
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
    //    Serial.print("ISR:");
    //    Serial.print(millis());
    //    Serial.print(",");
    //    Serial.println(lastCircMode);
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

int ActualReadData(int x, int y) {
  uint8_t bitarray[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
  GLCD.GotoXY(x, y);
  uint8_t data = GLCD.ReadData();
  if (data & bitarray[y % 8]) return BLACK;
  else return WHITE;
}
//DrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color)
//DrawCircle(uint8_ts xCenter, uint8_t yCenter, uint8_t radius, uint8_t color)
//DrawRoundRect(xCenter-radius, yCenter-radius, 2*radius+1, 2*radius+1, radius, color);
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

