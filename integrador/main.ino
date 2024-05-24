#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <EEPROM.h>

//Semaforo que controla las condiciones de carrera
SemaphoreHandle_t xWinnerSemaphore;

//Manejador de las tareas
TaskHandle_t xTaskReadBuffer = NULL, xTaskGame = NULL;

//Array con pines de los leds a encender
const int ledPins[] = {8, 9, 10, 11, 12, 13};

// Ultimo pin encendido y actual
int selectedPin = -1, currentPin = -1, winnerID = -1, lastPosition = 2;
bool bIsGaming = false;


//Método que lee el buffer
void readBuffer () {
  int buffer;

  for ( ;; ){
    //Revisa si hay datos en el buffer
    if(Serial.available() > 0){
      buffer = Serial.readStringUntil('\n').toInt();

      //Si se envia 0, comienza el juego y suspende la lecutura del buffer
      //Si se envia 1, se limpia la EEPROM
      if(buffer == 0){
        //Inicializa o la tarea que ejecuta el juego
        xTaskCreate(playGame, "juego", 128, NULL, 3, &xTaskGame);

        //Suspende la que lee el buffer
        vTaskSuspend(xTaskReadBuffer);
      } else if (buffer == 1){
        clearEEPROM();
      }
    }
  }
  vTaskDelete(NULL);
}
void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  lastPosition = 2;
}


//Método que ejecuta el juego
void playGame () {
  //Inicializa el juego
  initGame();

  bIsGaming = true;

  //Ejecuta el encendido de leds
  for ( ;; ){
    //Mientras no haya ganador
    if (winnerID < 0) {
      //Selecciona un número a encender
      currentPin = getRandomPin();

      //Enciende el pin seleccionado
      showPin(currentPin);
    } else {
      break;
    }
  }

  //Realiza las acciones de mostrar el ganador
  endGame();
}
void initGame () {
  //Apaga los pines ganadores
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);

  //Avisa que comienza el juego
  pinOn();
  pinOff();
  
  //Obtiene un pin aleatorio de manera inicial
  selectedPin = -1;
  selectedPin = getRandomPin();

  //Enciende el pin seleccionado
  showPin(selectedPin);

  //Cuenta el inicio del juego
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  vTaskDelay(70);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  vTaskDelay(70);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  vTaskDelay(70);
  pinOff();
}
void endGame () {
  vTaskDelay(70);

  //Avisa que termina el juego
  pinOff();

  //Si no es el correcto, ha ganado el otro jugador
  if (currentPin != selectedPin){
    if (winnerID == 1){
      winnerID = 2;
    } else {
      winnerID = 1;
    }
  }
  //Enciende el led ganador
  digitalWrite(winnerID + 5, HIGH);
  
  //Guarda los datos en la EEPROM
  EEPROM.put(lastPosition, winnerID);

  //Actualiza la posicion a escribir
  lastPosition += 2;
  EEPROM.put(0, lastPosition);

  //Restablece los datos
  currentPin = -1;
  winnerID = -1;
  bIsGaming = false;

  //Envia los datos al puerto serial
  readEEPROM();

  //Continua la ejecucion de la lectura del puerto serial
  vTaskResume(xTaskReadBuffer);
  vTaskDelete(NULL);
}
void readEEPROM () {
  String result = "";
  int currentPosition = 2, currentWinner;

  //Inicializa el evento ocurrido
  EEPROM.get(currentPosition, currentWinner);

  //Muentras este leyendo datos de usuarios
  while (currentWinner == 1 || currentWinner == 2) {
    //Lo mapea y almacena
    result += String(currentWinner) + ";";

    //Obtiene el siguiente ganador
    currentPosition += 2;
    EEPROM.get(currentPosition, currentWinner);
  }

  //Retorna el historial
  if (result == "") {
    Serial.println("no data");
  } else {
    Serial.println(result);
  }
}


//Métodos que determinan el ganador
void actionPin2 () {
  pressButton(1);
}
void actionPin3 () {
  pressButton(2);
}
void pressButton (int playerID) {
  //Verifica si no existe ganador
  if(bIsGaming && winnerID < 0){
    Serial.println("Presiona: " + String(playerID));
    winnerID = playerID;
    //Enciende los leds
    for (int i = 0; i < 6; i++) {
      digitalWrite(ledPins[i], HIGH);  
    }
  }
}


//Método que inicializa los pins
void initPins () {
  // Configura todos los pines como salidas
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);

    // Asegura que todos los LEDs estén apagados al inicio
    digitalWrite(ledPins[i], LOW);
  }

  //Pines que indican los ganadores
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
}
//Método que selecciona un pin de manera aleatoria
int getRandomPin (){
  //Selecciona un pin
  int newPin = random(8, 13);

  //Obligatoriamente selecciona uno distinto al anterior
  while (newPin == currentPin) {
    newPin = random(8, 13);
  }

  //Si no esta inicializando y no se ha seleccionado el correcto. Reselecciona con probabilidad
  if (selectedPin > 0 && newPin != selectedPin) {
    // 70% de probabilidad de seleccionar el anterior
    if (random(0, 100) < 30){
      newPin = selectedPin;
    }
  }
  Serial.println("Selecciona: " + String(newPin));
  return newPin;
}
// Enciende los leds
void pinOn () {
  for (int i = 0; i < 6; i++) {
    digitalWrite(ledPins[i], HIGH);  
  }
  vTaskDelay(50);
}
//Apaga los leds
void pinOff () {
  for (int i = 0; i < 6; i++) {
    digitalWrite(ledPins[i], LOW);  
  }
  vTaskDelay(50);
}
void showPin (int id) {
  digitalWrite(id, HIGH);
  vTaskDelay(100);
  if (winnerID < 0){
    digitalWrite(id, LOW);
    vTaskDelay(50);
  }
}

void setup() {
  Serial.begin(9600);

  // Inicializa el generador de números aleatorios
  randomSeed(analogRead(0));

  //Inicializa los pines 
  initPins();

  // Crear el semáforo
  xWinnerSemaphore = xSemaphoreCreateBinary();

  // Si la eeprom contiene datos, obtiene la posicion de memoria a escribir
  if (EEPROM.read(0) != 255) {
    EEPROM.get(0, lastPosition);
  }

  //Agrega la lógica de interrupciones
  attachInterrupt(digitalPinToInterrupt(2), actionPin2, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), actionPin3, FALLING);

  //Inicializa la tarea que lee el buffer
  xTaskCreate(readBuffer, "leerBuffer", 128, NULL, 3, &xTaskReadBuffer);
}

void loop() {}