/*
 * Prueba para extraer el RTOS
 * Miguel Ángel Ortiz
 * 25-04-2013
 */
/*-----------------------------------------------------------*/
/*                  RTOS                                     */
/*-----------------------------------------------------------*/
#include "udick.h"
float           tick=62.0/62500.0;  /* tick

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

/*===========================================================*/
/*                     INITIALIZATION                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* ini_system ---  system initialization	             */
/*-----------------------------------------------------------*/

void ini_system ( float tick )
{
      Serial.println("executing 'ini_system'..." );
      delay(1000);
     
      proc i;
      Serial.print("Time unit=" );
      Serial.println(tick, 6);   
      delay(500);
 
      pinMode(ledPin12, OUTPUT);
      pinMode(ledPin13, OUTPUT);
            
      Serial.println("Initializing list of free TCBs..." );
      delay(500);
 
      // < initialize the interrupt vector table >
      /* initialize the list of free TCBs and semaphores */
      for ( i=0; i < MAXPROC-1; i++ ) vdes[i].next = i + 1;
      vdes[MAXPROC-1].next = NIL;
      
      Serial.println("Initializing list of free semaphores..." );
      delay(500);
      /* initialize the list of free semaphores */    
      for ( i=0; i < MAXSEM-1; i++ )  vsem[i].next = i + 1;
      vsem[MAXSEM-1].next = NIL;
      ready = idle = zombie = NIL;
      freetcb = freesem = 0;
      
      util_fact = 0;    

      // < initialize the TCB of the main process >
      // pexe = <main index>;
};

/*===========================================================*/
/*                TASK' STATUS INQUIRY                       */
/*===========================================================*/


/*-----------------------------------------------------------*/
/* get_time ---  returns the system time in milliseconds     */
/*-----------------------------------------------------------*/
float get_time ( void )
{
    return tick * sys_clock;
}

/*-----------------------------------------------------------*/
/*                             Main                          */
/*-----------------------------------------------------------*/

void loop()
{
  // our program here...
  digitalWrite(ledPin13, 1);   // toggle LED pin 13
  system_time=get_time();
  Serial.print("System time = ");                       
  Serial.print(system_time); 
  //Serial.print("\t Time unit = ");
  Serial.print("   sys_clock = ");
  Serial.println(sys_clock,6);
  
  //interval=sys_clock-last_sys_clock;
  //last_sys_clock=sys_clock;
  //Serial.print("\t Interval = ");
  //Serial.print(interval,6);
  //Serial.print("\t ");
  //Serial.println(interval*tick,6);
  
  delay(500);
  digitalWrite(ledPin13, 0);
  delay(500);
}
