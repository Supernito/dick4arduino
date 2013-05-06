/*
	Test 1
 	uDick.h
 	Miguel Ángel Ortiz
 	25-04-2013
 */
/*---------------------------------------------------------------------*/
/*  Para usar los tipos estándar y las constantes del lenguaje Arduino */
/*---------------------------------------------------------------------*/
#include "Arduino.h"

/*-----------------------------------------------------------*/
/*                  Test PINs definition                     */
/*-----------------------------------------------------------*/
#define ledPin12 12
#define ledPin13 13

/*---------------------------------------------------------------------*/
/*                      Data types                                     */
/*---------------------------------------------------------------------*/
typedef  int    queue;    /* head index 			       */
typedef  int    sem; 	  /* semaphore index 	                       */
typedef  int    proc; 	  /* process index 		               */
typedef  int    cab;      /* cab (cyclical asynchronous buffers) index */
typedef  char*  pointer;  /* memory pointer 	                       */

/*---------------------------------------------------------------------*/
/*                      Global constants                               */
/*---------------------------------------------------------------------*/
#define MAXLEN      12           /* max string length		       */
#define MAXPROC     32           /* max number of tasks		       */
#define MAXSEM      32           /* max number of semaphores	       */
#define MAXDLINE    0x7FFFFFFF   /* max deadline		       */
#define PRT_LEV     255          /* priority levels		       */
#define NIL         -1           /* null pointer		       */
#define TRUE        1
#define FALSE       0
#define LIFETIME    MAXDLINE - PRT_LEV

/*-----------------------------------------------------------*/
/*                      Task types                           */
/*-----------------------------------------------------------*/
#define HARD      1      /* critical task                    */
#define NRT       2      /* non real-time task               */
/*-----------------------------------------------------------*/
/*                      Task states                          */
/*-----------------------------------------------------------*/
#define FREE      0      /* TCB not allocated                */
#define READY     1      /* ready state                      */
#define EXE       2      /* running state                    */
#define SLEEP     3      /* sleep state                      */
#define IDLE      4      /* idle state                       */
#define WAIT      5      /* wait state                       */
#define ZOMBIE    6      /* zombie state                     */
/*-----------------------------------------------------------*/
/*                      Error messages                       */
/*-----------------------------------------------------------*/
#define OK              0   /* no error                      */
#define TIME_OVERFLOW  -1   /* missed deadline               */
#define TIME_EXPIRED   -2   /* lifetime reached              */
#define NO_GUARANTEE   -3   /* task not schedulable          */
#define NO_TCB         -4   /* too may tasks                 */
#define NO_SEM         -5   /* too many semaphores           */

/*-----------------------------------------------------------*/
/*                      Actual values                        */
/*-----------------------------------------------------------*/
proc 	pexe;           /* task in execution	             */
queue	ready;          /* ready queue 		             */
queue	idle;           /* idle queue		             */
queue	zombie;         /* zombie queue		             */
queue	freetcb;        /* queue of free tcbs	             */
queue 	freesem;        /* queue of free semaphores          */
float 	util_fact;      /* utilization factor	             */

/*----------------------------------------------------------------------*/
/*                      Time management                                 */
/*----------------------------------------------------------------------*/
unsigned long	sys_clock;          /* system time of ticks after reset	*/
float		time_unit;          /* unit of time (ms) (time / tick)	*/                 
unsigned long   system_time;        /* system time in secs. after reset	*/
unsigned long   last_sys_clock;
unsigned long   interval;

/*---------------------------------------------------------*/
/*          Task Control Block structure definition        */
/*---------------------------------------------------------*/
struct tcb {
  char   name[MAXLEN+1];   /* task identifier name        */
  proc   (*addr)();        /* task address                */
  int    type;             /* task type (periodic, etc.)  */
  int    state;            /* task state                  */
  long   dline;            /* absolute deadline           */
  int    period;           /* period / relative deadline, */
  /* or priority of NRT          */
  int    prt;              /* task priority               */
  int    wcet;             /* wost-case execution time    */
  float  util;             /* task utilization factor     */
  int    *context;         /* pointer to the context      */
  proc   next;             /* pointer to the next tcb     */
  proc   prev;             /* pointer to previous tcb     */
};

/*-----------------------------------------------------------*/
/*       Semaphore Control Block structure definition        */
/*-----------------------------------------------------------*/
struct scb {
  int    count;            /* semaphore counter             */
  queue  qsem;             /* semaphore queue               */
  sem    next;             /* pointer to the next semaphore */
};

/*-----------------------------------------------------------*/
/*                  Array of TCB and array of SCB            */
/*-----------------------------------------------------------*/
struct  tcb     vdes[MAXPROC];  /* array of tcb's	     */
struct  scb     vsem[MAXSEM];   /* array of scb's	     */

/*----------------------------------------------------------------------*/
/*                Basic Timer Interrupt Service Routine                 */
/*----------------------------------------------------------------------*/
ISR(TIMER1_COMPA_vect);          // timer compare interrupt service routine

/*===========================================================*/
/*                     INITIALIZATION                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* ini_system ---  system initialization	             */
/*-----------------------------------------------------------*/

void ini_system ( float tick );

/*===========================================================*/
/*                  LOW-LEVEL PRIMITIVES                     */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* save_context ---  of the task in execution                */
/*-----------------------------------------------------------*/
void     save_context ( void );

/*-----------------------------------------------------------*/
/* load_context ---  of the task to be executed              */
/*-----------------------------------------------------------*/
void load_context ( void );

/*===========================================================*/
/*                    LIST MANAGEMENT                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* insert   ---  a task in a queue based on its deadline     */
/*-----------------------------------------------------------*/
void insert ( proc i, queue *que );


/*-----------------------------------------------------------*/
/* extract   ---  a task from a queue                        */
/*-----------------------------------------------------------*/
proc extract ( proc i, queue *que ); /* index ("TCBs num. ") and queue desired task */


/*-----------------------------------------------------------*/
/* getfirst   ---  extracts a task at the head of a queue    */
/*-----------------------------------------------------------*/
proc getfirst ( queue *que );

/*-----------------------------------------------------------*/
/* firstdline  ---  returns the deadline of the first task   */
/*-----------------------------------------------------------*/
long firstdline ( queue *que );

/*-----------------------------------------------------------*/
/* empty    ---  returns TRUE if a queue is empty            */
/*-----------------------------------------------------------*/
int empty ( queue *que );

/*===========================================================*/
/*                SCHEDULING MECHANISM                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* schedule --- selects the task with the earliest deadline  */
/*-----------------------------------------------------------*/

void    schedule ( void );

/*---------------------------------------------------------------*/
/* dispatch --- assigns the cpu to the first ready task          */
/*---------------------------------------------------------------*/
void    dispatch ( void );

/*-----------------------------------------------------------*/
/* wake_up --- final timer interrupt handling routine        */
/*-----------------------------------------------------------*/

void   wake_up ( void ); /* timer interrupt handling routine  */

/*===========================================================*/
/*                    TASK' MANAGEMENT                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* create  --- creates a task and puts it in sleep state     */
/*-----------------------------------------------------------*/
proc create (
char		name[MAXLEN+1],         /* task name */
proc		(*addr)(),           /* task address */
int		type,            /* type (HARD, NRT) */
float		period,/* period, relative dl or pri */
float wcet );                       /* execution time */

/*-----------------------------------------------------------*/
/* guarantee  --- guarantees the feasibility of a hard task  */
/*-----------------------------------------------------------*/
int guarantee ( proc p );

/*-----------------------------------------------------------*/
/* activate  --- inserts a task in the ready queue           */
/*-----------------------------------------------------------*/
int activate ( proc p );

/*-----------------------------------------------------------*/
/* sleep  --- suspends itself in a sleep state               */
/*-----------------------------------------------------------*/
int     sleep ( void );

/*-----------------------------------------------------------*/
/* end_cycle  --- inserts a task in the idle queue           */
/*-----------------------------------------------------------*/
int      end_cycle ( void );

/*-----------------------------------------------------------*/
/*  ---            Example of periodic task      ---         */
/*-----------------------------------------------------------*/
proc cycle();
/*-----------------------------------------------------------*/
/* end_process  ---  inserts a task in the idle queue        */
/*-----------------------------------------------------------*/
int end_process ( void );
/*-----------------------------------------------------------*/
/* kill  ---  terminates a task                              */
/*-----------------------------------------------------------*/
void kill ( proc p );
/*===========================================================*/
/*                        SEMAPHORES                         */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* newsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
sem      newsem (int n );
/*-----------------------------------------------------------*/
/* delsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
void      delsem ( sem s );
/*-----------------------------------------------------------*/
/* wait  ---  wait for an event                              */
/*-----------------------------------------------------------*/
void      wait ( sem s );
/*-----------------------------------------------------------*/
/* signal ---  signals an event                              */
/*-----------------------------------------------------------*/
void     signal ( sem s );
/*===========================================================*/
/*                TASK' STATUS INQUIRY                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* get_state ---  returns the state of a task                */
/*-----------------------------------------------------------*/
int      get_state ( proc p );
/*-----------------------------------------------------------*/
/* get_dline ---  returns the deadline of a task             */
/*-----------------------------------------------------------*/
long     get_dline ( proc p );
/*-----------------------------------------------------------*/
/* get_period ---  returns the period of a task              */
/*-----------------------------------------------------------*/
float      get_period ( proc p );
/*===========================================================*/
/*                  CAB IMPLEMENTATION                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/*             to insert a message in a CAB                  */
/*-----------------------------------------------------------*/
//buf_pointer = reserve (cab_id);
//< copy message in *buf_pointer >
//putmes (buf_pointer, cab_id);

/*-----------------------------------------------------------*/
/*             to get a message from a CAB                   */
/*-----------------------------------------------------------*/
//mes_pointer = getmes (cab_id);
//< use message  >
//unget (mes_pointer, cab_id);

/*-----------------------------------------------------------*/
/* reserve  ---  reserves a buffer in a CAB                  */
/*-----------------------------------------------------------*/
pointer     reserve (cab c);
/*-----------------------------------------------------------*/
/* putmes  ---  puts a message in a CAB                      */
/*-----------------------------------------------------------*/
void     putmes(cab c, pointer p);

/*-----------------------------------------------------------*/
/* getmes  ---  gets a pointer to the most recent buffer     */
/*-----------------------------------------------------------*/
pointer     getmes(cab c);

/*-----------------------------------------------------------*/
/* unget --- deallocates a buffer only if it is not accessed */
/*           and it is not the most recent buffer            */
/*-----------------------------------------------------------*/
void     unget(cab c, pointer p);

