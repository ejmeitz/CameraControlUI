
// include the library code:
#include <LiquidCrystal.h>
int incomingByte = 0; // for incoming serial data
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  lcd.begin(16,2);
}

void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    String incoming = Serial.readString();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(incoming);
        delay(100);
    Serial.write("Success,");
  }
}
