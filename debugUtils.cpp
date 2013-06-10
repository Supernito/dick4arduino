/**
 *	uDick Real Time Operating System
 *
 *	Diseño de Sistemas Operativos
 *	Curso 2012 - 2013
 *
 *	debugUtils.cpp
 */
 #include "Arduino.h"
 #include "debugUtils.h"

/*-----------------------------------------------------------*/
/* in_state  ---  Shows the processes for a given state      */
/*-----------------------------------------------------------*/
void in_state(tcb_t *vdes, int state){
    proc i;
    for (i = 0; i < MAXPROC; i++) {
    	if (vdes[i].state == state){
    		Serial.println("p" + i);
    	}
    }
}

/*-----------------------------------------------------------*/
/* tcb_status  ---  Shows data of one TCB      */
/*-----------------------------------------------------------*/
void tcb_status(tcb_t *pr)
{
    tcb_t p = *pr;
    Serial.print("Name -> "); Serial.print(p.name);
    Serial.println("Type -> " + p.type);
    Serial.println("State -> " + p.state);
    Serial.println("DeadLine -> " + p.dline);
    Serial.println("Period -> " + p.period);
    Serial.println("Priority -> " + p.prt);
    Serial.println("Wost-case -> " + p.wcet);
    Serial.println("Priority -> " + p.prt);
    Serial.print("Utilization -> "); Serial.println(p.util,2);
    Serial.println("Priority -> " + p.prt);
    Serial.println("Next tcb -> " + p.next);
    Serial.println("Previous tcb -> " + p.prev);
}

/*-----------------------------------------------------------*/
/* mrb_status  ---  Shows data of MRB      */
/*-----------------------------------------------------------*/
void mrb_status(mrb_t *pr)
{
    mrb_t p = *pr;
    Serial.println("Next -> " + p.next);
    Serial.println("Use -> " + p.use);
}


/*-----------------------------------------------------------*/
/* cabcb_status  ---  Shows data of CAB      */
/*-----------------------------------------------------------*/
void cabcb_status(cabcb_t *pr)
{
    cabcb_t p = *pr;
    Serial.println("Free -> " + p.free);
    Serial.println("MRB -> " + p.mrb);
    Serial.println("Max buffer -> " + p.max_buf);
    Serial.println("Buffer dim -> " + p.dim_buf);
}
