const int VRxPin= 5; //VRx pin connected to arduino pin A0
const int VRyPin= 4; //VRy pin connected to arduino in A1
const int SwButtonPin=3; //SW pin connected to arduino pin D8

int pressed; //this variable will determine whether joystick has been pressed down (selected)
int x= -1;//this variable will hold the X-coordinate value
int y= -1; //this variable will hold the Y-coordinate value

void readJoystick() {

}

void setup() {
pinMode(SwButtonPin, INPUT);//sets the SW switch as input
pinMode(VRyPin,INPUT);
digitalWrite(SwButtonPin, HIGH);//sets the SW button HIGH
Serial.begin(9600);//sets the baud rate
}

void loop() {
//readJoystick();//calls this function which reads the digital input button SW, the X-coordinate and the Y-coordinate
pressed= digitalRead(SwButtonPin);//reads whether joystick has been pressed down (selected) or not
x= analogRead(VRxPin);//reads the X-coordinate value
y= analogRead(VRyPin);//reads the Y-coordinate value
Serial.print("X: ");
Serial.println(x);//prints the X-coordinate
Serial.print("Y: ");
Serial.println(y);//prints the Y-coordinate
Serial.print (" Pressed: ");
Serial.println(pressed);//prints whether joystick knob has been pressed or not
delay(10);
}

