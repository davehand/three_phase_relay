int relay=53;
int cross=24;
bool state;
bool prestate=0;

void setup() {
  pinMode(relay,OUTPUT);
  pinMode(cross,INPUT);
 // Serial.begin(9600);


}

void loop() {
state=digitalRead(cross);
if(state!=prestate){
 // Serial.println(millis());
}
digitalWrite(relay,!state);

prestate=state;
  
}
