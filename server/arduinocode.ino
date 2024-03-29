void setup() {
  Serial.begin(9600, SERIAL_8N1);

  //Pins para controlar brillo
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  //Pin para captar el brillo
  pinMode(A3, INPUT);
}

void loop() {
  //Obtiene el valor leido en el pin A3
  int reader = analogRead(A3);
  if(Serial.available() > 0){
    //Lee el id del led
    int idLed = Serial.readStringUntil('\n').toInt();
    //Obtiene el valor para el led
    int value = Serial.readStringUntil('\n').toInt();


    //Acciones segun id led
    if (8 < idLed && idLed < 12) {
      analogWrite(idLed, value);
    } else {
      if (idLed == 13) {
        digitalWrite(idLed, value);
      }
    }
  }
  Serial.println( reader );
}