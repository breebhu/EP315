#include<openGLCD.h>
#include<math.h>

// int buttonPin = ;
int joyPin1 = A5;                 // slider variable connecetd to analog pin 0
int joyPin2 = A4;
int SWPin = 3;
// slider variable connecetd to analog pin 1
int value1 = 0;                  // variable to read the value from the analog pin 0
int value2 = 0;
int value3 = -1;
bool penLift = false;
// variable to read the value from the analog pin 1
int x, y;
int cursor_x, cursor_y;
bool Painting[64][64] = {0};

void setup() {
  GLCD.Init();
  x = GLCD.CenterX; y = GLCD.CenterY;
  cursor_x = GLCD.CenterX; cursor_y = GLCD.CenterY;
  GLCD.ClearScreen();
  GLCD.CursorTo(x, y);
  pinMode(joyPin1, INPUT);
  pinMode(joyPin2, INPUT);
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);
  Serial.begin(9600);
}

void loop() {

  //delay(150);
  value1 = analogRead(joyPin1);
  delay(10);
  value2 = analogRead(joyPin2);
  value3 = digitalRead(SWPin);
  if (value2 > 985)
  {
    x = x + 1;
    if (x > 63)x = 0;
  }
  else if (value2 < 10)
  {
    x = x - 1;
    if (x < 0)x = 63;
  }
  if (value1 < 10)
  {
    y = y + 1;
    if (y > 63)y = 0;
  }
  else if (value1 > 985)
  {
    y = y - 1;
    if (y < 0)y = 63;
  }
  if (value3 == 0)
  {
    penLift = !(penLift);
  }

  Painting[x][y] = 1;
  if (Painting[cursor_x][cursor_y] == 0) {
    GLCD.SetDot(cursor_x, cursor_y, WHITE);
  }
  if (Painting[cursor_x + 1][cursor_y] == 0) {
    GLCD.SetDot(cursor_x + 1, cursor_y, WHITE);
  }
  if (Painting[cursor_x - 1][cursor_y] == 0) {
    GLCD.SetDot(cursor_x - 1, cursor_y, WHITE);
  }
  if (Painting[cursor_x][cursor_y + 1] == 0) {
    GLCD.SetDot(cursor_x, cursor_y + 1, WHITE);
  }
  if (Painting[cursor_x][cursor_y - 1] == 0) {
    GLCD.SetDot(cursor_x, cursor_y - 1, WHITE);
  }

  cursor_x = x; cursor_y = y;

  GLCD.SetDot(cursor_x, cursor_y, BLACK);
  GLCD.SetDot(cursor_x + 1, cursor_y, BLACK);
  GLCD.SetDot(cursor_x - 1, cursor_y, BLACK);
  GLCD.SetDot(cursor_x, cursor_y + 1, BLACK);
  GLCD.SetDot(cursor_x, cursor_y - 1, BLACK);

  //if(!penLift)
  GLCD.SetDot(x, y, BLACK);
  //else
  //GLCD.SetDot(x,y,WHITE);
  // Serial.print(",");
  //  Serial.println(cursor);
}
