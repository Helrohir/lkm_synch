/*
 * barrrieDataStructure.h
 *
 *  Created on: Oct 17, 2012
 *      Authors: Nicolo Rivetti, Eleonora Calore
 */

#ifndef BARRIERDATASTRUCTURE_H_
#define BARRIERDATASTRUCTURE_H_

#define ERROR_BARRIERALREADYUSED -101
#define ERROR_EXCLUSIVEBARRIER -103
#define ERROR_NOSUCHBARRIER -104

int barrier_Init(void);
int barrier_Cleanup(void);

int barrier_getOrCreateBarrier(int key, int flags, int *bd);
int barrier_addProcessOnTag(int bd, int tag, void* processData);
int barrier_removeProcessOnTag(int bd, int tag, int removeProcessData);
int barrier_removeAllProcessesOnTag(int bd, int tag);
int barrier_removeBarrier(int bd);

int _to_String(int bd);



#endif /* KEYDATASTRUCTURE_H_ */
