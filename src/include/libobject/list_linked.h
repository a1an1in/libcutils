#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/list.h>
#include <libdata_structure/hash_list.h>
#include <libobject/iterator_hlist.h>

typedef struct Linked_List_s Linked_List;

struct Linked_List_s{
	List list;

	int (*construct)(List *list,char *init_str);
	int (*deconstruct)(List *list);
	int (*set)(List *list, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
    int (*insert)(List *list,void *key,void *value);
    int (*insert_wb)(List *list,void *key,void *value,Iterator *iter);
    int (*search)(List *list,void *key,Iterator *iter);
    int (*del)(List *list,Iterator *iter);
    /*
     *void (*for_each)(List *list,void (*func)(List *list, char *key, void *value));
     */
    void (*for_each)(List *list,void (*func)(Iterator *iter));
    Iterator *(*begin)(List *list);
    Iterator *(*end)(List *list);
    int (*destroy)(List *list);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
    hash_list_t *hlist;
    uint16_t key_size;
    uint16_t bucket_size;
    uint16_t value_size;
};

#endif
