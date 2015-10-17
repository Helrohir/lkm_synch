/*
 * Common.h
 *
 *  Created on: Oct 17, 2012
 *      Author: helrohir
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>

#define O_CREAT 1
#define O_EXCL 2

#define NOT_INITIALIZED 0
#define INITIALIZED 1

#define EXECUTION_OK 0 // The execution has completed wihtout errors
#define ERROR_WRONGINPUTDATA -1 //The input data is incoherent
#define ERROR_INITALLOCATIONERROR -2 // An error occurred allocating in the init function
#define ERROR_NOTINITIALZIED -3 // The data structured is been used before being initialized
#define ERROR_ALLOCATIONERROR -4 // An error occurred allocating at runtime

#define TAG_NUMBER 32

#define PRINTK_DEBUG KERN_ERR
#define PRINTK_INFO KERN_ERR
#define PRINTK_ERR KERN_ERR

typedef struct list_head list;

#define KMALLOCFLAGS GFP_KERNEL

//#define REMOVELOCKS

#endif /* COMMON_H_ */
