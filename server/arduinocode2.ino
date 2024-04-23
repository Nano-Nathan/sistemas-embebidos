#include <Arduino_FreeRTOS.h>

//Fecha inicial: 23/04/24
long int time = 758851200;

char buffer_date[20];

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  sprintf(
    buffer_date,
    "%02ld/%02ld/%02ld-%02ld/%02ld/%02ld\n",
    (time / 86400) % 30, //day
    (time / 2592000) % 12, //month
    time / 31104000, //year
    (time / 3600) % 24, //hour
    (time / 60) % 60, //min
    time % 60 //sec
  );
  Serial.println(buffer_date);
}

void loop() {}