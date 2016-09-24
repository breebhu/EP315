#include<openGLCD.h>


// int buttonPin = ;
 int joyPin1 = A5;                 // slider variable connecetd to analog pin 0
 int joyPin2 = A4;
 int SWPin=3; 
 // slider variable connecetd to analog pin 1
 int value1 = 0;                  // variable to read the value from the analog pin 0
 int value2 = 0;   
 int value3 = -1;
 bool penLift=false;
 // variable to read the value from the analog pin 1
 bool up=false,down=false,left=false,right=false;
 int t_up=1000,t_down=10,t_right=1000,t_left=10;
 int x=0,y=0;
 int cursor_x = 0,cursor_y = 0;
 void setup() {
  GLCD.Init();
  GLCD.ClearScreen();
  GLCD.SelectFont(cp437font8x8);
  GLCD.CursorTo(x,y);
  pinMode(joyPin1,INPUT);
  pinMode(joyPin2,INPUT);
  pinMode(SWPin,INPUT);
  digitalWrite(SWPin,HIGH);
  Serial.begin(9600);
 }

 void loop() {
  int prev_cur_x = cursor_x, prev_cur_y = cursor_y;
  delay(50);
  value1 = analogRead(joyPin1);  
  delay(10);            
  value2 = analogRead(joyPin2);
  value3 = digitalRead(SWPin); 
  if(value1>985)
  {
    right=false;
    x=x+1;
    if(x>127)x=0;
  }
  else if(value1<10)
  {
    right=false;
    x=x-1;
    if(x<0)x=127;
  }
  if(value2<10)
  {
    up=true;
    y=y+1;
    if(y>63)y=0;
  }
  else if(value2>985)
  {
    up=false;
    y=y-1;
    if(y<0)y=63;
  }
  if(value3==0)
  {
    penLift=!(penLift);
  }
  
  cursor_x = x; cursor_y = y;
  GLCD.SetDot(prev_cur_x,prev_cur_y,WHITE);
  GLCD.SetDot(prev_cur_x+1,prev_cur_y,WHITE);
  GLCD.SetDot(prev_cur_x-1,prev_cur_y,WHITE);
  GLCD.SetDot(prev_cur_x,prev_cur_y+1,WHITE);
  GLCD.SetDot(prev_cur_x,prev_cur_y-1,WHITE);

  GLCD.SetDot(cursor_x,cursor_y,BLACK);
  GLCD.SetDot(cursor_x+1,cursor_y,BLACK);
  GLCD.SetDot(cursor_x-1,cursor_y,BLACK);
  GLCD.SetDot(cursor_x,cursor_y+1,BLACK);
  GLCD.SetDot(cursor_x,cursor_y-1,BLACK);

  if(!penLift)
  GLCD.SetDot(x,y,BLACK);
  else 
  GLCD.SetDot(x,y,WHITE);
  Serial.print(value1);
  Serial.print(",");
  Serial.print(value2);
  Serial.print(",");
  Serial.println((value3));
 }
