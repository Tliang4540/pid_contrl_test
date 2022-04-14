#ifndef _MINISCH_H_
#define _MINISCH_H_

#include <setjmp.h>

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;
typedef signed int int16_t;
typedef signed char int8_t;
typedef signed long int32_t;

#define MAXTASKS 5
extern bit MINI_SCH_RUN;
extern volatile uint8_t timers[MAXTASKS];
#define _SS static jmp_buf env; static unsigned char started; if(started) longjmp(env,1); started=1;
#define _EE started=0; return 255;

#define WaitX(tickets)  do { if(!setjmp(env)) return tickets;} while(0);
#define WaitUntil(A)    do { WAITX(0);} while((A)==0);

#define RunTask(TaskName,TaskID)  do { if (timers[TaskID]==0) timers[TaskID]=TaskName(); }  while(0); 
#define RunTaskA(TaskName,TaskID) do { if (timers[TaskID]==0) {timers[TaskID]=TaskName(); continue;}}while(0);    

#define CallSub(SubTaskName) do {uint8_t currdt; WaitX(0); currdt=SubTaskName(); if(currdt!=0xFF) return currdt;} while(0);
#define InitTasks() do {uint8_t i; for(i=MAXTASKS;i>0 ;i--) timers[i-1]=0; } while(0);
#define UpdateTimers() do{uint8_t i; for(i=MAXTASKS;i>0 ;i--){if((timers[i-1]!=0)&&(timers[i-1]!=0xFF)) timers[i-1]--;}} while(0);

typedef struct
{
	uint8_t sem_tick;
	uint8_t sem_value;
}sem_t;
#define Sem_Init(sem, value)    		(sem.sem_value = value)
#define Sem_Wait(sem,tick)      		do{sem.sem_tick = tick; setjmp(env); if(sem.sem_tick && (0 == sem.sem_value)){ if(sem.sem_tick != 0xff)sem.sem_tick--; return 1; } }while(0);
#define Sem_Release(sem)	    		(sem.sem_value++)
#define Sem_Take(sem)					(sem.sem_value--)
#define Sem_Send(sem, val)				do{ setjmp(env); if(sem.sem_value){return 1;}else sem.sem_value = val; }while(0);
#define Sem_Rece(sem)					(sem.sem_value = 0)
#define Event_Send(sem, val)			(sem.sem_value |= 1 << val)
#define Event_Rece(sem, val)			(sem.sem_value &= ~(1 << val))

#endif