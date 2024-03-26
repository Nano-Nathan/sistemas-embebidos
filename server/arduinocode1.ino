#include <Arduino_FreeRTOS.h>
//Manejador de las tarea
TaskHandle_t xTaskHandle = NULL;

//Variable que guarda la intensidad
int brightness = 0;
//Variable que 

//codigo para tarea que lee la intensidad de los leds
void readBrightness(){
  for ( ;; ) {
    //La lectura es seccion critica
    taskENTER_CRITICAL();
    brightness = analogRead(A3);
    taskEXIT_CRITICAL();
  }
}


//codigo para tarea que escribe la intensidad de los leds
void writeBrightness(){
  //Calcula 3 segundos en tiks
  float time = 3000 / portTICK_PERIOD_MS;

  for ( ;; ) {
    //Espera 3 segundos para escribir
    vTaskDelay(time);

    //La escritura es seccion critica
    taskENTER_CRITICAL();
    Serial.println( brightness );
    taskEXIT_CRITICAL();
  }
}

//Metodo para hacer parpadear un led
void blink (int id) {
  digitalWrite(id, HIGH);
  digitalWrite(id, LOW);
}

//codigo para tarea que hace parpadear el led 12
void blink12 () {
  //Calcula 0.5 segundos en tiks
  float time = 500 / portTICK_PERIOD_MS;

  for ( ;; ) {
    //Hace parpadear el led cada 0.5 seg
    blink(12)
    vTaskDelay(time);
  }
}

//codigo para tarea que hace parpadear el led 11
void blink11 () {
  //Calcula 1 segundos en tiks
  float time = 1000 / portTICK_PERIOD_MS;

  for ( ;; ) {
    //Hace parpadear el led cada 1 seg
    blink(11)
    vTaskDelay(time);
  }
}




void setup() {
  Serial.begin(9600, SERIAL_8N1);

  //Manejador de las tareas
  TaskHandle_t xTaskReadBrightness = NULL,
    xTaskWriteBrightness = NULL,
    xTaskBlink12 = NULL;

  //Pin para captar el brillo
  pinMode(A3, INPUT);

  //Pines que parpadearan
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  //Crea la tarea que lee la intensidad del brillo (High Water Mark: 69)
  xTaskCreate(readBrightness, "lectorIntensidad", 75, NULL, 0, &xTaskReadBrightness);

  //Crea la tarea que escribe la intensidad del brillo (High Water Mark: 62)
  xTaskCreate(writeBrightness, "escritorIntensidad", 70, NULL, 0, &xTaskWriteBrightness);

  //Crea la tarea que hace parpadear el led 12 (High Water Mark: 89)
  xTaskCreate(blink12, "parpadear12", 95, NULL, 0, &xTaskBlink12);

  // xTaskCreate(tarea,"miPrimeraTarea",1000,(void*)&pin,10,NULL);
  // xTaskCreate(TaskBlink, "Blink", 128, NULL, 0, NULL);
}

void loop() {
  //Obtiene el tamaño de la pila
  // UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(xTaskHandle);
  // Serial.print("High Water Mark: ");
  // Serial.println(uxHighWaterMark);
}