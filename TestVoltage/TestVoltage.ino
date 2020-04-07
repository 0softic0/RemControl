/*
 Name:		TestVoltage.ino
 Created:	07.04.2020 13:58:59
 Author:	Анатолий
*/

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
}

// the loop function runs over and over again until power down or reset
void loop() {
  Serial.println(analogRead(A7));
}
