#include <Arduino_FreeRTOS.h>
//Manejador de las tareas
TaskHandle_t xTaskReadBrightness = NULL,
  xTaskReadBuffer = NULL,
  xTaskWriteBrightness = NULL,
  xTaskBlink12 = NULL,
  xTaskBlink11 = NULL;

//Variable que guarda la intensidad
int brightness = 0, buffer = 1;
//Avisa si la intensidad es mayor a 800
boolean isMaxBrightness = false, is12Active = false,
  //Avisa si se esta leyendo la intensidad de la luz
  isReaderActive = true;

//codigo para tarea que lee la intensidad de los leds
void readBrightness(){
  for ( ;; ) {
    //Revisa si hay datos en el buffer, solo se envian ordenes para activar (1) o desactivar (0) la tarea
    if(Serial.available() > 0){
      buffer = Serial.readStringUntil('\n').toInt();
      if (buffer == 0) {
        stopReading();
      }
    }

    //La lectura es seccion critica
    taskENTER_CRITICAL();
    brightness = analogRead(A3);
    taskEXIT_CRITICAL();

    isMaxBrightness = brightness > 800;

    //Si el brillo supera 800, activa la alarma, si no la estuviese
    if(isMaxBrightness && !is12Active){
      Serial.println( "active_12" );
      is12Active = true;
      vTaskResume(xTaskBlink12);
    }
  }
}


//codigo para leer el buffer y activar tarea
void readBuffer () {
  for ( ;; ){
    //Revisa si hay datos en el buffer, solo se envian ordenes para activar (1) o desactivar (0) la tarea
    if(Serial.available() > 0){
      buffer = Serial.readStringUntil('\n').toInt();
      if(buffer == 1){
        continueReading();
      }
    }
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
void blink (void * pvParameters) {
  //Obtengo el id del led a controlar
  int id = *(int*)pvParameters;
  //Boleano para no calcular al momento de comparaciones
  boolean is12 = (id == 12);

  //Tendra el tiempo que demora en parpadear el led
  float time;


  //Espera 1 seg para el led 11 y 0.5 seg para el led 12
  if(id == 11){
    time = pdMS_TO_TICKS( 1000 );
  } else {
    time = pdMS_TO_TICKS( 500 );
  }

  for ( ;; ) {
    //Si el brillo no supera 800, avisa a la aplicacion y suspende la tarea
    if(is12 && !isMaxBrightness){
      is12Active = false;
      Serial.println( "suspend_12" );
      vTaskSuspend(xTaskBlink12);
    }

    //Hace parpadear el led
    digitalWrite(id, HIGH);
    // vTaskDelay(1);
    digitalWrite(id, LOW);
    vTaskDelay(time);
  }
}

//Método que termina la lectura de la luminosidad
void stopReading () {
  vTaskResume(xTaskReadBuffer);
  vTaskSuspend(xTaskBlink11);
  vTaskSuspend(xTaskBlink12);
  vTaskSuspend(xTaskWriteBrightness);
  vTaskSuspend(xTaskReadBrightness);
  isReaderActive = false;
}

//Método que inicia la lectura de la luminosidad
void continueReading () {
  vTaskResume(xTaskBlink11);
  vTaskResume(xTaskWriteBrightness);
  vTaskResume(xTaskReadBrightness);
  vTaskSuspend(xTaskReadBuffer);
  isReaderActive = true;
}

//Metodo que termina o comienza la lectura de intensidad
void interrupt() {
  if (isReaderActive){
    stopReading();
  } else {
    continueReading();
  }
}

void setup() {
  static int id11 = 11, id12 = 12;

  Serial.begin(9600, SERIAL_8N1);

  //Pin para captar el brillo
  pinMode(A3, INPUT);

  //Pines que parpadearan
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  //Pulsador
  pinMode(2, INPUT_PULLUP);

  //Agrega evento de interrupción
  attachInterrupt(digitalPinToInterrupt(2), interrupt, FALLING);

  //Crea la tarea que lee la intensidad del brillo
  xTaskCreate(readBrightness, "lectorIntensidad", 128, NULL, 0, &xTaskReadBrightness);
  //Crea la tarea que espera los mensajes desde la aplicacion para activar lectura
  xTaskCreate(readBuffer, "lectorBuffer", 128, NULL, 0, &xTaskReadBuffer);

  //Crea la tarea que escribe la intensidad del brillo
  xTaskCreate(writeBrightness, "escritorIntensidad", 128, NULL, 1, &xTaskWriteBrightness);

  //Crea la tarea que hace parpadear el led 11
  xTaskCreate(blink, "parpadear11", 128, &id11, 2, &xTaskBlink11);
  //Crea la tarea que hace parpadear el led 12
  xTaskCreate(blink, "parpadear12", 128, &id12, 2, &xTaskBlink12);
}

void loop() {}