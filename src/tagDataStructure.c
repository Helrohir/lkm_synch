/*
 * tagDataStructure.c
 *
 *  Created on: Oct 17, 2012
 *      Authors: Nicolo Rivetti, Eleonora Calore
 */

#include "Common.h"
#include "tagDataStructure.h"
#include "processDataStructure.h"

struct tag_struct {
	int tag;
	void* processHead; //Processes list head
	list tagList; //List handler
};
typedef struct tag_struct tagNode;

int tag_createTagHead(void ** pointer);

int tag_addProcessOnTag(void* tagHead, int tag, void* processData);
int tag_removeAllProcessesOnTag(void* tagHead, int tag);
int tag_removeAllTags(void* tagHead);
int tag_isEmpty(void* tagHead);

int _to_StringTags(void* tagListHead);

int _getTagNode(list* tagHead, int tag, tagNode** tagEntry);
int _getOrCreateProcessList(list* tagHead, int tag, void** processHead);

/*
 *  Returns a pointer to the head of a new Tag data structure
 *  @pointer: void pointer which will store the address of the data structure
 */
int tag_createTagHead(void ** pointer) {

	// Poniamo pointer a NULL per verificare se l'allocazione è avvenuta con successo
	*((list**) pointer) = NULL;

	// Castiamo void ** pointer a list** pointer, deferenziamo a list* pointer e allochiamo la list head
	*((list**) pointer) = (list*) kmalloc(sizeof(list), KMALLOCFLAGS);

	if (*((list**) pointer) == NULL ) {
		printk(PRINTK_ERR "Cannot allocate tagList\n");
		return ERROR_INITALLOCATIONERROR;
	}

	INIT_LIST_HEAD(*((list**)pointer));

	return EXECUTION_OK;
}

/*
 * Store a process given a pointer to the head of the tag data structure, the process data and a tag
 * @tagHead: void pointer to the head of the tag data structure
 * @tag: the tag on which the process must be stored
 * @processData: void pointer to the process data
 * OUT:
 * @removeProcessData: pointer to the in containing the data to remove the process
 */
int tag_addProcessOnTag(void* tagHead, int tag, void* processData) {
	int ret;
	void* processListHead;
	if (tagHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Node Struct Pointer NULL\n");
		return ERROR_WRONGINPUTDATA;
	}

	if (tag < 0 || tag >= TAG_NUMBER) {
		printk(PRINTK_ERR "ERROR: Tag out of range: %d\n", tag);
		return ERROR_WRONGINPUTDATA;
	}

	ret = EXECUTION_OK;
	ret = _getOrCreateProcessList((list*) tagHead, tag, &processListHead);
	if (ret != EXECUTION_OK) {
		return ret;
	}

	ret = process_addProcess(processListHead, processData);
	if (ret != EXECUTION_OK) {
		return ret;
	}
	return EXECUTION_OK;
}

/*
 * Removes a procoess given a pointer to the tag data structure, a tag and the remove process data
 * of the process
 * @tagHead: void pointer to the head of the tag data structure
 * @tag: the tag on which the process is stored
 * @removeProcessData: data provided when the process requested to be stored
 */
int tag_removeProcessOnTag(void* tagHead, int tag, int removeProcessData) {
	tagNode* tagEntry;
	int ret = EXECUTION_OK;
	ret = _getTagNode((list*) tagHead, tag, &tagEntry);
	if (ret != EXECUTION_OK) {
		return ret;
	}

	ret = process_removeProcess(tagEntry->processHead, removeProcessData);
	if (ret != EXECUTION_OK) {
		return ret;
	}

	return EXECUTION_OK;
}

/*
 * Removes all process stored on pointed tag data structure on the given tag
 * @tagHead: void pointer to the head of the tag data structure
 * @tag: the tag on which the process must be stored
 */
int tag_removeAllProcessesOnTag(void* tagHead, int tag) {
	tagNode* tagEntry;
	int ret;
	if (tagHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Node Struct Pointer NULL");
		return ERROR_WRONGINPUTDATA;
	}

	if (tag < 0 || tag >= TAG_NUMBER) {
		printk(PRINTK_ERR "ERROR: Tag out of range: %d\n", tag);
		return ERROR_WRONGINPUTDATA;
	}

	ret = EXECUTION_OK;
	ret = _getTagNode((list*) tagHead, tag, &tagEntry);
	if (ret != EXECUTION_OK) {
		return ret;
	}

	ret = process_removeAllProcess(tagEntry->processHead);
	if (ret != EXECUTION_OK) {
		return ret;
	}

	return EXECUTION_OK;
}

/*
 * Removes all process stored on pointed tag data structure
 * @tagHead: void pointer to the head of the tag data structure
 */
int tag_removeAllTags(void* tagHead) {
	int ret;
	list* ptr, *next;
	tagNode* entry;
	if (tagHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Node Struct Pointer NULL");
		return ERROR_WRONGINPUTDATA;
	}

	ret = EXECUTION_OK;
	next = (list*) kmalloc(sizeof(next), KMALLOCFLAGS);

	list_for_each_safe(ptr,next,(list*)tagHead)
	{
		entry = list_entry(ptr, tagNode, tagList);
		ret = remove_processHead(entry->processHead);
		if (ret != EXECUTION_OK) {
			return ret;
		}
		list_del(&entry->tagList);
		kfree(entry);
	}
	return ret;
}

/*
 * Given a pointer to the head of the tag data structure, it returns a non zero value if the processList is empty
 * @tagHead: void pointer to the head of the tag data structure
 */
int tag_isEmpty(void* tagHead) {
	if (tagHead == NULL ) {
		printk(PRINTK_ERR "ERROR: Node Struct Pointer NULL");
		return ERROR_WRONGINPUTDATA;
	}
	return list_empty((list*) tagHead);
}

int _to_StringTags(void* tagListHead) {
	list* ptr;
	tagNode* entry = NULL;
	list_for_each(ptr, (list*)tagListHead)
	{
		entry = list_entry(ptr, tagNode, tagList);
		if (entry && entry->processHead) {
			printk(PRINTK_INFO "nel tag %d:\n", entry->tag);
			_to_String_Processes((void*) entry->processHead);
		}
	}
	return EXECUTION_OK;
}

/*
 * Inner Function
 * Given the pointer to the head of the list of tagNodes and a tag, returns the pointer to tagNode
 * If the tagNode for the given tag does not exists, an error is returned
 * @tagHead: void pointer to the head of the tag data structure
 * @tag: the tag to look for
 * OUT:
 * tagEntry: a pointer to the pointer in which the retrieved tagNode address is stored
 */
int _getTagNode(list* tagHead, int tag, tagNode** tagEntry) {

	list* ptr;
	list_for_each(ptr, (list*) tagHead)
	{
		*tagEntry = list_entry(ptr, tagNode, tagList);
		if ((*tagEntry)->tag == tag) {
			return EXECUTION_OK;
		}
	}
	return ERROR_NOSUCHTAG;
}

/*
 * Inner Function
 * Given the pointer to the head of the list of tagNodes and a tag, returns the pointer to the head
 * of the process data structure. If the tagNode for the given tag does not exists, a new tagNode
 * is added to the tagList.
 * @tagHead: void pointer to the head of the tag data structure
 * @tag: the tag to look for
 * OUT:
 * processHead: a void pointer to the pointer in which the retrieved processHead address is stored
 */
inline int _getOrCreateProcessList(list* tagHead, int tag, void** processHead) {
	list* ptr;
	tagNode* entry;
	list_for_each(ptr, tagHead)
	{
		entry = list_entry(ptr, tagNode, tagList);
		if (entry->tag == tag) {
			*processHead = entry->processHead;
			return EXECUTION_OK;
		}
	}

	entry = (tagNode*) kmalloc(sizeof(tagNode), KMALLOCFLAGS);
	entry->tag = tag;
	process_createProcessHead(&entry->processHead);

	list_add(&entry->tagList, tagHead);

	*processHead = entry->processHead;

	return EXECUTION_OK;
}

