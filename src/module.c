/*
 * module.c
 *
 *  Created on: Oct 16, 2012
 *      Authors: Nicolo Rivetti, Eleonora Calore
 */

#define __SMP__ //TODO

#include <linux/module.h>
#include <linux/kernel.h>

#include "barrierDataStructure.h"
#include "barrierSyscall.h"

static int __init init_mysyscall(void) {
	//TODO Manage error
	printk(KERN_INFO "My Syscall init_module() called\n");

	registerMySyscall();

	barrier_Init();

	return 0;
}

static void __exit cleanup_mysyscall(void) {
	//TODO Manage error
	printk(KERN_INFO "My Syscall cleanup_module() called\n");

	barrier_Cleanup();

	unregisterMySyscall();
}

module_init(init_mysyscall);
module_exit(cleanup_mysyscall);
