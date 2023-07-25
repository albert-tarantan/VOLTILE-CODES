
int offset = 20;

void setup() {
pinMode(A0, INPUT);
pinMode(4, OUTPUT);

Serial.begin(9600);

}

void loop() {

int volt = analogRead(A0);
double voltage = map (volt,0,1023,0,2500) + offset;

voltage /=100;

if ( voltage > 15 ) {
  digitalWrite(4, HIGH);
} else {
  digitalWrite(4, LOW);
}
}