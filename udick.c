/*
	Test 1
	uDick.cpp
	Miguel Ángel Ortiz
	25-04-2013
*/

#include "udick.h"

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
void     save_context ( void )
{
int      *pc;                  /* pointer to context of pexe */
     // < disable interrupts >
     // pc = vdes[pexe].context;
     // pc[0] = < register_0 >;   /* save register 0            */
     // pc[1] = < register_1 >;   /* save register 1            */
     // <...>;
     // pc[n] = < register_n >;   /* save register n            */
}

/*-----------------------------------------------------------*/
/* load_context ---  of the task to be executed              */
/*-----------------------------------------------------------*/
void     load_context ( void )
{
int      *pc;                  /* pointer to context of pexe */
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
 void     insert ( proc i, queue *que )
{
	/*  *que is the pointer to the queue to be used	    	*/
     long dl;         /* deadline of the task to be inserted 	*/
     int  p;          /* pointer to the previous TCB         	*/
     int  q;          /* pointer to the next TCB             	*/
	p = NIL;      /* Start at the beginning of the queue 	*/
	q = *que;
	dl = vdes[i].dline; /*Use deadline of the current TCB	*/

	  /* find the element before the insertion point	*/
     while ( ( q != NIL ) && ( dl >= vdes[q].dline ) ) {
      /* Surf queue gradually until .. 		*/
        p = q;
        q = vdes[q].next;
     }
     if ( p != NIL ) vdes[p].next = i;
     else *que = i;
     if ( q != NIL ) vdes[q].prev = i;
     vdes[i].next = q;
     vdes[i].prev = p;
}


/*-----------------------------------------------------------*/
/* extract   ---  a task from a queue                        */
/*-----------------------------------------------------------*/
//proc extract ( proc i, queue *que ) /* index ("TCBs num. ")  
//				       and queue desired task */
//{
//int      p, q;                         /* auxiliary pointers  */
//     p = vdes[i].prev; /*Linking information from current TCB */
//     q = vdes[i].next;
//     if ( p == NIL ) *que = q;              /* first element  */
//     else vdes[p].next = vdes[i].next;
//     if ( q != NIL ) vdes[q].prev = vdes[i].prev;
//     return i;
//}

/*-----------------------------------------------------------*/
/* getfirst   ---  extracts a task at the head of a queue    */
/*-----------------------------------------------------------*/
proc getfirst ( queue *que )
{
     int     q;        	     /* pointer to the first element */
     q = *que;
     if ( q == NIL ) return NIL;
     *que = vdes[q].next;
     vdes[*que].prev = NIL;
     return q;
}

/*-----------------------------------------------------------*/
/* firstdline  ---  returns the deadline of the first task   */
/*-----------------------------------------------------------*/
long    firstdline ( queue *que )
{
     return vdes[*que].dline;
};

/*-----------------------------------------------------------*/
/* empty    ---  returns TRUE if a queue is empty            */
/*-----------------------------------------------------------*/
int empty ( queue *que )
{
     if ( *que == NIL )
         return TRUE;
     else
         return FALSE;
}

/*===========================================================*/
/*                SCHEDULING MECHANISM                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* schedule --- selects the task with the earliest deadline  */
/*-----------------------------------------------------------*/

void    schedule ( void )
{
           /* The smaller P_i is, the higher its priority is.    */
	   /* Has first task in ready queue earlier 
	      period as an ongoing task?		         */
//     if ( firstdline ( ready ) < vdes[pexe].dline ) {
       vdes[pexe].state = READY;
//     insert ( pexe, &ready );/*... then manage the ongoing 
			       /*  task in the ready queue ...   */
       dispatch (); /* and make the first task at the ready 
		   queue as the current                          */
//     }
};

/*---------------------------------------------------------------*/
/* dispatch --- assigns the cpu to the first ready task          */
/*---------------------------------------------------------------*/
void    dispatch ( void )
{
//     pexe = getfirst ( &ready );/* Make task with the earliest */ 
//     vdes[pexe].state = RUN;    /* deadline of the Ready queue */
				  /* as he ongoing task          */
};

/*-----------------------------------------------------------*/
/* wake_up --- final timer interrupt handling routine        */
/*-----------------------------------------------------------*/

void   wake_up ( void ) /* timer interrupt handling routine  */
{
proc      p;
int       count = 0;
//  save_context ();
  sys_clock++;
//   if ( sys_clock >= LIFETIME ) abort( TIME_EXPIRED );
//   if ( vdes[pexe].type == HARD )      /* ... must check other*/   
                                        /*    tasks in queue ? */
//        if ( sys_clock > vdes[pexe].dline )
//             abort( TIME_OVERFLOW );
//   while (!empty(zombie) && 
//            (firstdline(zombie) <= sys-clock)) {
//        p = getfirst( &zombie );
//        util_fact = util_fact - vdes[p].util;
       vdes[p].state = FREE;
//        insert ( p, &freetcb );
//   }

//   while (!empty(idle) && ( firstdline(idle) <= sys_clock)){
//        p = getfirst ( &idle );
       vdes[p].dline += (long)vdes[p].period;
       vdes[p].state = READY;
//        insert ( p, &ready );
       count++;
//   }

//    if ( count > 0 )  	/* Whenever the ready queue changes, */
//    schedule ();      	   /* scheduling should be performed */
//    load_context ();
}

/*===========================================================*/
/*                    TASK' MANAGEMENT                       */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* create  --- creates a task and puts it in sleep state     */
/*-----------------------------------------------------------*/
proc   create ( 
	char		name[MAXLEN+1],         /* task name */
	proc		(*addr)(),           /* task address */
	int		type,            /* type (HARD, NRT) */
	float		period,/* period, relative dl or pri */
	float wcet )                       /* execution time */
{
proc p;
    cli(); //< disable cpu interrupts >
//    p = getfirst ( &freetcb );
//    if ( p == NIL ) abort( NO_TCB );
//    vdes[p].name = name; /* Actually, characters should be copied */
    vdes[p].addr = addr;
    vdes[p].type = type;
    vdes[p].state = SLEEP;
    vdes[p].period = (int)( period / time_unit );
    vdes[p].wcet = (int)( wcet / time_unit );
    vdes[p].util = wcet / period;
    vdes[p].prt = (int)period;
   // vdes[p].dline = MAX_LONG -PRT_LEV + (long)period;
           /* ^ dline is set by supposing this task is a NRT */
           /* The value is updated if this task is not a NRT */
    if ( vdes[p].type == HARD ) 
//         if ( !guarantee( p ) ) return NO_GUARANTEE;
//    < initialize process stack >
    sei(); //< enable cpu interrupts >
    return p;
}

/*-----------------------------------------------------------*/
/* guarantee  --- guarantees the feasibility of a hard task  */
/*-----------------------------------------------------------*/
// int   guarantee ( proc p )
//{
//      util_fact = util_fact + vdes[p].util;
//      if ( util_fact > 1.0 ) {  /*Processor overload? 
//  				Newly created task causes 
//				overload on the processor?   */
//          util_fact = util_fact - vdes[p].util;
//          return FALSE;
//     }
//     else return TRUE;
//}


/*-----------------------------------------------------------*/
/* activate  --- inserts a task in the ready queue           */
/*-----------------------------------------------------------*/
// int      activate ( proc p )
// {
//      save_context ();
//      if ( vdes[p].type == HARD )      /* update the deadline */
//           vdes[p].dline = sys_clock + (long)vdes[p].period;
                              /* period == relative deadline */
//      vdes[p].state = READY;
//      insert ( p, &ready );
//      schedule ();   /* Whenever the ready queue changes, ... */
//      load_context ();
// }

/*-----------------------------------------------------------*/
/* sleep  --- suspends itself in a sleep state               */
/*-----------------------------------------------------------*/
//  int     sleep ( void )
//  {
//  save_context ();
//      vdes[pexe].state = SLEEP;
//  dispatch ();/* Extracting the 1st task from the ready queue*/
              			 /* as the next running task */
//  load_context (); /* The next running task is loaded on CPU */
//  }

/*-----------------------------------------------------------*/
/* end_cycle  --- inserts a task in the idle queue           */
/*-----------------------------------------------------------*/
//  int      end_cycle ( void )
//  {
//  long     dl;
//  save_context ();
//    dl = vdes[pexe].dline;
//    if ( sys_clock < dl ) { 
                     /* Before the start of the next period, */
//      vdes[pexe].state = IDLE;        /* going into idle state */
//    insert ( pexe, &idle );
//    }
//    else { /* At the same time as the start of the next period,*/
         /* activating immediately */
//        dl = dl + (long)vdes[pexe].period; 
//        vdes[pexe].dline = dl;
//        vdes[pexe].state = READY;
//      insert ( pexe, &ready );
//    }
//  dispatch ();
//  load_context ();
//  }

/*-----------------------------------------------------------*/
/*  ---            Example of periodic task      ---         */
/*-----------------------------------------------------------*/
//proc    cycle()
//{
//     while (TRUE)  {
//           < periodic code >
//           end_cycle;
//      }
//}

/*-----------------------------------------------------------*/
/* end_process  ---  inserts a task in the idle queue        */
/*-----------------------------------------------------------*/
// int      end_process ( void )
// {
//     cli(); //< disable cpu interrupts >
//     if ( vdes[pexe].type == HARD )
//          insert ( pexe, &zombie );
//     else {
//         vdes[pexe].state = FREE;
//         insert ( pexe, &freetcb );
//     }
//     dispatch ();
//     load_context ();
//}

/*-----------------------------------------------------------*/
/* kill  ---  terminates a task                              */
/*-----------------------------------------------------------*/
//void      kill ( proc p )
// {
//     cli(); //< disable cpu interrupts >
//     if ( pexe == p ) {
//          end_process ();
//          return;
//     }
//     if ( vdes[p].state == READY ) extract ( p, &ready );
//     if ( vdes[p].state == IDLE ) extract ( p, &idle );
//     if ( vdes[p].type == HARD )
//          insert ( p, &zombie );
//     else {
//          vdes[pexe].state = FREE;
//          insert ( pexe, &freetcb );
//     }
//     sei(); //< enable cpu interrupts >
//}

/*===========================================================*/
/*                        SEMAPHORES                         */
/*===========================================================*/

/*-----------------------------------------------------------*/
/* newsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
//  sem      newsem (int n )
//  {
//  sem      s;
//     cli(); //< disable cpu interrupts >
//     s = freesem;              /* first free semaphore index   */
//     if ( s == NIL ) abort( NO_SEM );
//     freesem = vsem[s].next;   /* update the freesem list      */
//     vsem[s].count = n;        /* initialize counter           */
//     vsem[s].qsem = NIL;       /* initialize sem.queue         */
                             /* (for wait queue)             */
//     sei(); //< enable cpu interrupts >
//     return s;
//  }

/*-----------------------------------------------------------*/
/* delsem  ---  allocates and initializes a semaphore        */
/*-----------------------------------------------------------*/
//  void      delsem ( sem s )
//  {
//       cli(); //< disable cpu interrupts >
//       vsem[s].next = freesem;        /* inserts s at the head */
//       freesem = s;                   /* of the freesem list   */
//       sei(); //< enable cpu interrupts >
//       return s;
//  }

/*-----------------------------------------------------------*/
/* wait  ---  wait for an event                              */
/*-----------------------------------------------------------*/
//  void      wait ( sem s )
//  {
//   cli(); //< disable cpu interrupts >
//   if ( vsem[s].count > 0 )        	/* When a resource remains, */
//         vsem[s].count --;          	/* a resource is given      */
//   else {                      	    /* When resources are exhausted,*/
//  	save_context ();
//  	vdes[pexe].state = WAIT;     /* the task goes to wait state */
//  	insert (pexe, &vsem[s].qsem); /* inserted to the wait queue */
//  	dispatch ();/* Extracting the 1st task from the ready queue */
        				/* as the next running task */
//  	load_context ();  /* The next running task is loaded on CPU */
//   }
//   sei(); //< enable cpu interrupts >
//  }

/*-----------------------------------------------------------*/
/* signal ---  signals an event                              */
/*-----------------------------------------------------------*/
// void     signal ( sem s )
// {
//     proc p;
//     cli(); //< disable cpu interrupts >
//     if (!empty( vsem[s].qsem )) { /* When there is a task that 
        			 /* is waiting for the semaphore */
//         p = getfirst ( &vsem[s].qsem );
//         vdes[p].state = READY;
//         insert ( p, &ready );
//         save_context ();
//         schedule ();
// 	load_context ();
//     }
//     else                /* When there is not a task waiting */
//     vsem[s].count++;    /* returning the resource           */
//     sei(); //< enable cpu interrupts >
// }


/*-----------------------------------------------------------*/
/* get_state ---  returns the state of a task                */
/*-----------------------------------------------------------*/
// int      get_state ( proc p )
// {
//     return vdes[p].state;
// }

/*-----------------------------------------------------------*/
/* get_dline ---  returns the deadline of a task             */
/*-----------------------------------------------------------*/
// long     get_dline ( proc p )
// {
//     return vdes[p].dline;
// }

/*-----------------------------------------------------------*/
/* get_period ---  returns the period of a task              */
/*-----------------------------------------------------------*/
// float      get_period ( proc p )
// {
//     return vdes[p].period;
// }

/*===========================================================*/
/*                  CAB IMPLEMENTATION                       */
/*===========================================================*/

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
//  pointer     reserve (cab c)
//  {
//  pointer     p;
//       cli(); //< disable cpu interrupts >
//       p = c.free;                 /* get a free buffer        */
//       c.free = p.next;            /* update the free list     */
//       return(p);
//       sei(); //< enable cpu interrupts >
//  }

/*-----------------------------------------------------------*/
/* putmes  ---  puts a message in a CAB                      */
/*-----------------------------------------------------------*/
//  void     putmes(cab c, pointer p)
//  {
//       cli(); //< disable cpu interrupts >
//       if (c.mrb.use == 0) {           /* if not accessed,     */
//            c.mrb.next = c.free;       /* deallocates the mrb  */     
//            c.free = c.mrb;
//       }
//       c.mrb = p;                      /* update the mrb       */
//       sei(); //< enable cpu interrupts >
//  }


/*-----------------------------------------------------------*/
/* getmes  ---  gets a pointer to the most recent buffer     */
/*-----------------------------------------------------------*/
//  pointer     getmes(cab c)
//  {
//  pointer     p;
//       cli(); //< disable cpu interrupts >
//       p = c.mrb;                 /* get the pointer to mrb    */
//       p.use = p.use + 1;         /* increment the counter     */
//       return(p);
//       sei(); //< enable cpu interrupts >
//  }


/*-----------------------------------------------------------*/
/* unget --- deallocates a buffer only if it is not accessed */
/*           and it is not the most recent buffer            */
/*-----------------------------------------------------------*/
//  void     unget(cab c, pointer p)
//  {
//       cli(); //< disable cpu interrupts >
//       p.use = p.use - 1;		/*decrement the counter      */
//       if ((p.use == 0) && (p ! = c.mrb)) {
//            p.next = c.free;
//            c.free = p;
//       }
//       sei(); //< enable cpu interrupts >
//  }
