/*
 * tagDataStructure.h
 *
 *  Created on: Oct 17, 2012
 *      Author: helrohir
 */

#ifndef TAGDATASTRUCTURE_H_
#define TAGDATASTRUCTURE_H_

#define ERROR_NOSUCHTAG -201

int tag_createTagHead(void ** pointer);

int tag_addProcessOnTag(void* tagHead, int tag, void* processData);
int tag_removeProcessOnTag(void* tagHead, int tag, int removeProcessData);
int tag_removeAllProcessesOnTag(void* tagHead, int tag);
int tag_removeAllTags(void* tagHead);

int _to_StringTags(void* tagListHead);

#endif /* TAGDATASTRUCTURE_H_ */
