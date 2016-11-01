#include<openGLCD.h>
uint8_t Lx[128] = {0};
uint8_t Ly[128] = {0};
bool L1[128] = {false};
bool L2[128] = {false};
bool L3[128] = {false};
int LCount = 0;
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
int CircPin=21;
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
volatile long lastCircMode=0;
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
  attachInterrupt(digitalPinToInterrupt(CircPin),CircPinISR,RISING);
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
  Serial.print(rectMode);
  Serial.print(" ");
  Serial.println(nextRectMode);
  Serial.print(digitalRead(20));
  Serial.print(":");
  Serial.println(millis());
  if (rectMode)
  {
    //print real image
    for (int i = 0; i < LCount; i++)
    {
      int x0 = Lx[i];
      int y0 = Ly[i];
      if (L2[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
      //      if(isSteep)
      //      {
      //        //Serial.println("Entered steep:");
      //        x0=(Lx[i]-1)%128;
      //        if(L1[i]==true) GLCD.SetDot(x0,y0,BLACK);
      //        x0=(Lx[i]+1)%128;
      //        if(L3[i]==true) GLCD.SetDot(x0,y0,BLACK);
      //      }
      //      else
      //      {
      //        //Serial.println("Entered:");
      //        y0=(Ly[i]-1)%64;
      //        if(L1[i]==true) GLCD.SetDot(x0,y0,BLACK);
      //        y0=(Ly[i]+1)%64;
      //        if(L3[i]==true) GLCD.SetDot(x0,y0,BLACK);
      //      }
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
      //storeArray
      //Serial.print("Entered:");
      //Serial.println(LCount);
      LCount = storeArray(rx1, ry1, rx2, ry2);
      //      isSteep=absDiff(ry2,ry1)>absDiff(rx2,rx1);
      for (int i = 0; i < LCount; i++)
      {
        int x0 = Lx[i];
        int y0 = Ly[i];
        if (ActualReadData(x0, y0) == BLACK) L2[i] = true; else L2[i] = false;
        //        if(isSteep)
        //        {
        //          x0=(Lx[i]-1)%128;
        //          y0=Ly[i];
        //          if(ActualReadData(x0,y0)==BLACK) L1[i]=true; else L1[i]=false;
        //          x0=(Lx[i]+1)%128;
        //          y0=Ly[i];
        //          if(ActualReadData(x0,y0)==BLACK) L3[i]=true; else L3[i]=false;
        //        }
        //        else
        //        {
        //          x0=Lx[i];
        //          y0=(Ly[i]-1)%64;
        //          if(ActualReadData(x0,y0)==BLACK) L1[i]=true; else L1[i]=false;
        //          x0=Lx[i];
        //          y0=(Ly[i]+1)%64;
        //          if(ActualReadData(x0,y0)==BLACK) L3[i]=true; else L3[i]=false;
        //        }
      }
      GLCD.DrawLine(rx1, ry1, rx2, ry2);
    }
  }
  if (circMode)
  {
    //print real image
    for (int i = 0; i < LCount; i++)
    {
      int x0 = Lx[i];
      int y0 = Ly[i];
      if (L2[i] == true) GLCD.SetDot(x0, y0, BLACK);
      else GLCD.SetDot(x0, y0, WHITE);
    }
    if (nextCircMode)
    {
      int w = absDiff(rx1, rx2)+1;
      int h = absDiff(ry1, ry2)+1;
      int rad=(int)(sqrt(w*w+h*h));
      GLCD.DrawCircle(rx1,ry1,rad);
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
      LCount = storeArray(rx1, ry1, rx2, ry2);
      for (int i = 0; i < LCount; i++)
      {
        int x0 = Lx[i];
        int y0 = Ly[i];
        if (ActualReadData(x0, y0) == BLACK) L2[i] = true; else L2[i] = false;
      }
      GLCD.DrawLine(rx1, ry1, rx2, ry2);
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
  if (millis() - lastRectMode > debounceTime)
  {
    Serial.print("ISR:");
    Serial.print(millis());
    Serial.print(",");
    Serial.println(lastRectMode);
    if (rectMode)
    {
      nextRectMode = true;
    }
    else
    {
      rectMode = true;
      nextRectMode = false;
      prevRectMode = false;
    }
    lastRectMode = millis();
  }
}
void CircPinISR()
{
  if (millis() - lastCircMode > debounceTime)
  {
    Serial.print("ISR:");
    Serial.print(millis());
    Serial.print(",");
    Serial.println(lastCircMode);
    if (circMode)
    {
      nextCircMode = true;
    }
    else
    {
      circMode = true;
      nextCircMode = false;
      prevCircMode = false;
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
