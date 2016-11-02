const int buttonPin1 = 2;
const int buttonPin2 = 3;
const int buttonPin3 = 20;
const int buttonPin4 = 19;
const int buttonPin5 = 18;
const int buttonPin6 = 21;
const int ledPin1 = 8;
const int ledPin2 = 9;
const int ledPin3 = 10;
const int ledPin4 = 11;
const int ledPin5 = 14;
const int ledPin6 = 15;
const int ledPin7 = 16;
const int pin1 = 12;
bool lightUp = false;
bool light2 = false;
int val[6] = {0};
void setup() {
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  pinMode(buttonPin5, INPUT);
  pinMode(buttonPin6, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  pinMode(ledPin6, OUTPUT);
  pinMode(ledPin7, OUTPUT);
  pinMode(pin1, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin1), ISR1, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), ISR2, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonPin3), ISR3, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonPin4), ISR4, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonPin5), ISR5, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonPin6), ISR6, RISING);
  digitalWrite(pin1, HIGH);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(val[0]);
  Serial.print(",");
  Serial.print(val[1]);
  Serial.print(",");
  Serial.print(val[2]);
  Serial.print(",");
  Serial.print(val[3]);
  Serial.print(",");
  Serial.print(val[4]);
  Serial.print(",");
  Serial.print(val[5]);
  Serial.println(",");
  //  if(lightUp)
  //  {
  //    digitalWrite(ledPin1,HIGH);
  //    digitalWrite(ledPin2,HIGH);
  //    digitalWrite(ledPin3,HIGH);
  //    digitalWrite(ledPin4,HIGH);
  //  }
  //  else
  //  {
  //    digitalWrite(ledPin1,LOW);
  //    digitalWrite(ledPin2,LOW);
  //    digitalWrite(ledPin3,LOW);
  //    digitalWrite(ledPin4,LOW);
  //  }
  //  if(lightUp&&light2)
  //  {
  //    digitalWrite(ledPin5,HIGH);
  //    digitalWrite(ledPin6,LOW);
  //    digitalWrite(ledPin7,LOW);
  //    light2=!light2;
  //  }
  //  else if(lightUp && !light2)
  //  {
  //    digitalWrite(ledPin6,HIGH);
  //    digitalWrite(ledPin5,LOW);
  //    digitalWrite(ledPin7,LOW);
  //    light2=!light2;
  //  }
  //  else
  //  {
  //    digitalWrite(ledPin7,HIGH);
  //    digitalWrite(ledPin5,LOW);
  //    digitalWrite(ledPin6,LOW);
  //  }
  //  lightUp=!lightUp;
  delay(500);
}

void ISR1()
{
  val[0] = 1 - val[0];
}

void ISR2()
{
  val[1] = 1 - val[1];
}

void ISR3()
{
  val[2] = 1 - val[2];
}

void ISR4()
{
  val[3] = 1 - val[3];
}

void ISR5()
{
  val[4] = 1 - val[4];
}

void ISR6()
{
  val[5] = 1 - val[5];
}


