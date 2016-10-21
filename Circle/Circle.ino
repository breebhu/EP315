#include<openGLCD.h>
int Lx[]={0};
int Ly[]={0};
bool L1x[]={0};
bool L2x[]={0};
bool L3x[]={0};
bool L1y[]={0};
bool L2y[]={0};
bool L3y[]={0};
int LCount=0;
void setup() {
  GLCD.Init();
  storeArray(0,0,25,64);
  GLCD.DrawLine(0,0,25,64);
  Serial.begin(9600);
  for(int j=0;j<=LCount;j++) 
  {Serial.print(Lx[j]);Serial.print(",");Serial.println(Ly[j]);}
}

void loop() {
  // put your main code here, to run repeatedly:
  //  storeArray()
  // draw()
  // replaceArray()    
}
int absDiff(int x,int y)
{
  return x>y?x-y:y-x;
}

int swap(int x, int y)
{
  int t=x;
  x=y;
  y=t;
}
void storeArray(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
LCount=0;
uint8_t deltax, deltay, x,y, steep;
int8_t error, ystep;
  if(x1>=128) x1=x1%128;
  if(x2>=128) x2=x2%128;
  if(y1>=64) y1=y1%64;
  if(y2>=64) y1=y1%64;

  steep = absDiff(y1,y2) > absDiff(x1,x2);  

  if (steep)
  {
    swap(x1, y1);
    swap(x2, y2);
  }

  if (x1 > x2)
  {
    swap(x1, x2);
    swap(y1, y2);
  }

  deltax = x2 - x1;
  deltay =absDiff(y2,y1);  
  error = deltax / 2;
  y = y1;
  if(y1 < y2) ystep = 1;  else ystep = -1;

  for(x = x1; x <= x2; x++)
  {
    if (steep) 
    {
      Lx[LCount]=y;
      Ly[LCount]=x;
    }
    else
    {
      Lx[LCount]=x;
      Ly[LCount]=y;
    }
    LCount=LCount+1;
    error = error - deltay;
    if (error < 0)
    {
      y = y + ystep;
      error = error + deltax;
    }
  }
}

