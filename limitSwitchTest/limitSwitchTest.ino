const int LimitPin = 0;
int flag = 0;

void setup() {
  pinMode(LimitPin,INPUT_PULLUP);
  Serial.begin(9600);

}

void loop() {
  if(digitalRead(LimitPin) == LOW && flag == 0){
    Serial.println("Closed");
    flag = 1;
    delay(20);
  }
   if(digitalRead(LimitPin) == HIGH && flag == 1){
    Serial.println("Opened");
    flag = 0;
    delay(20);
  }
}
