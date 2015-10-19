/*
 * wakeupProcess.h
 *
 *  Created on: Oct 22, 2012
 *      Authors: Nicolo Rivetti, Eleonora Calore
 */

#ifndef WAKEUPPROCESS_H_
#define WAKEUPPROCESS_H_

#include "Common.h"
#include <linux/wait.h>
#include <linux/sched.h>

int wakeUpProcess(void *processData){
	wake_up((wait_queue_head_t*) processData);

	return EXECUTION_OK;
}



#endif /* WAKEUPPROCESS_H_ */
