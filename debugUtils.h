/**
 *	uDick Real Time Operating System
 *
 *	Diseño de Sistemas Operativos
 *	Curso 2012 - 2013
 *
 *	debugUtils.h
 */
 #include "udick.h"

/*-----------------------------------------------------------*/
/* in_state  ---  Shows the processes for a given state      */
/*-----------------------------------------------------------*/
void in_state(tcb_t *vdes, int state);
void tcb_status(tcb_t *p);
void mrb_status(mrb_t *pr);
void cabcb_status(cabcb_t *pr);
void scb_status(scb_t *pr);
void cabt_status(cab_t *pr);
