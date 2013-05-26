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

/*-----------------------------------------------------------*/
/*  Timer, serial comms. and test PINs initialization        */
/*-----------------------------------------------------------*/
void setup() {
    // initialize serial communications at 9600 bps:
    Serial.begin(9600);

    Serial.println("System's setup..." );
    delay(1000);

    // < enable the timer to interrupt every time_unit >
    Serial.println("Setting up timer..." );
    delay(1000);

    // initialize timer1
    noInterrupts();          // disable all interrupts
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1  = 0;

        OCR1A = 62;              // compare match register 16MHz (16E6/256=62500)  62500/62= 1000 Hz, Tick=1ms
        TCCR1B |= (1 << WGM12);  // CTC mode
        TCCR1B |= (1 << CS12);   // 256 prescaler
        TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
    interrupts();            // enable all interrupts

    ini_system(tick);
}

/*-----------------------------------------------------------*/
/*                             Main                          */
/*-----------------------------------------------------------*/
void loop()
{
    // our program here...
    digitalWrite(ledPin13, 1);   // toggle LED pin 13

    system_time = get_time();
    Serial.print("System time = ");
    Serial.print(system_time);

    Serial.print("\t Time unit = ");
    Serial.print("   sys_clock = ");
    Serial.println(sys_clock, 6);

    interval = sys_clock - last_sys_clock;
    last_sys_clock = sys_clock;
    Serial.print("\t Interval = ");
    Serial.print(interval, 6);
    Serial.print("\t ");
    Serial.println(interval * tick, 6);
    delay(500);

    digitalWrite(ledPin13, 0);
    delay(500);
}

/*-----------------------------------------------------------*/
/*  ---            Example of periodic task      ---         */
/*-----------------------------------------------------------*/
proc cycle()
{
    while (TRUE) {
        /* < periodic code > */
        end_cycle();
    }
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
