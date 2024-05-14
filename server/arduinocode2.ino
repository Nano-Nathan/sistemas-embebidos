#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>

//Manejador de las tareas
TaskHandle_t xTaskUpdateTime = NULL;

//Fecha inicial: 23/04/24
long int time = 758851200;
long int lastDate = 0;

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

void updateTime () {
  //Contador de tiks
  TickType_t xLastWakeTime = xTaskGetTickCount(), xTimeIncrement = pdMS_TO_TICKS(1000);

  for ( ;; ) {
    //Aumenta el contador
    time++;

    //Espera por un segundo
    vTaskDelayUntil(&xLastWakeTime, xTimeIncrement);
  }
}

void writeDate () {
  Serial.println("fecha a guardar: " + String(time));

  Serial.println("escribe en: " + String(position));
  // Escribir el valor en la EEPROM byte por byte
  EEPROM.write(position, (byte)(time >> 24));
  EEPROM.write(position + 1, (byte)(time >> 16));
  EEPROM.write(position + 2, (byte)(time >> 8));
  EEPROM.write(position + 3, (byte)time);

  // Actualizar la posición de memoria para apuntar a la siguiente posición disponible
  position += 4;

  //Guarda la ultima posicion escrita
  saveLastPosition();
  getLastDate();
}

void saveLastPosition () {
  // Escribir la ultima posición utilizada en la primera posición de la EEPROM
  EEPROM.write(0, (byte)(position >> 8)); // Escribir el byte más significativo
  EEPROM.write(1, (byte)position); // Escribir el byte menos significativo
}

void getLastPosition () {
  position = (EEPROM.read(0) << 8) | EEPROM.read(1);
}

void getLastDate () {
  if(position > 5) {
    // Leer el valor de la EEPROM para verificar que se ha guardado correctamente
    lastDate = (EEPROM.read(2) << 24) | (EEPROM.read(3) << 16) | (EEPROM.read(4) << 8) | EEPROM.read(5);

    // Mostrar el valor leído en el puerto serie
    Serial.println("Ultima fecha guardada: " + String(lastDate));
  }
}


//LA ULTIMA POSICIÓN DE MEMORIA UTILIZADA SE GUARDA AL COMIENZO DE LA EEPROM
//ESTA UTILIZA LOS 2 PRIMEROS BYTES (0 Y 1).
void setup() {
  Serial.begin(9600, SERIAL_8N1);

  // Si la eeprom contiene datos, obtiene la ultima posicion de memoria escrita
  if (EEPROM.read(0) != 255) {
    getLastPosition();
  }

  //Crea eventos de interrupción para guardar la fecha en la eeprom
  attachInterrupt(digitalPinToInterrupt(2), writeDate, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), writeDate, FALLING);

  //Crea la tarea que aumenta el tiempo en segundos
  xTaskCreate(updateTime, "aumentaSegundos", 128, NULL, 0, &xTaskUpdateTime);
}

void loop() {

}