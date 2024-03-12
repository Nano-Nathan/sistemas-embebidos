void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600,SERIAL_8N1);

  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0){
    String string_recibido=Serial.readStringUntil('\n');
    digitalWrite(13, string_recibido.toInt());

    analogWrite(13, string_recibido.toInt());
  }
}
