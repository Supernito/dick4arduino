/**
 *	uDick Real Time Operating System
 *
 *	Diseño de Sistemas Operativos
 *	Curso 2012 - 2013
 */
#ifndef UDICK_H
#define UDICK_H

/*---------------------------------------------------------------------*/
/*                      Data types                                     */
/*---------------------------------------------------------------------*/
typedef  int    queue;    /* head index 			                   */
typedef  int    sem; 	  /* semaphore index 	                       */
typedef  int    proc; 	  /* process index 		                       */
typedef  int    cab;      /* cab (cyclical asynchronous buffers) index */
typedef  char*  pointer;  /* memory pointer 	                       */

/*---------------------------------------------------------------------*/
/*                      Global constants                               */
/*---------------------------------------------------------------------*/
#define MAXLEN      12           /* max string length		   */
#define MAXPROC     32           /* max number of tasks		   */
#define MAXSEM      32           /* max number of semaphores   */
#define MAXDLINE    0x7FFFFFFF   /* max deadline		       */
#define PRT_LEV     255          /* priority levels		       */
#define NIL         -1           /* null pointer		       */
#define TRUE        1
#define FALSE       0
#define LIFETIME    MAXDLINE - PRT_LEV
#define MAX_STACK_SIZE 512

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
#define NO_CAB         -6   /* too many CABs                 */

/*-----------------------------------------------------------*/
/*                  Test PINs definition                     */
/*-----------------------------------------------------------*/
#define ledPin11 11
#define ledPin12 12
#define ledPin13 13
#define analogPin0 0

/*----------------------------------------------------------------------*/
/*                      Time management                                 */
/*----------------------------------------------------------------------*/
extern unsigned long	sys_clock;          /* system time of ticks after reset	*/
extern float		    time_unit;          /* unit of time (ms) (time / tick)	*/
extern unsigned long    system_time;        /* system time in secs. after reset	*/
extern unsigned long    last_sys_clock;
extern unsigned long    interval;
extern float            tick;               /* tick */

/*---------------------------------------------------------*/
/*          Task Control Block structure definition        */
/*---------------------------------------------------------*/
typedef int* Context;

typedef struct tcb {
    char   name[MAXLEN+1];   /* task identifier name        */
    proc   (*addr)();        /* task address                */
    int    type;             /* task type (periodic, etc.)  */
    int    state;            /* task state                  */
    unsigned long   dline;            /* absolute deadline           */
    int    period;           /* period / relative deadline, */
    /* or priority of NRT          */
    int    prt;              /* task priority               */
    int    wcet;             /* wost-case execution time    */
    float  util;             /* task utilization factor     */
    Context context;         /* pointer to the context      */
    proc   next;             /* pointer to the next tcb     */
    proc   prev;             /* pointer to previous tcb     */
} tcb_t;

/*---------------------------------------------------------*/
/*          Context data structure definition        	   */
/*---------------------------------------------------------*/
typedef struct context_data {
    unsigned char data[MAX_STACK_SIZE];
} context_data_t;

/*-----------------------------------------------------------*/
/*       Semaphore Control Block structure definition        */
/*-----------------------------------------------------------*/
typedef struct scb {
    int    count;            /* semaphore counter             */
    queue  qsem;             /* semaphore queue               */
    sem    next;             /* pointer to the next semaphore */
} scb_t;

/*-----------------------------------------------------------*/
/*   Cyclic Asynchronous Buffer (CAB) structure definition   */
/*-----------------------------------------------------------*/
#define BUFFER_SIZE 32
#define BUFFER_MAX 16

typedef char buffer;

typedef struct mrb {
    cab next;                   /* An index to the next MRB in the list */
    cab use;                    /* A counter that stores the current number of tasks */
                                /* accessing that buffer */
    buffer data[BUFFER_SIZE];   /* Memory area for storing the message */
} mrb_t;

typedef struct cab_cb {
    cab free;                   /* An index to maintain a list of free buffers */
    cab mrb;                    /* An index to the most recent buffer */
    int max_buf;                /* The maximum number of buffers */
    long dim_buf;               /* The dimension of the buffers */
} cabcb_t;

typedef struct cab_t {
    cabcb_t cab_cbs;
    mrb_t vmrbs[BUFFER_MAX];
} cab_t;


/*===========================================================*/
/*                     INITIALIZATION                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* ini_system ---  system initialization	                 */
/*-----------------------------------------------------------*/
void ini_system(float tick);

/*===========================================================*/
/*                TASK' STATUS INQUIRY                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* get_time ---  returns the system time in milliseconds     */
/*-----------------------------------------------------------*/
float get_time(void);

/*===========================================================*/
/*                  LOW-LEVEL PRIMITIVES                     */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* save_context ---  of the task in execution                */
/*-----------------------------------------------------------*/
void save_context(void);

/*-----------------------------------------------------------*/
/* load_context ---  of the task to be executed              */
/*-----------------------------------------------------------*/
void load_context(void);

/*===========================================================*/
/*                    LIST MANAGEMENT                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* insert   ---  a task in a queue based on its deadline     */
/*-----------------------------------------------------------*/
void insert(proc i, queue *que);


/*-----------------------------------------------------------*/
/* extract   ---  a task from a queue                        */
/*-----------------------------------------------------------*/
proc extract(proc i, queue *que); /* index ("TCBs num. ") and queue desired task */


/*-----------------------------------------------------------*/
/* getfirst   ---  extracts a task at the head of a queue    */
/*-----------------------------------------------------------*/
proc getfirst(queue *que);

/*-----------------------------------------------------------*/
/* firstdline  ---  returns the deadline of the first task   */
/*-----------------------------------------------------------*/
unsigned long firstdline(queue *que);

/*-----------------------------------------------------------*/
/* empty    ---  returns TRUE if a queue is empty            */
/*-----------------------------------------------------------*/
int empty(queue *que);

/*===========================================================*/
/*                SCHEDULING MECHANISM                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* schedule --- selects the task with the earliest deadline  */
/*-----------------------------------------------------------*/
void schedule(void);

/*---------------------------------------------------------------*/
/* dispatch --- assigns the cpu to the first ready task          */
/*---------------------------------------------------------------*/
void dispatch(void);

/*-----------------------------------------------------------*/
/* wake_up --- final timer interrupt handling routine        */
/*-----------------------------------------------------------*/
void wake_up(void); /* timer interrupt handling routine  */

/*===========================================================*/
/*                    TASK' MANAGEMENT                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* create  --- creates a task and puts it in sleep state     */
/*-----------------------------------------------------------*/
proc create(
        char		name[MAXLEN+1],         /* task name */
        proc		(*addr)(),           /* task address */
        int		    type,            /* type (HARD, NRT) */
        float		period,/* period, relative dl or pri */
        float       wcet             /* execution time */
        );

/*-----------------------------------------------------------*/
/* guarantee  --- guarantees the feasibility of a hard task  */
/*-----------------------------------------------------------*/
int guarantee(proc p);

/*-----------------------------------------------------------*/
/* activate  --- inserts a task in the ready queue           */
/*-----------------------------------------------------------*/
void activate(proc p);

/*-----------------------------------------------------------*/
/* sleep  --- suspends itself in a sleep state               */
/*-----------------------------------------------------------*/
void sleep(void);

/*-----------------------------------------------------------*/
/* end_cycle  --- inserts a task in the idle queue           */
/*-----------------------------------------------------------*/
void end_cycle(void);

/*-----------------------------------------------------------*/
/*  ---            Example of periodic task      ---         */
/*-----------------------------------------------------------*/
//proc cycle(void);

/*-----------------------------------------------------------*/
/* end_process  ---  inserts a task in the idle queue        */
/*-----------------------------------------------------------*/
void end_process(void);

/*-----------------------------------------------------------*/
/* kill  ---  terminates a task                              */
/*-----------------------------------------------------------*/
void kill(proc p);

/*===========================================================*/
/*                        SEMAPHORES                         */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* newsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
sem newsem(int n);
/*-----------------------------------------------------------*/
/* delsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
void delsem(sem s);
/*-----------------------------------------------------------*/
/* wait  ---  wait for an event                              */
/*-----------------------------------------------------------*/
void wait(sem s);
/*-----------------------------------------------------------*/
/* signal ---  signals an event                              */
/*-----------------------------------------------------------*/
void signal(sem s);

/*===========================================================*/
/*                TASK' STATUS INQUIRY                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* get_state ---  returns the state of a task                */
/*-----------------------------------------------------------*/
int get_state(proc p);

/*-----------------------------------------------------------*/
/* get_dline ---  returns the deadline of a task             */
/*-----------------------------------------------------------*/
long get_dline(proc p);

/*-----------------------------------------------------------*/
/* get_period ---  returns the period of a task              */
/*-----------------------------------------------------------*/
float get_period(proc p);

/*===========================================================*/
/*                          CAB                              */
/*===========================================================*/
/*                                                           */
/*             Usage:                                        */
/*-----------------------------------------------------------*/
/*             to insert a message in a CAB                  */
/*-----------------------------------------------------------*/
//      buf_pointer = reserve (cab_id);
//       < copy message in *buf_pointer >
//       putmes (buf_pointer, cab_id);
//
/*-----------------------------------------------------------*/
/*             to get a message from a CAB                   */
/*-----------------------------------------------------------*/
//       mes_pointer = getmes (cab_id);
//       < use message  >
//       unget (mes_pointer, cab_id);

/*-----------------------------------------------------------*/
/* reserve  ---  reserves a buffer in a CAB                  */
/*-----------------------------------------------------------*/
cab reserve();

/*-----------------------------------------------------------*/
/* putmes  ---  puts a message in a CAB                      */
/*-----------------------------------------------------------*/
void putmes(cab c, pointer msg);

/*-----------------------------------------------------------*/
/* getmes  ---  gets a pointer to the most recent buffer     */
/*-----------------------------------------------------------*/
pointer getmes(cab* c);

/*-----------------------------------------------------------*/
/* unget --- deallocates a buffer only if it is not accessed */
/*           and it is not the most recent buffer            */
/*-----------------------------------------------------------*/
void unget(cab c);

#endif // UDICK_H
