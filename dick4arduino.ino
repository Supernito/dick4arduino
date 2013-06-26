/**
 *	Diseño de Sistemas Operativos
 *    Curso 2012 - 2013
 *
 *	Práctica Arduino-uDick
 */
#include "Arduino.h"
/*-----------------------------------------------------------*/
/*                  RTOS                                     */
/*-----------------------------------------------------------*/
#include "udick.h"
#define timeTick 62500

//TAreas Periodicas
#define T1 20.0
#define WCET1 1.1

int t1;
proc cycle();

/*-----------------------------------------------------------*/
/*  Timer, serial comms. and test PINs initialization        */
/*-----------------------------------------------------------*/
void setup(){
  pinMode(ledPin11, OUTPUT);
  pinMode(ledPin12, OUTPUT);
  pinMode(ledPin13, OUTPUT);
  digitalWrite(ledPin13, HIGH);
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  
  Serial.println("Initializing the system...");
 
  ini_system(timeTick);

  Serial.println("System initialized.");

  delay(300);
  
  Serial.println("Creada la tarea");
  delay(300);
  char ciclo[] = "ciclo       ";
  t1= create(ciclo, cycle, HARD, T1, WCET1);
  Serial.println("Activar la tarea");
  delay(300);
//  Serial.println(pexe);
    activate(t1);
//  delay(1000);
//  Serial.println("cosa");
//  Serial.println("hola");
//  abort();
  digitalWrite(ledPin13, LOW);
}

/*-----------------------------------------------------------*/
/*                             Main                          */
/*-----------------------------------------------------------*/
void loop(){
  
    int val = analogRead(analogPin0);
    float slice = 256.0;
    float level = val/slice;
    Serial.print("El valor de level es:");
    Serial.println(level);
    if (level >= 0 && level<=1){
        digitalWrite(ledPin11, 0);
        digitalWrite(ledPin12, 0);
        digitalWrite(ledPin13, 0);
    //    Serial.println("Estoy entre 0 y 1");
    }else if(level > 1 && level<=2){
        digitalWrite(ledPin11, 1);
        digitalWrite(ledPin12, 0);
        digitalWrite(ledPin13, 0);
    //    Serial.println("Estoy entre 1 y 2");
    }else if(level > 2 && level<=3){
        digitalWrite(ledPin11, 1);
        digitalWrite(ledPin12, 1);
        digitalWrite(ledPin13, 0);
    //    Serial.println("Estoy entre 2 y 3");
    }else if(level > 3){
        digitalWrite(ledPin11, 1);
        digitalWrite(ledPin12, 1);
        digitalWrite(ledPin13, 1);
    //    Serial.println("Estoy en mas de 3");
    }
    //Serial.println("El valor es:");
    //Serial.println(val);
    delay(100);
}

/*-----------------------------------------------------------*/
/*  ---            Example of periodic task      ---         */
/*-----------------------------------------------------------*/
proc cycle()
{
//    while (TRUE) {
      Serial.println("Estamos en la tarea periodica.");
      digitalWrite(ledPin13, 1);
      delay(1000);
      digitalWrite(ledPin13, 0);
        /* < periodic code > */
        end_cycle();
//    }
    return t1;
}

/*-----------------------------------------------------------*/
/*  ---            Example of cab usage          ---         */
/*-----------------------------------------------------------*/
/**
 * Some example messages
 */
const int eMsg_Hello = 1;
const int eMsg_GoodBye = 2;

/**
 * Example of how to insert a message in a CAB. In
 * this case, the message will be an integer
 */
void CAB_insert_message_example()
{

    cab c;
    pointer p;
    char msg[BUFFER_SIZE];

    /* reserve a cab */
    c = reserve();

    /* clear buffer */
    memset(msg, 0, sizeof(char)*BUFFER_SIZE);

    /* get a pointer to the desired data */
    p = (pointer)&eMsg_GoodBye;

    /* copy data into buffer */
    memcpy(msg, p, sizeof(int));

    /* call putmes to put the message into cab */
    putmes(c, msg);
}

/**
 * Example of how to get a message from a CAB. In
 * this case, the message will be an integer
 */
void CAB_get_message_example()
{
    cab c;
    pointer msg;
    int decodedMsg;

    /* get the message and a handle to the cab */
    msg = getmes(&c);

    /* copy the message to the desired memory region */
    memcpy(&decodedMsg, msg, sizeof(int));

    /* decrement the reference counter of this cab */
    unget(c);
}

int read_potentiometer(int pin)
{
  return analogRead(pin); 
}
