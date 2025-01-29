#ifndef __tHOOKS_H
#define __tHOOKS_H

#include "tinyOS.h"

void tHooksCpuIdle(void);
void tHooksSysTickHandler(void);
void tHooksTaskSchedule (tTask * from, tTask * to);
void tHooksTaskInit(tTask * task);


#endif
