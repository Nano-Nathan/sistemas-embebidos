#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>

//Manejador de las tareas
TaskHandle_t xTaskUpdateTime = NULL;

//Fecha inicial: 23/04/24
long int time = 758851200;

//Posicion de memoria
int position = 2;

//Char para las fechas
char buffer_date[18] = "23/04/24-00:00:00";
char day[3] = "23";
char month[3] = "04";
char year[3] = "24";
char hour[3] = "00";
char minutes[3] = "00";
char seconds[3] = "00";

void toString () {
  //day
  sprintf(day,"%02d", (time / 86400) % 30);
  //month
  sprintf(month,"%02d", (time / 2592000) % 12);
  //year
  sprintf(year,"%02d", time / 31104000);
  //hour
  sprintf(hour,"%02d", (time / 3600) % 24);
  //minutes
  sprintf(minutes,"%02d", (time / 60) % 60);
  //seconds
  sprintf(seconds,"%02d", time % 60);

  //result
  sprintf(buffer_date, "%s/%s/%s-%s:%s:%s", day, month, year, hour, minutes, seconds); 
}

void toInt () {
  time = 
    atoi(day) * 86400 +       //day
    atoi(month) * 2592000 +   //month
    atoi(year) * 31104000 +   //year
    atoi(hour) * 3600 +       //hour
    atoi(minutes) * 60 +      //min
    atoi(seconds);            //sec
}

//Código de la tarea que actualiza el contador
void updateTime () {
  //Contador de tiks
  TickType_t xLastWakeTime = xTaskGetTickCount(), xTimeIncrement = pdMS_TO_TICKS(1000);

  for ( ;; ) {
    //Aumenta el contador
    time++;

    //Envia el dato
    toString();
    Serial.println(buffer_date);

    //Espera por un segundo
    vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
  }
  vTaskDelete(NULL);
}

//Código de la interrupción que escribe en la EEPROM
void writeDate () {
  // Escribir el valor en la EEPROM
  EEPROM.put(2, time);

  // Actualizar la posición de memoria para apuntar a la siguiente posición disponible
  position += 4;

  //Guarda la proxima posicion donde escribir
  EEPROM.put(0, position);
}

//codigo para leer el buffer y escribir en la EEPROM o setear la fecha
void readBuffer () {
  int buffer;

  for ( ;; ){
    //Revisa si hay datos en el buffer, (0) para setear el tiempo; (1) para guardar en la EEPROM
    if(Serial.available() > 0){
      buffer = Serial.readStringUntil('\n').toInt();

      //Setea el tiempo
      if(buffer == 0){
        time = Serial.readStringUntil('\n').toInt();
      //Escribe el dato
      } else {
        writeDate();
      }
    }
  }
  vTaskDelete(NULL);
}

//LA ULTIMA POSICIÓN DE MEMORIA UTILIZADA SE GUARDA AL COMIENZO DE LA EEPROM
//ESTA UTILIZA LOS 2 PRIMEROS BYTES.
void setup() {
  Serial.begin(9600, SERIAL_8N1);

  // Si la eeprom contiene datos, obtiene la ultima posicion de memoria escrita y la fecha guardada
  if (EEPROM.read(0) != 255) {
    EEPROM.get(0, position);
    EEPROM.get(2, time);
  }

  //Crea eventos de interrupción para guardar la fecha en la eeprom
  attachInterrupt(digitalPinToInterrupt(2), writeDate, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), writeDate, FALLING);

  //Crea la tarea que aumenta el tiempo en segundos
  xTaskCreate(updateTime, "aumentaSegundos", 128, NULL, 0, &xTaskUpdateTime);
}

void loop() {}