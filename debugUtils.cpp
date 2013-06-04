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
