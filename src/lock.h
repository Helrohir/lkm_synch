/*
 * lock.h
 *
 *  Created on: Oct 20, 2012
 *      Author: helrohir
 */

#ifndef LOCK_H_
#define LOCK_H_

#include "Common.h"
#include "linux/spinlock.h"
#include "linux/rwsem.h"

#define ERROR_LOCKNOTCREATED -301
#define ERROR_LOCKNOTREMOVED -302
#define ERROR_LOCKNOTTAKEN -303
#define ERROR_LOCKNOTRELEASED -304

#define GIANTLOCK(giantLockName) DEFINE_RWLOCK(giantLockName);

typedef struct rw_semaphore readWriteLock;
typedef spinlock_t lock;

int rwLock_createLock(readWriteLock* rwLockPtr) {
	init_rwsem(rwLockPtr);
	return EXECUTION_OK;

}

int rwLock_takeReadLock(readWriteLock* rwLockPtr) {
	down_read(rwLockPtr);
	return EXECUTION_OK;
}

int rwLock_takeWriteLock(readWriteLock* rwLockPtr) {
	down_write(rwLockPtr);
	return EXECUTION_OK;
}

int rwLock_releaseReadLock(readWriteLock* rwLockPtr) {
	up_read(rwLockPtr);
	return EXECUTION_OK;
}

int rwLock_releaseWriteLock(readWriteLock* rwLockPtr) {
	up_write(rwLockPtr);
	return EXECUTION_OK;
}


int rwLock_removeLock(readWriteLock* rwLockPtr) {
	//mutex_destroy(lockPtr);
	return EXECUTION_OK;
}

int lock_createLock(lock* lockPtr) {
	spin_lock_init(lockPtr);
	return EXECUTION_OK;

}

int lock_takeLock(lock* lockPtr) {
	spin_lock(lockPtr);
	return EXECUTION_OK;
}

int lock_releaseLock(lock* lockPtr) {

	spin_unlock(lockPtr);
	return EXECUTION_OK;
}

int lock_removeLock(lock* lockPtr) {
	//mutex_destroy(lockPtr);
	return EXECUTION_OK;
}
#endif /* LOCK_H_ */
