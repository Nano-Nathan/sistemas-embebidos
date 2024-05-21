#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>

//Manejador de las tareas
TaskHandle_t xTaskUpdateTime = NULL;
TaskHandle_t xTaskReadBuffer = NULL;

//Fecha inicial: 23/04/24
long int time = 758851200;

//Posicion de memoria inicial
int position = 2;

bool isCleaning = false;

//Código de la tarea que actualiza el contador
void updateTime () {
  //Contador de tiks
  TickType_t xLastWakeTime = xTaskGetTickCount(), xTimeIncrement = pdMS_TO_TICKS(1000);

  for ( ;; ) {
    //Aumenta el contador
    time++;

    //Espera por un segundo
    vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
  }
  vTaskDelete(NULL);
}

void actionPin2 () {
  writeDate(2);
}
void actionPin3 () {
  writeDate(3);
}

//Código de la interrupción que escribe en la EEPROM
void writeDate (int eventCall) {
  if (!isCleaning){
    // Escribir el evento ocurrido en la EEPROM
    EEPROM.put(position, eventCall);
    // Escribir el valor en la EEPROM
    EEPROM.put(position + 2, time);

    // Actualizar la posición de memoria para apuntar a la siguiente posición disponible
    position += 6;

    //Guarda la proxima posicion donde escribir
    EEPROM.put(0, position);
  }
}

//codigo para leer el buffer y escribir en la EEPROM o setear la fecha
void readBuffer () {
  int buffer;

  for ( ;; ){
    //Revisa si hay datos en el buffer
    // (0) para setear el tiempo;
    // (1) para guardar en la EEPROM;
    // (2) para obtener los datos;
    // (3) para limpiar la EEPROM;
    if(Serial.available() > 0){
      buffer = Serial.readStringUntil('\n').toInt();

      //Setea el tiempo
      if(buffer == 0){
        time = Serial.readStringUntil('\n').toInt();
      //Escribe el dato
      } else if (buffer == 1){
        writeDate(1);
      //Obtiene los datos
      } else if (buffer == 2){
        readEEPROM();
      //Limpia la EEPROM
      } else if (buffer == 3){
        clearEEPROM();
      }
    }
  }
  vTaskDelete(NULL);
}

void readEEPROM () {
  long int currentTime;
  int lastEvent = 1,
    currentPosition = 2;
  String result = "";

  //Obtiene el evento ocurrido
  EEPROM.get(currentPosition, lastEvent);
  //Obtiene el dato
  EEPROM.get(currentPosition + 2, currentTime);

  while (1 <= lastEvent && lastEvent <= 3) {
    //Lo mapea y almacena
    result += String(lastEvent) + ";" + String(currentTime) + ";";

    //Obtiene el siguiente evento ocurrido
    currentPosition += 6;

    //Obtiene el evento ocurrido
    EEPROM.get(currentPosition, lastEvent);
    //Obtiene el dato
    EEPROM.get(currentPosition + 2, currentTime);
  }

  Serial.println(result);
}

void clearEEPROM() {
  isCleaning = true;
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  position = 2;
  isCleaning = false;
  Serial.println("Limpia");
}


//LA ULTIMA POSICIÓN DE MEMORIA UTILIZADA SE GUARDA AL COMIENZO DE LA EEPROM
//ESTA UTILIZA LOS 2 PRIMEROS BYTES.
void setup() {
  Serial.begin(9600, SERIAL_8N1);

  // Si la eeprom contiene datos, obtiene la ultima posicion de memoria escrita, quien lo hizo y la fecha guardada
  if (EEPROM.read(0) != 255) {
    EEPROM.get(0, position);
    EEPROM.get(4, time);
  }

  //Crea eventos de interrupción para guardar la fecha en la eeprom
  attachInterrupt(digitalPinToInterrupt(2), actionPin2, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), actionPin3, FALLING);

  //Crea la tarea que aumenta el tiempo en segundos
  xTaskCreate(updateTime, "aumentaSegundos", 128, NULL, 0, &xTaskUpdateTime);
  //Crea la tarea que lee el buffer
  xTaskCreate(readBuffer, "leerBuffer", 128, NULL, 0, &xTaskReadBuffer);
}

void loop() {}