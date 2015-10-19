/*
 * barrier.h
 *
 *  Created on: Oct 16, 2012
 *      Authors: Nicolo Rivetti, Eleonora Calore
 */

#ifndef MYSYSCALL_H_
#define MYSYSCALL_H_

#include <linux/unistd.h>

#define __NR_sys_get_barrier 17 // int get_barrier(key_t key, int flags) - install a barrier with the given key and returns the barrier descriptor; the flags can ether be simple opening, exclusive or non exclusive installation
#define __NR_sys_sleep_on_barrier 31 // int sleep_on_barrier(int bd, int tag) - request sleepon barrier with the given tag
#define __NR_sys_awake_barrier 32 // int awake_barrier(int bd, int tag) - wakeup from the barrier the procceses with the given tag
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
