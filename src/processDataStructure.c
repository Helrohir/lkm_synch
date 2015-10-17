/*
 * processDataStructure.c
 *
 *  Created on: Oct 19, 2012
 *      Author: helrohir
 */

#include "Common.h"
#include "processDataStructure.h"
#include "wakeupProcess.h"

struct process_struct {
	void* processPointer; //Dati processo
	list processList; //Nodo lista.
};
typedef struct process_struct processNode;

int process_addProcess(void* processHead, void* processData);
int process_removeProcess(void * processHead, int removeProcessData);
int process_removeAllProcess(void* processHead);
int process_isEmptyProcess(void* processHead);

int _to_String_Processes(void* processListHead);

int _removeAllProcess(list* processHead);

/*
 *  Returns a pointer to the head of a new Process data structure
 *  @pointer: void pointer which will store the address of the data structure
 */
int process_createProcessHead(void ** pointer) {
	;
	// Poniamo pointer a NULL per verificare se l'allocazione è avvenuta con successo
	*((list**) pointer) = NULL;

	// Castiamo void ** pointer a list** pointer, deferenziamo a list* pointer e allochiamo la list head
	*((list**) pointer) = (list*) kmalloc(sizeof(list), KMALLOCFLAGS);

	if (*((list**) pointer) == NULL ) {
		printk(PRINTK_ERR "ERROR: Cannot allocate process Head\n");
		return ERROR_INITALLOCATIONERROR;
	}

	INIT_LIST_HEAD(*((list**)pointer));

	return EXECUTION_OK;
}

/*
 * Given a pointer to the head of the list of processNodes and pointer to the process it adds a new processNode,
 * containing the process pointer, the address of the node is returned in the int pointed by the remove process data
 * @processHead: a void pointer to the head of the processes Nodes list
 * @processData: a void pointer to the processData
 * OUT:
 * @removeProcessData: data required to remove the specific process
 */
int process_addProcess(void* processHead, void* processData) {
	processNode* newProcessNode;
	if (processHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Process List Head Pointer NULL\n");
		return ERROR_WRONGINPUTDATA;
	}

	if (processData == NULL ) {
		printk(PRINTK_ERR "ERROR: Process Pointer NULL\n");
		return ERROR_WRONGINPUTDATA;
	}

	// Create and setup the new node
	newProcessNode = (processNode*) kmalloc(sizeof(processNode), KMALLOCFLAGS);

	if (newProcessNode == NULL ) {
		printk(PRINTK_ERR "Cannot allocate new Process Node\n");
		return ERROR_INITALLOCATIONERROR;
	}

	newProcessNode->processPointer = processData;
	list_add(&newProcessNode->processList, processHead);

	//printf("ADDED PROCESS %d\n", (int) processData);
	return EXECUTION_OK;
}

/*
 * Given the remove process data, it removes the related process from the data structure
 * @removeProcessData: data required to remove the specific process
 */
int process_removeProcess(void * processHead, int removeProcessData) {
	processNode* processNodeEntry;
	list *next = (list*) kmalloc(sizeof(list), KMALLOCFLAGS);
	list* ptr;
	list_for_each_safe(ptr, next, (list*)processHead)
	{
		processNodeEntry = list_entry(ptr,processNode, processList);
		//printf("REMOVING PROCESS %d\n", (int) processNodeEntry->processPointer);
		if (processNodeEntry->processPointer == (void*) removeProcessData) {
			list_del(&processNodeEntry->processList);
			kfree(processNodeEntry);
			return EXECUTION_OK;
		}

	}

	return ERROR_NOSUCHPROCESS;
}

/*
 * Given a pointer to the head of the processList it removes all processes
 * @processHead: a void pointer to the head of the processes Nodes list
 */
int process_removeAllProcess(void* processHead) {
	if (processHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Process Head Pointer NULL");
		return ERROR_WRONGINPUTDATA;
	}

	return _removeAllProcess((list*) processHead);
}

/*
 * Given a pointer to the head of the processList it removes all processes and also the process head itself
 * @processHead: a void pointer to the head of the processes Nodes list
 */
int remove_processHead(void* processHead) {
	int ret;
	if (processHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Process Head Pointer NULL");
		return ERROR_WRONGINPUTDATA;
	}

	ret = _removeAllProcess((list*) processHead);
	if(ret != EXECUTION_OK){
		return ret;
	}

	kfree(processHead);

	return EXECUTION_OK;
}

/*
 * Given a pointer to the head of the processList it returns a non zero value if the processList is empty
 * @processHead: a void pointer to the head of the processes Nodes list
 */
int process_isEmptyProcess(void* processHead) {
	if (processHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Process Head Pointer NULL");
		return ERROR_WRONGINPUTDATA;
	}

	return list_empty((list*) processHead);
}

int _to_String_Processes(void* processListHead){
	list* ptr;
	processNode* entry = NULL;
	list_for_each(ptr, (list*)processListHead){
		entry = list_entry(ptr, processNode, processList);
		if(entry && entry->processPointer){
			printk(PRINTK_INFO "\t il processo: %p\n", entry->processPointer);
		}
	}
	return EXECUTION_OK;
}

/*
 * Inner Funciton
 * Given a pointer to the head of the processList it removes all processes
 * @processHead: a list pointer to the head of the processes Nodes list
 */
int _removeAllProcess(list* processHead) {
	processNode* processNodeEntry;
	list *next = (list*) kmalloc(sizeof(list), KMALLOCFLAGS);
	list* ptr;
	list_for_each_safe(ptr, next, processHead)
	{
		processNodeEntry = list_entry(ptr,processNode, processList);
		wakeUpProcess(processNodeEntry->processPointer);
		//printf("REMOVING PROCESS %d\n", (int) processNodeEntry->processPointer);
		list_del(&processNodeEntry->processList);
		kfree(processNodeEntry);
	}
	return EXECUTION_OK;
}
