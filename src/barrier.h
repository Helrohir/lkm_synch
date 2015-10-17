/*
 * mysyscall.h
 *
 *  Created on: Oct 16, 2012
 *      Author: helrohir
 */

#ifndef MYSYSCALL_H_
#define MYSYSCALL_H_

#include <linux/unistd.h>

#define __NR_sys_get_barrier 17 // int get_barrier(key_t key, int flags) - installazione del della barriera con chiave di identificazione key, e ritorno del codice operativo (barrier-descriptor); flags definisce installazioni esclusive o non, oppure semplici aperture
#define __NR_sys_sleep_on_barrier 31 // int sleep_on_barrier(int bd, int tag) - richiesta di blocco sulla barriera, con indicazione del TAG relativo
#define __NR_sys_awake_barrier 32 // int awake_barrier(int bd, int tag) - riveglio da barriera di processi con un determinato tag
#define __NR_sys_release_barrier 35 // int release_barrier(int bd)

int get_barrier(key_t key, int flags) {
	return syscall(__NR_sys_get_barrier, key, flags);
}

int sleep_on_barrier(int bd, int tag) {
	return syscall(__NR_sys_sleep_on_barrier, bd, tag);
}

int awake_barrier(int bd, int tag) {
	return syscall(__NR_sys_awake_barrier, bd, tag);
}

int release_barrier(int bd) {
	return syscall(__NR_sys_release_barrier, bd);
}

#endif
