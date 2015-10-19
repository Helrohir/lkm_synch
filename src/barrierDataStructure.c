/*
 * barrierDataStructure.c
 *
 *  Created on: Oct 17, 2012
 *      Authors: Nicolo Rivetti, Eleonora Calore
 */

#include "Common.h"
#include "lock.h"
#include "barrierDataStructure.h"
#include "tagDataStructure.h"

#define BARRIER_BUCKETS_NUM 16
#define HASH_SHIFT 4
#define HASH_MASK 15

struct barrier_data_struct {
	int key; // Barrier Key
	int bd; //Barrier descriptor
	int flags; //Barrier installation flag
	void* tagsHead; //Pointer to tags data structure
	list hashOverflowList; //Overflow hash list
};
typedef struct barrier_data_struct barrierNode;

static int isInitialized = NOT_INITIALIZED;
static list barrierHashTable[BARRIER_BUCKETS_NUM];
static lock bucketLocks[BARRIER_BUCKETS_NUM];

static readWriteLock giantLock;

static int sn = 5;
int barrier_Init(void);
int barrier_Cleanup(void);

int barrier_getOrCreateBarrier(int key, int flags, int *bd);
int barrier_addProcessOnTag(int bd, int tag, void* processData);
int barrier_removeProcessOnTag(int bd, int tag, int removeProcessData);
int barrier_removeAllProcessesOnTag(int bd, int tag);
int barrier_removeBarrier(int bd);

int _to_String(int bd);

static int _getBD(int hash);
static int _get_hash_from_bd(int bd);
static int _get_hash_from_key(int key);

static int _create_barrier(int key, int bd, int flags);
static int _lookup_barrier(int bd, int hash, barrierNode** barrierNodePointer);

/*
 * Initialize the Barrier Data Structure and creates the Giant Lock
 */

int barrier_Init(void) {
	int i, ret;

	if (isInitialized == INITIALIZED)
		return EXECUTION_OK;

	rwLock_takeWriteLock(&giantLock);

	// Create the giant lock

	ret = EXECUTION_OK;
	for (i = 0; i < BARRIER_BUCKETS_NUM; i++) {
		ret = lock_createLock(&bucketLocks[i]);
		if (ret != EXECUTION_OK) {
			return ret;
		}
	}

	// Initialize the head of the overflow hash list for each bucket
	for (i = 0; i < BARRIER_BUCKETS_NUM; i++) {
		INIT_LIST_HEAD(&barrierHashTable[i]);
	}

	isInitialized = INITIALIZED;

	rwLock_releaseWriteLock(&giantLock);

	return ret;
}

/*
 * Cleanup the Barrier Data Structure and removes the Giant Lock
 */
int barrier_Cleanup(void) {
	int i = 0;
	int ret;
	list* ptr;
	list* next;
	barrierNode* entry;

	rwLock_takeWriteLock(&giantLock);
	if(isInitialized != INITIALIZED){
		rwLock_releaseWriteLock(&giantLock);
		return ERROR_NOTINITIALZIED;
	}

	ret = EXECUTION_OK;

	for(i = 0; i < BARRIER_BUCKETS_NUM; i++){
		list_for_each_safe(ptr, next, &barrierHashTable[i]){
			entry = list_entry(ptr, barrierNode, hashOverflowList);
			tag_removeAllTags(entry->tagsHead);
			list_del(&entry->hashOverflowList);
			kfree(entry);
		}
	}

	for (i = 0; i < BARRIER_BUCKETS_NUM; i++) {
		ret = lock_removeLock(&bucketLocks[i]);
		if (ret != EXECUTION_OK) {
			return ret;
		}
	}

	isInitialized = NOT_INITIALIZED;
	rwLock_releaseWriteLock(&giantLock);
	return ret;
}

/*
 * Given a key it returns the matching barrier, the behavior is changed with respect to the flags:
 * - NONE: it returns the barrier if the barrier already exists and is not O_EXCL
 * - O_CREAT: it returns the barrier if the barrier already exists and is not O_EXCL. If the barrier
 * does not exists, then it creates a new one
 * - O_EXCL: if a no matching barrier exists, it creates a new one
 * @key: the barrier key
 * @flags: the barrier installation flags
 */
int barrier_getOrCreateBarrier(int key, int flags, int *bd) {
	int hash;
	list * hash_overflow_head;
	list* ptr;
	int ret;
	barrierNode* entry;

	rwLock_takeReadLock(&giantLock);
	if (isInitialized != INITIALIZED) {
		rwLock_releaseReadLock(&giantLock);
		return ERROR_NOTINITIALZIED;
	}

	// Get the hash of the key to retrieve the corresponding bucket

	hash = _get_hash_from_key(key);

	// Taking bucket lock
	lock_takeLock(&bucketLocks[hash]);


	hash_overflow_head= &barrierHashTable[hash];

	// Lookup if a matching barrier exists in the overflow hash list

	entry = NULL;
	list_for_each(ptr,hash_overflow_head)
	{
		entry = list_entry(ptr, barrierNode, hashOverflowList);
		if (entry->key == key) {
			break;
		}
	}

	ret = EXECUTION_OK;
	if (entry == NULL || entry->key != key) {
		// If the barrier does not exists
		if ((flags & O_CREAT) == O_CREAT || (flags & O_EXCL) == O_EXCL) {
			// If the flags allows the creation of a new barrier, the barrier is created and the
			// barrier descritor is returned
			*bd = _getBD(hash);
			ret = _create_barrier(key, *bd, flags);
		} else {
			// If the flags do not allow the creation of a new barrier, an error is raised
			lock_releaseLock(&bucketLocks[hash]);
			rwLock_releaseReadLock(&giantLock);
			return ERROR_NOSUCHBARRIER;
		}
	} else {
		// If the barrier exists
		if ((flags & O_EXCL) == O_EXCL || (entry->flags & O_EXCL) == O_EXCL) {
			// If flags are O_EXCK or the found barrier is O_EXCL, an error is raised
			lock_releaseLock(&bucketLocks[hash]);
			rwLock_releaseReadLock(&giantLock);
			return ERROR_EXCLUSIVEBARRIER;
		} else {
			// If the flags and the barrier flags allows to open it, the barrier descriptor is returned
			*bd = entry->bd;
		}
	}

	// Releasing the bucket lock
	lock_releaseLock(&bucketLocks[hash]);
	rwLock_releaseReadLock(&giantLock);
	return ret;
}

/*
 * Given the Barrier Descriptor and a tag, if a matching barrier exists, the current process is
 * added to the sleeping queue on the given tag. A pointer to remove the specific process from the
 * data structure is returned
 * @bd: the barrier descriptor
 * @tag: the tag on which the process must sleep
 * OUT:
 * @removeProcessPointer: the address where the pointer needed to remove the process will be stored
 */
int barrier_addProcessOnTag(int bd, int tag, void* processData) {
	int hash;
	int ret;
	barrierNode* barrieNodePtr;

	rwLock_takeReadLock(&giantLock);
	if (isInitialized != INITIALIZED) {
		rwLock_releaseReadLock(&giantLock);
		return ERROR_NOTINITIALZIED;
	}

	// Taking bucket lock

	hash = _get_hash_from_bd(bd);
	lock_takeLock(&bucketLocks[hash]);


	ret = EXECUTION_OK;


	ret = _lookup_barrier(bd, hash, &barrieNodePtr);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}
	// The process is added to the retrieved barrier on the given tag
	ret = tag_addProcessOnTag(barrieNodePtr->tagsHead, tag, processData);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}

	// Releasing the bucket lock
	lock_releaseLock(&bucketLocks[hash]);
	rwLock_releaseReadLock(&giantLock);
	return EXECUTION_OK;
}

/*
 * Given a barrier descriptor and a tag, all the process sleeping on the tag
 * @bd: barrier descriptor
 * @tag: sleeping queue tag
 */
int barrier_removeAllProcessesOnTag(int bd, int tag) {
	int hash;
	int ret;
	barrierNode* barrieNodePtr;

	rwLock_takeReadLock(&giantLock);
	if (isInitialized != INITIALIZED) {
		rwLock_releaseReadLock(&giantLock);
		return ERROR_NOTINITIALZIED;
	}

	// Taking bucket lock
	hash = _get_hash_from_bd(bd);
	lock_takeLock(&bucketLocks[hash]);

	ret = EXECUTION_OK;
	// Lookup the barrier

	ret = _lookup_barrier(bd, hash, &barrieNodePtr);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}

	// Removing all process on a given tag
	ret = tag_removeAllProcessesOnTag(barrieNodePtr->tagsHead, tag);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}

	// Releasing the bucket lock
	lock_releaseLock(&bucketLocks[hash]);
	rwLock_releaseReadLock(&giantLock);
	return EXECUTION_OK;
}

int barrier_removeProcessOnTag(int bd, int tag, int removeProcessData) {
	int hash;
	int ret;
	barrierNode* barrieNodePtr;

	rwLock_takeReadLock(&giantLock);
	if (isInitialized != INITIALIZED) {
		rwLock_releaseReadLock(&giantLock);
		return ERROR_NOTINITIALZIED;
	}
	// Taking bucket lock

	hash = _get_hash_from_bd(bd);
	lock_takeLock(&bucketLocks[hash]);


	ret = EXECUTION_OK;
	// Lookup the barrier

	ret = _lookup_barrier(bd, hash, &barrieNodePtr);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}
	//printf("DEBUG: tag_removeProcessOnTag(%p, %d, %d)", barrieNodePtr->tagsHead, tag, removeProcessData);
	// Removing all processo on all tags
	ret = tag_removeProcessOnTag(barrieNodePtr->tagsHead, tag,
			removeProcessData);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}
	// Releasing the bucket lock
	lock_releaseLock(&bucketLocks[hash]);
	rwLock_releaseReadLock(&giantLock);

	return EXECUTION_OK;
}

/*
 * Given a barrier descriptor, the matching barrier is removed from the corresponding hash
 * overflow list and destroyed. Any sleeping process on any tag is waked up and the process
 * and tag data structure are removed
 * @bd: barrier descriptor
 */
int barrier_removeBarrier(int bd) {
	int hash;
	int ret;
	barrierNode* barrieNodePtr;

	rwLock_takeReadLock(&giantLock);
	if (isInitialized != INITIALIZED) {
		rwLock_releaseReadLock(&giantLock);
		return ERROR_NOTINITIALZIED;
	}

	// Taking bucket lock

	hash = _get_hash_from_bd(bd);
	lock_takeLock(&bucketLocks[hash]);

	ret = EXECUTION_OK;
	// Lookup the barrier

	ret = _lookup_barrier(bd, hash, &barrieNodePtr);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}

	// Removing all processo on all tags
	ret = tag_removeAllTags(barrieNodePtr->tagsHead);
	if (ret != EXECUTION_OK) {
		lock_releaseLock(&bucketLocks[hash]);
		rwLock_releaseReadLock(&giantLock);
		return ret;
	}

	list_del(&barrieNodePtr->hashOverflowList);
	kfree(barrieNodePtr);

	// Releasing the bucket lock
	lock_releaseLock(&bucketLocks[hash]);
	rwLock_releaseReadLock(&giantLock);

	return EXECUTION_OK;
}

int _to_String(int bd) {
	int hash = _get_hash_from_bd(bd);
	list* overflow_list_hash = &barrierHashTable[hash];
	list* ptr;
	barrierNode* entry;
	list_for_each(ptr, overflow_list_hash)
	{
		entry = list_entry(ptr, barrierNode, hashOverflowList);
		if (entry->bd == bd) {
			printk(PRINTK_INFO "LA BARRIERA CON BD: %d\n", bd);
			_to_StringTags(entry->tagsHead);
			//printf("\n");
		}
	}
	return EXECUTION_OK;
}

/*
 * Inner Function
 * Given a key, it returns the hashed value, which is the displacement in the hash table
 * @key: barrier key
 */
int _get_hash_from_key(int key) {
	int hash = key % BARRIER_BUCKETS_NUM;
	if (hash < 0 || hash >= BARRIER_BUCKETS_NUM) {
		printk(
				PRINTK_ERR "ERROR: Retried hash %d from key %d is out of range\n",
				hash, key);
		return ERROR_WRONGINPUTDATA;
	}
	return hash;
}

/*
 * Inner Function
 * Given a hash, it returns a new barrier descriptor
 * @hash: barrier hash
 */
int _getBD(int hash) {
	return hash + (sn++ << HASH_SHIFT);
}

/*
 * Inner Function
 * Given a barrier descriptor, it return the hash, which is the displacement in the hash table
 * @bd: barrier descriptor
 */
int _get_hash_from_bd(int bd) {
	int hash = bd & HASH_MASK;
	if (hash < 0 || hash >= BARRIER_BUCKETS_NUM) {
		printk(PRINTK_ERR "ERROR: Retried hash %d from bd %d is out of range\n",
				hash, bd);
		return ERROR_WRONGINPUTDATA;
	}
	return hash;
}

/*
 * Inner Function
 * Given a key, a barrier descriptor and the flags, it creates a new barrier and adds it to the
 * corresponding overflow has list
 * @key: the barrier key
 * @bd: the barrier descriptor
 * @flags: the barrier installation flags
 */
int _create_barrier(int key, int bd, int flags) {
	int hash;
	list * overflow_list_head;

	barrierNode * newBarrierNode = (barrierNode*) kmalloc(sizeof(barrierNode),
			KMALLOCFLAGS);
	newBarrierNode->key = key;
	newBarrierNode->flags = flags;
	newBarrierNode->bd = bd;
	tag_createTagHead(&newBarrierNode->tagsHead);
	hash = _get_hash_from_key(key);


	overflow_list_head = &barrierHashTable[hash];

	list_add(&newBarrierNode->hashOverflowList, overflow_list_head);

	return EXECUTION_OK;
}

/*
 * Inner Function
 * Given a barrier descriptor, if the matching barrier exists, a pointer to its barrier node is returned
 * @bd: barrier descriptor
 * OUT:
 * @barrierNodePointer: the address where the pointer to the retrieved barrier node will be stored
 */
int _lookup_barrier(int bd, int hash, barrierNode** barrierNodePointer) {
	list* hash_overflow_head = &barrierHashTable[hash];
	list* ptr;
	*barrierNodePointer = NULL;
	list_for_each(ptr,hash_overflow_head)
	{
		*barrierNodePointer = list_entry(ptr, barrierNode, hashOverflowList);
		if ((*barrierNodePointer)->bd == bd) {
			return EXECUTION_OK;
		}
	}
	printk(PRINTK_ERR "ERROR: Barrier %d not found\n", bd);
	return ERROR_NOSUCHBARRIER;
}
