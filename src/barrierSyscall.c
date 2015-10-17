/*
 * syscall2.c
 *
 *  Created on: Oct 16, 2012
 *      Author: helrohir
 */

//#include "barrier.h"
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/unistd.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "Common.h"
#include "barrierDataStructure.h"

#define __NR_sys_get_barrier 17 // int get_barrier(key_t key, int flags) - installazione del della barriera con chiave di identificazione key, e ritorno del codice operativo (barrier-descriptor); flags definisce installazioni esclusive o non, oppure semplici aperture
#define __NR_sys_sleep_on_barrier 31 // int sleep_on_barrier(int bd, int tag) - richiesta di blocco sulla barriera, con indicazione del TAG relativo
#define __NR_sys_awake_barrier 32 // int awake_barrier(int bd, int tag) - riveglio da barriera di processi con un determinato tag
#define __NR_sys_release_barrier 35 // int release_barrier(int bd)
extern void *sys_call_table[];

static long previousSysCall_get_barrier = 0x0;
static long previousSysCall_sleep_on_barrier = 0x0;
static long previousSysCall_awake_barrier = 0x0;
static long previousSysCall_release_barrier = 0x0;

asmlinkage int sys_get_barrier(key_t key, int flags) {
	int bd = -1;
	int ret;
	printk(KERN_ERR "barrier_getOrCreateBarrier(%d, %d)\n", key, flags);
	ret = barrier_getOrCreateBarrier((int) key, flags, &bd);
	printk(KERN_ERR "barrier_getOrCreateBarrier: %d, bd: %d\n", ret, bd);
	return bd;
}

asmlinkage int sys_sleep_on_barrier(int bd, int tag) {
	//TODO MANAGE ERRNO
	DECLARE_WAIT_QUEUE_HEAD(waitQueue);

	if (barrier_addProcessOnTag(bd, tag, &waitQueue) != EXECUTION_OK) {
		return -1;
	}
	printk( PRINTK_DEBUG "interruptible_sleep_on(&waitQueue)\n");
	interruptible_sleep_on(&waitQueue);
	printk( PRINTK_DEBUG "waken up\n");
	if (signal_pending(current)) {
		printk( PRINTK_DEBUG "waken up by signal\n");
		if (barrier_removeProcessOnTag(bd, tag,
						(int) &waitQueue)!= EXECUTION_OK) {
			return -1;
		}
	}else{
		printk( PRINTK_DEBUG "waken up by syscall (maybe)\n");
	}

	return 0;
}

asmlinkage int sys_awake_barrier(int bd, int tag) {
	//TODO MANAGE ERRNO
	int ret;
	printk(KERN_ERR "barrier_removeAllProcessesOnTag(%d, %d)\n", bd, tag);
	ret = barrier_removeAllProcessesOnTag(bd, tag);
	printk(KERN_ERR "barrier_removeAllProcessesOnTag: %d", ret);

	return ret;
}

asmlinkage int sys_release_barrier(int bd) {
	//TODO MANAGE ERRNO
	int ret;
	printk(KERN_ERR "barrier_removeBarrier(%d)\n", bd);
	ret = barrier_removeBarrier(bd);
	printk(KERN_ERR "barrier_removeBarrier: %d\n", ret);

	return ret;
}

int registerMySyscall(void) {

	previousSysCall_get_barrier = (long) sys_call_table[__NR_sys_get_barrier];
	sys_call_table[__NR_sys_get_barrier] = sys_get_barrier;

	previousSysCall_sleep_on_barrier =
			(long) sys_call_table[__NR_sys_sleep_on_barrier];
	sys_call_table[__NR_sys_sleep_on_barrier] = sys_sleep_on_barrier;

	previousSysCall_awake_barrier =
			(long) sys_call_table[__NR_sys_awake_barrier];
	sys_call_table[__NR_sys_awake_barrier] = sys_awake_barrier;

	previousSysCall_release_barrier =
			(long) sys_call_table[__NR_sys_release_barrier];
	sys_call_table[__NR_sys_release_barrier] = sys_release_barrier;

	return 0;
}

int unregisterMySyscall(void) {
	sys_call_table[__NR_sys_get_barrier] = (void*) previousSysCall_get_barrier;

	sys_call_table[__NR_sys_sleep_on_barrier] =
			(void*) previousSysCall_sleep_on_barrier;

	sys_call_table[__NR_sys_awake_barrier] =
			(void*) previousSysCall_awake_barrier;

	sys_call_table[__NR_sys_release_barrier] =
			(void*) previousSysCall_release_barrier;

	return 0;
}
