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
struct tcb      vdes[MAXPROC];  /* array of tcb's	     */
struct scb      vsem[MAXSEM];   /* array of scb's	     */

/*===========================================================*/
/*                     INITIALIZATION                        */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* ini_system ---  system initialization	             */
/*-----------------------------------------------------------*/
void ini_system(float tick)
{
    proc i;

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

    /* initialize the list of free TCBs and semaphores */
    for (i = 0; i < MAXPROC - 1; i++) {
        vdes[i].next = i + 1;
    }
    vdes[MAXPROC-1].next = NIL;

    Serial.println("Initializing list of free semaphores..." );
    delay(500);

    /* initialize the list of free semaphores */
    for (i = 0; i < MAXSEM - 1; i++) {
        vsem[i].next = i + 1;
    }
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
    int* pc;                  /* pointer to context of pexe */

    noInterrupts();// < disable interrupts >
    // pc = vdes[pexe].context;
    // pc[0] = < register_0 >;   /* save register 0            */
    // pc[1] = < register_1 >;   /* save register 1            */
    // <...>;
    // pc[n] = < register_n >;   /* save register n            */
}

/*-----------------------------------------------------------*/
/* load_context ---  of the task to be executed              */
/*-----------------------------------------------------------*/
void load_context(void)
{
    int* pc;                  /* pointer to context of pexe */

    pc = vdes[pexe].context;
    // < register_0 > = pc[0];   /* load register 0         */
    // < register_1 > = pc[1];   /* load register 1         */
    // <...>;
    // < register_n > = pc[n];   /* load register n         */
    // < instruction such as "return from interrupt" >
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
    // TODO: Implementation required
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
    else {
        return TRUE;
    }
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
    interrupts();
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
pointer reserve(cab c)
{
    pointer p;

    noInterrupts(); //< disable cpu interrupts >
//        p = c.free;                 /* get a free buffer        */
//        c.free = p.next;            /* update the free list     */
    interrupts(); //< enable cpu interrupts >

    return p;
}

/*-----------------------------------------------------------*/
/* putmes  ---  puts a message in a CAB                      */
/*-----------------------------------------------------------*/
void putmes(cab c, pointer p)
{
    noInterrupts(); //< disable cpu interrupts >
//    if (c.mrb.use == 0) {           /* if not accessed,     */
//        c.mrb.next = c.free;       /* deallocates the mrb  */
//        c.free = c.mrb;
//    }
//    c.mrb = p;                      /* update the mrb       */
    interrupts(); //< enable cpu interrupts >
}


/*-----------------------------------------------------------*/
/* getmes  ---  gets a pointer to the most recent buffer     */
/*-----------------------------------------------------------*/
pointer getmes(cab c)
{
    pointer p;
    noInterrupts(); //< disable cpu interrupts >
//        p = c.mrb;                 /* get the pointer to mrb    */
//        p.use = p.use + 1;         /* increment the counter     */
    interrupts(); //< enable cpu interrupts >

    return p;
}


/*-----------------------------------------------------------*/
/* unget --- deallocates a buffer only if it is not accessed */
/*           and it is not the most recent buffer            */
/*-----------------------------------------------------------*/
void unget(cab c, pointer p)
{
    noInterrupts(); //< disable cpu interrupts >
//        p.use = p.use - 1;		/*decrement the counter      */
//        if ((p.use == 0) && (p ! = c.mrb)) {
//            p.next = c.free;
//            c.free = p;
//        }
    interrupts(); //< enable cpu interrupts >
}
