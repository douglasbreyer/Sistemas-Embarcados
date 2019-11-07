// Douglas Breyer - 1511100048

#include <Arduino_FreeRTOS.h>
#include <queue.h>

#define NUM_OF_PHILOSOPHERS 5
#define LEFT (id+NUM_OF_PHILOSOPHERS-1)%NUM_OF_PHILOSOPHERS //filosofo da esquerda
#define RIGHT (id+1)%NUM_OF_PHILOSOPHERS  //filosofo da direita
#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define MIN_DELAY 1
#define MAX_DELAY 5

#define FALSE 0
#define TRUE 1

QueueHandle_t garfos;

void vPrintString( const char *pcString )
{
  vTaskSuspendAll();
  {
      Serial.print(pcString);
      Serial.flush();
  }
  xTaskResumeAll();

  if( Serial.available() )
  {
    vTaskEndScheduler();
  }
}
/*==========================================================================================================*/

void printStringNumberState( const char *pcString, unsigned portLONG ulValue, const char *pcString1, unsigned portLONG ulValue1, const char *pcString2)
{

  vTaskSuspendAll();
  {
      Serial.print(pcString);
      Serial.write(' ');
      Serial.print(ulValue);
      Serial.write(' ');
      Serial.print(pcString1);
      Serial.write(' ');
      Serial.print(ulValue1);
      Serial.write(' ');
      Serial.println(pcString2);
      Serial.flush();
  }
  xTaskResumeAll();

  if( Serial.available() )
  {
    vTaskEndScheduler();
  }
}

/*==========================================================================================================*/

void printStringNumber( const char *pcString, unsigned portLONG ulValue, const char *pcString1)
{

  vTaskSuspendAll();
  {
      Serial.print(pcString);
      Serial.write(' ');
      Serial.print(ulValue);
      Serial.write(' ');
      Serial.println(pcString1);
      Serial.flush();
  }
  xTaskResumeAll();

  if( Serial.available() )
  {
    vTaskEndScheduler();
  }
}


void setup() {
  Serial.begin(9600);
  vPrintString("Começando...\n");

  garfos = xQueueCreate(NUM_OF_PHILOSOPHERS, sizeof(int)); //fila de garfos

  xTaskCreate(philosopher, "Task 0", 100, 0, 1, NULL);     //tarefas
  xTaskCreate(philosopher, "Task 1", 100, 1, 1, NULL);
  xTaskCreate(philosopher, "Task 2", 100, 2, 1, NULL);
  xTaskCreate(philosopher, "Task 3", 100, 3, 1, NULL);
  xTaskCreate(philosopher, "Task 4", 100, 4, 1, NULL);

  for(int i = 0; i < NUM_OF_PHILOSOPHERS; i++){
    xQueueSendToBack(garfos, &i, 0);                    //adiciona os garfos no final fila
    vPrintString("Testando...\n");
  }

  vTaskStartScheduler();

  vPrintString("Alguma coisa deu ruim :(");
  for( ;; );
}

void loop() {}

void philosopher(void *pvParameters){
  while(TRUE){
    int sleepTime = random(MIN_DELAY, MAX_DELAY);

    think(pvParameters);
    printStringNumber("Filosofo", pvParameters, "FAMINTO");
    while (pegaGarfos(pvParameters) == FALSE) {
      delay(sleepTime*1000);
    }
    comer(pvParameters);
    devolveGarfos(pvParameters);

    delay(sleepTime*1000);
  }
}

void think(int id){
  int sleepTime = random(MIN_DELAY, MAX_DELAY);

  printStringNumberState("Filosofo", id, "PENSANDO por", sleepTime, "segundos");

  delay(sleepTime*1000);
}


int pegaGarfos(int id){

  BaseType_t xStatus;
  int forkReceivedId, forkA = -999;

  for(int i = 0; i < NUM_OF_PHILOSOPHERS; i++){
    xStatus = xQueueReceive(garfos, &forkReceivedId, 0); //tenta pegar um garfo

    if(xStatus == pdPASS){         //se conseguiu pegar um garfo, qual garfo foi
      if(forkReceivedId == RIGHT || forkReceivedId == LEFT){
        forkA = forkReceivedId;

        for(int j = 0; j < NUM_OF_PHILOSOPHERS; j++){
          xStatus = xQueueReceive(garfos, &forkReceivedId, 0);

          if(xStatus == pdPASS){
            if(forkReceivedId == RIGHT || forkReceivedId == LEFT){
              return TRUE;
            }else{
              xQueueSendToBack(garfos, &forkReceivedId, 0);
            }
          }else{
            xQueueSendToBack(garfos, &forkA, 0);
            return FALSE;
          }
        }
      }else{
        xQueueSendToBack(garfos, &forkReceivedId, 0);
      }
    }else{

      return FALSE;
    }
  }

  if(forkA != -999){
    xQueueSendToBack(garfos, &forkA, 0);
  }

  return FALSE;
}

void comer(int id){
  int sleepTime = random(MIN_DELAY, MAX_DELAY);

  printStringNumberState("Filosofo", id, "COMENDO por ", sleepTime, "segundos");

  delay(sleepTime*1000);
}

void devolveGarfos(int id){
  int forkId = LEFT;
  if(xQueueSendToBack(garfos, &forkId, 0) == pdPASS){ //Conseguiu jogar de volta o garfo A

  }

  forkId = RIGHT;
  if(xQueueSendToBack(garfos, &forkId, 0) == pdPASS){  //Conseguiu jogar de volta o garfo B

  }

  Serial.print("Filosofo ");
  Serial.print(id);
  Serial.print(" devolveu os garfos.\n");
}
