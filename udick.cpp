/**
 *	uDick Real Time Operating System
 *
 *	Diseño de Sistemas Operativos
 *	Curso 2012 - 2013
 *
 *	uDick.cpp
 */
#include "Arduino.h"
#include "udick.h"


/* List of supported platforms */
#define P_ARDUINO 1
#define P_SIMULATOR 2

/* Current platform to build for */
#define PLATFORM P_ARDUINO

/* Pointer to the context of pexe to use in ASM code. */
Context pxCurrentContext = NULL;
/*-----------------------------------------------------------*/
/*                     Assembly Code                         */
/*-----------------------------------------------------------*/
#if PLATFORM == P_ARDUINO
#   define SAVE_CONTEXT()									\
	asm volatile (	/* Save r0, and then use it to store SREG value */ \
                    /* we store it now to have the value SREG had before interrupts were removed */ \
                    "push	r0						\n\t"	\
					"in		r0, __SREG__			\n\t"	\
                    /* Clear interrupt bit */               \
					"cli							\n\t"	\
                    /* Save SREG into stack */              \
					"push	r0						\n\t"	\
                    /* Save the general purpose registers r1-r31 (r0 already saved) */ \
					"push	r1						\n\t"	\
                    /* Clear r1 because AVR GCC compiler expects r1 to be always 0 (it is the __zero_reg__) */ \
					"clr	r1						\n\t"	\
					"push	r2						\n\t"	\
					"push	r3						\n\t"	\
					"push	r4						\n\t"	\
					"push	r5						\n\t"	\
					"push	r6						\n\t"	\
					"push	r7						\n\t"	\
					"push	r8						\n\t"	\
					"push	r9						\n\t"	\
					"push	r10						\n\t"	\
					"push	r11						\n\t"	\
					"push	r12						\n\t"	\
					"push	r13						\n\t"	\
					"push	r14						\n\t"	\
					"push	r15						\n\t"	\
					"push	r16						\n\t"	\
					"push	r17						\n\t"	\
					"push	r18						\n\t"	\
					"push	r19						\n\t"	\
					"push	r20						\n\t"	\
					"push	r21						\n\t"	\
					"push	r22						\n\t"	\
					"push	r23						\n\t"	\
					"push	r24						\n\t"	\
					"push	r25						\n\t"	\
					"push	r26						\n\t"	\
					"push	r27						\n\t"	\
					"push	r28						\n\t"	\
					"push	r29						\n\t"	\
					"push	r30						\n\t"	\
					"push	r31						\n\t"	\
					/* Load context pointer from dataspace to register x */ \
					/* x register is r27:r26 */             \
					"lds	r26, pxCurrentContext		\n\t"	\
					"lds	r27, pxCurrentContext + 1	\n\t"	\
					/* Now save Stack pointer SPL (IO Register 0x3d) in x, and increment x */ \
					"in		r0, __SP_L__			\n\t"	\
					"st		x+, r0					\n\t"	\
					/* Now save Stack pointer SPH (IO Register 0x3e) in x, and increment x */ \
					"in		r0, __SP_H__			\n\t"	\
					"st		x+, r0					\n\t"	\
					/* This las operation has the effect of setting pxCurrentContext to the current SP */ \
				);

#   define LOAD_CONTEXT()								\
	asm volatile (  /* Load context pointer from dataspace to register x */ \
					/* x register is r27:r26 */             \
                    "lds	r26, pxCurrentContext		\n\t"	\
					"lds	r27, pxCurrentContext + 1	\n\t"	\
					/* At this point, interrupts are disabled, so we can write to the Stack Pointer */ \
					/* Load the new context's stack pointer and write it to the SP register */         \
					"ld		r28, x+					\n\t"	\
                    "out	__SP_L__, r28			\n\t"	\
					"ld		r29, x+					\n\t"	\
					"out	__SP_H__, r29			\n\t"	\
					/* Restore the general purpose registers r31-r1 */ \
					"pop	r31						\n\t"	\
					"pop	r30						\n\t"	\
					"pop	r29						\n\t"	\
					"pop	r28						\n\t"	\
					"pop	r27						\n\t"	\
					"pop	r26						\n\t"	\
					"pop	r25						\n\t"	\
					"pop	r24						\n\t"	\
					"pop	r23						\n\t"	\
					"pop	r22						\n\t"	\
					"pop	r21						\n\t"	\
					"pop	r20						\n\t"	\
					"pop	r19						\n\t"	\
					"pop	r18						\n\t"	\
					"pop	r17						\n\t"	\
					"pop	r16						\n\t"	\
					"pop	r15						\n\t"	\
					"pop	r14						\n\t"	\
					"pop	r13						\n\t"	\
					"pop	r12						\n\t"	\
					"pop	r11						\n\t"	\
					"pop	r10						\n\t"	\
					"pop	r9						\n\t"	\
					"pop	r8						\n\t"	\
					"pop	r7						\n\t"	\
					"pop	r6						\n\t"	\
					"pop	r5						\n\t"	\
					"pop	r4						\n\t"	\
					"pop	r3						\n\t"	\
					"pop	r2						\n\t"	\
					"pop	r1						\n\t"	\
					/* restore register SREG */ \
					"pop	r0						\n\t"	\
					"out	__SREG__, r0			\n\t"	\
					/* restore register r0 */               \
					"pop	r0						\n\t"	\
				);

#else
#   define SAVE_CONTEXT()
#   define LOAD_CONTEXT()
#endif

/*-----------------------------------------------------------*/
/*                      Utils                                */
/*-----------------------------------------------------------*/
const long MAX_LONG = 2147483647;

/*-----------------------------------------------------------*/
/*                      Actual values                        */
/*-----------------------------------------------------------*/
proc 	pexe;           /* task in execution	     */
queue   ready;          /* ready queue 		         */
queue	idle;           /* idle queue		         */
queue	zombie;         /* zombie queue		         */
queue	freetcb;        /* queue of free tcbs	     */
queue 	freesem;        /* queue of free semaphores  */
float 	util_fact;      /* utilization factor	     */

/*----------------------------------------------------------------------*/
/*                      Time management                                 */
/*----------------------------------------------------------------------*/
unsigned long   sys_clock;          /* system time of ticks after reset	*/
float		    time_unit;          /* unit of time (ms) (time / tick)	*/
unsigned long   system_time;        /* system time in secs. after reset	*/
unsigned long   last_sys_clock;
unsigned long   interval;
float           tick = 62.0/62500.0;  /* tick */

/*-----------------------------------------------------------*/
/*                  Array of TCB and array of SCB            */
/*-----------------------------------------------------------*/
tcb_t           vdes[MAXPROC];  /* array of tcb's	     */
scb_t           vsem[MAXSEM];   /* array of scb's	     */
cab_t           cabs;          /* CAB structure */

#define cabcb cabs.cab_cbs
#define mrbs cabs.vmrbs

/*===========================================================*/
/*                     INITIALIZATION                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* init_TBCs ---  initialize the list of free TCBs	         */
/*-----------------------------------------------------------*/
inline void init_TBCs()
{
    proc i;
    for (i = 0; i < MAXPROC - 1; i++) {
        vdes[i].next = i + 1;
    }
    vdes[MAXPROC-1].next = NIL;
}

/*-----------------------------------------------------------*/
/* init_SCBs ---  initialize the list of free semaphores */
/*-----------------------------------------------------------*/
inline void init_SCBs()
{
    sem i;
    for (i = 0; i < MAXSEM - 1; i++) {
        vsem[i].next = i + 1;
    }
    vsem[MAXSEM-1].next = NIL;
    ready = idle = zombie = NIL;
    freetcb = freesem = 0;
}

/*-----------------------------------------------------------*/
/* init_CABs ---  initialize the CABs	                     */
/*-----------------------------------------------------------*/
inline void init_CABs()
{
    cab i;

    cabcb.free = 0;
    cabcb.mrb = NIL;
    cabcb.max_buf = BUFFER_MAX;
    cabcb.dim_buf = BUFFER_SIZE;

    for (i = 0; i < cabcb.max_buf; i++) {
        mrbs[i].next = i + 1;
        mrbs[i].use = 0;
    }
    mrbs[cabcb.max_buf-1].next = NIL;
}

/*-----------------------------------------------------------*/
/* ini_system ---  system initialization	             */
/*-----------------------------------------------------------*/
void ini_system(float tick)
{

    Serial.println("executing 'ini_system'..." );
    delay(1000);

    Serial.print("Time unit=" );
    Serial.println(tick, 6);
    delay(500);

    pinMode(ledPin12, OUTPUT);
    pinMode(ledPin13, OUTPUT);

    Serial.println("Initializing list of free TCBs..." );
    delay(500);

    // < initialize the interrupt vector table >

    /* initialize the list of free TCBs */
    init_TBCs();

    Serial.println("Initializing list of free semaphores..." );
    delay(500);

    /* initialize the list of free semaphores */
    init_SCBs();

    Serial.println("Initializing CABs..." );
    delay(500);

    /* initialize the CABs */
    init_CABs();

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
float get_time(void)
{
    return tick * sys_clock;
}

/*----------------------------------------------------------------------*/
/*                Basic Timer Interrupt Service Routine                 */
/*----------------------------------------------------------------------*/
ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
    digitalWrite(ledPin12, digitalRead(ledPin12) ^ 1);   // toggle LED pin
    sys_clock++;
}

/*===========================================================*/
/*                  LOW-LEVEL PRIMITIVES                     */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* save_context ---  of the task in execution                */
/*-----------------------------------------------------------*/
void save_context(void)
{
    /* TODO: Interrupts will be disabled in SAVE_CONTEXT.
        Check if we should disable them here instead.
        But if we do so, there is no way to know the previous state
        of SREG. Additionally, we may have to reenable them in
        load_context() if we disable here. */
    /*noInterrupts();// < disable interrupts >*/
    pxCurrentContext = vdes[pexe].context;
    SAVE_CONTEXT();
}

/*-----------------------------------------------------------*/
/* load_context ---  of the task to be executed              */
/*-----------------------------------------------------------*/
void load_context(void)
{
    pxCurrentContext = vdes[pexe].context;
    LOAD_CONTEXT();

	/* Simulate a function call end as generated by the compiler.  We will now
	jump to the start of the task the context of which we have just restored. */
	asm volatile ( "ret" );
}

/*===========================================================*/
/*                    LIST MANAGEMENT                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* insert   ---  a task in a queue based on its deadline     */
/*-----------------------------------------------------------*/
 void insert(proc i, queue *que)
{
    /*  que is the pointer to the queue to be used	    	*/
    long dl;         /* deadline of the task to be inserted 	*/
    int  p;          /* pointer to the previous TCB         	*/
    int  q;          /* pointer to the next TCB             	*/

    p = NIL;      /* Start at the beginning of the queue 	*/
    q = *que;
    dl = vdes[i].dline; /*Use deadline of the current TCB	*/

    /* find the element before the insertion point	*/
    while (( q != NIL ) && ( dl >= vdes[q].dline )) {
        /* Surf queue gradually until .. 		*/
        p = q;
        q = vdes[q].next;
    }

    if (p != NIL) {
        vdes[p].next = i;
    }
    else {
        *que = i;
    }

    if (q != NIL) {
        vdes[q].prev = i;
    }

    vdes[i].next = q;
    vdes[i].prev = p;
}


/*-----------------------------------------------------------*/
/* extract   ---  a task from a queue                        */
/*-----------------------------------------------------------*/
proc extract(proc i, queue *que)  /* index ("TCBs num. ") */
                                    /* and queue desired task */
{
    /* auxiliary pointers */
    int p, q;

    /* Linking information from current TCB */
    p = vdes[i].prev;
    q = vdes[i].next;

    if (p == NIL) {
        /* first element  */
        *que = q;
    }
    else {
        vdes[p].next = vdes[i].next;
    }

    if (q != NIL) {
        vdes[q].prev = vdes[i].prev;
    }

    return i;
}

/*-----------------------------------------------------------*/
/* getfirst   ---  extracts a task at the head of a queue    */
/*-----------------------------------------------------------*/
proc getfirst(queue *que)
{
     int q;

    /* pointer to the first element */
     q = *que;
     if (q == NIL) {
        return NIL;
     }
     *que = vdes[q].next;
     vdes[*que].prev = NIL;

     return q;
}

/*-----------------------------------------------------------*/
/* firstdline  ---  returns the deadline of the first task   */
/*-----------------------------------------------------------*/
long firstdline(queue *que)
{
     return vdes[*que].dline;
};

/*-----------------------------------------------------------*/
/* empty    ---  returns TRUE if a queue is empty            */
/*-----------------------------------------------------------*/
int empty(queue *que)
{
     if (*que == NIL) {
         return TRUE;
     }
     else {
         return FALSE;
     }
}

/*===========================================================*/
/*                SCHEDULING MECHANISM                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* schedule --- selects the task with the earliest deadline  */
/*-----------------------------------------------------------*/

void schedule(void)
{
    /* The smaller P_i is, the higher its priority is. */
    /* Has first task in ready queue earlier */
    /* period as an ongoing task? */
    if (firstdline(&ready) < vdes[pexe].dline) {
        vdes[pexe].state = READY;
        insert(pexe, &ready);   /*... then manage the ongoing */
                                /*    task in the ready queue ... */
        dispatch();             /* and make the first task at the ready */
                                /* queue as the current */
    }
};

/*---------------------------------------------------------------*/
/* dispatch --- assigns the cpu to the first ready task          */
/*---------------------------------------------------------------*/
void dispatch(void)
{
     pexe = getfirst(&ready); /* Make task with the earliest */
     vdes[pexe].state = EXE;    /* deadline of the Ready queue */
                                /* as he ongoing task          */
};

/*-----------------------------------------------------------*/
/* abort  --- aborts a task from executing                   */
/*-----------------------------------------------------------*/
void abort(int reason)
{
    switch (reason) {
    case OK:
        /* No error */
        return;
    case TIME_OVERFLOW:
        Serial.println("Missed deadline");
        break;
    case TIME_EXPIRED:
        Serial.println("Lifetime reached");
        break;
    case NO_GUARANTEE:
        Serial.println("Task not schedulable");
        break;
    case NO_TCB:
        Serial.println("Too may tasks");
        break;
    case NO_SEM:
        Serial.println("Too many semaphores");
        break;
    case NO_CAB:
        Serial.println("Too many CABs");
        break;
    default:
        Serial.println("Unexpected behaviour");
    }

    abort();
}

/*-----------------------------------------------------------*/
/* wake_up --- final timer interrupt handling routine        */
/*-----------------------------------------------------------*/

void wake_up( void ) /* timer interrupt handling routine  */
{
    proc p;
    int count = 0;

    save_context();
    sys_clock++;
    if (sys_clock >= LIFETIME) {
        abort(TIME_EXPIRED);
    }
    if (vdes[pexe].type == HARD) {    /* ... must check other*/
                                        /*    tasks in queue ? */
        if (sys_clock > vdes[pexe].dline) {
            abort( TIME_OVERFLOW );
        }
    }
    while (!empty(&zombie) && (firstdline(&zombie) <= sys_clock)) {
        p = getfirst(&zombie);
        util_fact = util_fact - vdes[p].util;
        vdes[p].state = FREE;
        insert(p, &freetcb);
    }

    while (!empty(&idle) && (firstdline(&idle) <= sys_clock)) {
        p = getfirst(&idle);
        vdes[p].dline += (long)vdes[p].period;
        vdes[p].state = READY;
        insert(p, &ready);
        count++;
    }

    if (count > 0) { 	/* Whenever the ready queue changes, */
        schedule();    /* scheduling should be performed */
    }
    load_context();
}

/*===========================================================*/
/*                    TASK' MANAGEMENT                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* create  --- creates a task and puts it in sleep state     */
/*-----------------------------------------------------------*/
proc create (
	char		name[MAXLEN+1],         /* task name */
	proc		(*addr)(),           /* task address */
	int		    type,            /* type (HARD, NRT) */
	float		period,/* period, relative dl or pri */
	float       wcet               /* execution time */
	)
{
    static const long MAX_LONG = pow(2, 16) - 1;

    proc p;

    noInterrupts(); //< disable cpu interrupts >
    p = getfirst(&freetcb);
    if (p == NIL) {
        abort(NO_TCB);
    }
    // Copy task name
    strcpy(vdes[p].name, name);
    vdes[p].addr = addr;
    vdes[p].type = type;
    vdes[p].state = SLEEP;
    vdes[p].period = (int)( period / time_unit );
    vdes[p].wcet = (int)( wcet / time_unit );
    vdes[p].util = wcet / period;
    vdes[p].prt = (int)period;
    vdes[p].dline =  MAX_LONG -PRT_LEV + (long)period;
           /* ^ dline is set by supposing this task is a NRT */
           /* The value is updated if this task is not a NRT */
    if (vdes[p].type == HARD) {
        if (!guarantee(p)) {
            return NO_GUARANTEE;
        }
    }
//      < initialize process stack >
    interrupts(); //< enable cpu interrupts >

    return p;
}

/*-----------------------------------------------------------*/
/* guarantee  --- guarantees the feasibility of a hard task  */
/*-----------------------------------------------------------*/
 int guarantee(proc p)
{
    util_fact = util_fact + vdes[p].util;
    if (util_fact > 1.0) {      /* Processor overload? */
                                /* Newly created task causes    */
                                /* overload on the processor?   */
        util_fact = util_fact - vdes[p].util;
        return FALSE;
    }
    return TRUE;
}


/*-----------------------------------------------------------*/
/* activate  --- inserts a task in the ready queue           */
/*-----------------------------------------------------------*/
 int activate(proc p)
 {
    save_context ();
    if (vdes[p].type == HARD) {     /* update the deadline */
        vdes[p].dline = sys_clock + (long)vdes[p].period;
    }
                        /* period == relative deadline */
    vdes[p].state = READY;
    insert(p, &ready);
    schedule();   /* Whenever the ready queue changes, ... */
    load_context();
 }

/*-----------------------------------------------------------*/
/* sleep  --- suspends itself in a sleep state               */
/*-----------------------------------------------------------*/
int sleep(void)
{
    save_context();
    vdes[pexe].state = SLEEP;
    dispatch(); /* Extracting the 1st task from the ready queue*/
                     /* as the next running task */
    load_context(); /* The next running task is loaded on CPU */
}

/*-----------------------------------------------------------*/
/* end_cycle  --- inserts a task in the idle queue           */
/*-----------------------------------------------------------*/
int end_cycle(void)
{
    long dl;

    save_context();
    dl = vdes[pexe].dline;
    if (sys_clock < dl) {
                     /* Before the start of the next period, */
        vdes[pexe].state = IDLE;        /* going into idle state */
        insert(pexe, &idle);
    }
    else { /* At the same time as the start of the next period,*/
           /* activating immediately */
        dl = dl + (long)vdes[pexe].period;
        vdes[pexe].dline = dl;
        vdes[pexe].state = READY;
        insert(pexe, &ready);
    }
    dispatch();
    load_context();
}

/*-----------------------------------------------------------*/
/* end_process  ---  inserts a task in the idle queue        */
/*-----------------------------------------------------------*/
 int end_process(void)
 {
    noInterrupts(); //< disable cpu interrupts >
    if (vdes[pexe].type == HARD) {
        insert(pexe, &zombie);
    }
    else {
        vdes[pexe].state = FREE;
        insert(pexe, &freetcb);
    }
    dispatch();
    load_context();
    /* load_context will re-enable interrupts and jump directly into the new dispached task */
}

/*-----------------------------------------------------------*/
/* kill  ---  terminates a task                              */
/*-----------------------------------------------------------*/
void kill( proc p )
 {
    noInterrupts(); //< disable cpu interrupts >
    if (pexe == p) {
        end_process();
        return;
    }
    if (vdes[p].state == READY) {
        extract(p, &ready);
    }
    if (vdes[p].state == IDLE) {
        extract(p, &idle);
    }
    if (vdes[p].type == HARD) {
        insert(p, &zombie);
    }
    else {
        vdes[pexe].state = FREE;
        insert(pexe, &freetcb);
    }
    interrupts(); //< enable cpu interrupts >
}


/*===========================================================*/
/*                        SEMAPHORES                         */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* newsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
sem newsem(int n)
{
    sem s;

    noInterrupts(); //< disable cpu interrupts >
    s = freesem;              /* first free semaphore index   */
    if (s == NIL) {
        abort(NO_SEM);
    }
    freesem = vsem[s].next;   /* update the freesem list      */
    vsem[s].count = n;        /* initialize counter           */
    vsem[s].qsem = NIL;       /* initialize sem.queue         */
                                  /* (for wait queue)             */
    interrupts(); //< enable cpu interrupts >

    return s;
}

/*-----------------------------------------------------------*/
/* delsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
void delsem(sem s)
{
    noInterrupts(); //< disable cpu interrupts >
    vsem[s].next = freesem;        /* inserts s at the head */
    freesem = s;                   /* of the freesem list   */
    interrupts(); //< enable cpu interrupts >
    return s;
}

/*-----------------------------------------------------------*/
/* wait  ---  wait for an event                              */
/*-----------------------------------------------------------*/
void wait(sem s)
{
    noInterrupts(); //< disable cpu interrupts >
    if (vsem[s].count > 0) {       	/* When a resource remains, */
        vsem[s].count--;          	    /* a resource is given      */
    }
    else {                      	    /* When resources are exhausted,*/
        save_context();
        vdes[pexe].state = WAIT;      /* the task goes to wait state */
        insert(pexe, &vsem[s].qsem); /* inserted to the wait queue */
        dispatch();/* Extracting the 1st task from the ready queue */
                        /*  as the next running task */
        load_context();  /* The next running task is loaded on CPU */
    }
    interrupts(); //< enable cpu interrupts >
}

/*-----------------------------------------------------------*/
/* signal ---  signals an event                              */
/*-----------------------------------------------------------*/
 void signal(sem s)
{
    proc p;

    noInterrupts(); //< disable cpu interrupts >
    if (!empty(&vsem[s].qsem)) { /* When there is a task that
                              /* is waiting for the semaphore */
        p = getfirst(&vsem[s].qsem);
        vdes[p].state = READY;
        insert(p, &ready);
        save_context();
        schedule();
        load_context();
    }
    else {                  /* When there is not a task waiting */
        vsem[s].count++;    /* returning the resource           */
    }
    interrupts(); //< enable cpu interrupts >
}


/*-----------------------------------------------------------*/
/* get_state ---  returns the state of a task                */
/*-----------------------------------------------------------*/
int get_state(proc p)
{
    return vdes[p].state;
}

/*-----------------------------------------------------------*/
/* get_dline ---  returns the deadline of a task             */
/*-----------------------------------------------------------*/
long get_dline(proc p)
{
    return vdes[p].dline;
}

/*-----------------------------------------------------------*/
/* get_period ---  returns the period of a task              */
/*-----------------------------------------------------------*/
float get_period(proc p)
{
    return vdes[p].period;
}

/*===========================================================*/
/*                  CAB IMPLEMENTATION                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* reserve  ---  reserves a buffer in a CAB                  */
/*-----------------------------------------------------------*/
/*                                                           */
/* @return  returns the index of the first free cab          */
/*-----------------------------------------------------------*/
cab reserve()
{
    cab c;

    noInterrupts();
    /* get the first CAB on the free list */
    c = cabcb.free;
    /* if there are no CABs left, trigger an error */
    if (c == NIL) {
        abort(NO_CAB);
    }
    /* update the list of free CABs */
    cabcb.free = mrbs[c].next;
    interrupts();

    return c;
}

/*-----------------------------------------------------------*/
/* putmes  ---  puts a message in a CAB                      */
/*-----------------------------------------------------------*/
/*                                                           */
/* @param c     CAB index to put the message into            */
/* @param msg   the message to be copied                     */
/* @return      returns the index of the first free cab      */
/*-----------------------------------------------------------*/
void putmes(cab c, pointer msg)
{
    noInterrupts();
    /* if the most recent buffer is not being used, deallocate it */
    if (mrbs[cabcb.mrb].use == 0) {
        mrbs[cabcb.mrb].next = cabcb.free;
        cabcb.free = c;
    }
    /* the most recent buffer is now c */
    cabcb.mrb = c;
    /* copy the msg into c data space */
    memcpy(mrbs[c].data, msg, sizeof(char)*BUFFER_SIZE);
    interrupts();
}


/*-----------------------------------------------------------*/
/* getmes  ---  gets a pointer to the most recent buffer     */
/*-----------------------------------------------------------*/
/*                                                           */
/* @param c     will contain a handle to the most recent     */
/*              buffer                                       */
/* @return      returns a pointer to the message memory area */
/*-----------------------------------------------------------*/
pointer getmes(cab* c)
{
    pointer msg;

    noInterrupts();
    /* set c to the most recent buffer index */
    *c = cabcb.mrb;
    /* mrb is being used by one more entity */
    mrbs[*c].use++;
    /* point to the message memory area */
    msg = mrbs[*c].data;
    interrupts();

    return msg;
}


/*-----------------------------------------------------------*/
/* unget --- deallocates a buffer only if it is not accessed */
/*           and it is not the most recent buffer            */
/*-----------------------------------------------------------*/
/*                                                           */
/* @param c     the idex to the CAB to deallocate            */
/*-----------------------------------------------------------*/
void unget(cab c)
{
    noInterrupts();
    /* reduce the reference counter */
    mrbs[c].use--;
    /* deallocate if no one is using it and it is not   */
    /* the most recent buffer                           */
    if (mrbs[c].use == 0 && c != cabcb.mrb) {
        mrbs[c].next = cabcb.free;
        cabcb.free = c;
    }
    interrupts();
}
