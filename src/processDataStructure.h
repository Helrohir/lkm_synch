/*
 * processDataStructure.h
 *
 *  Created on: Oct 19, 2012
 *      Authors: Nicolo Rivetti, Eleonora Calore
 */

#ifndef PROCESSDATASTRUCTURE_H_
#define PROCESSDATASTRUCTURE_H_

#define ERROR_NOSUCHPROCESS -301

int process_createProcessHead(void ** pointer);

int process_addProcess(void* processListHead, void* processData);
int process_removeProcess(void * processHead, int processNodePointer);
int process_removeAllProcess(void* processListHead);
int remove_processHead(void* processHead);
int process_isEmptyProcess(void* processListHead);

int _to_String_Processes(void* processListHead);

#endif /* PROCESSDATASTRUCTURE_H_ */
