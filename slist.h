#ifndef __SLIST_H__
#define __SLIST_H__

#include <stddef.h>       /* use type size_t */
#include <stdbool.h>      /* use type bool, true and false in C99 */  

typedef struct SlistNode SlistNode;
typedef struct Slist Slist;

typedef int   SlistDataCmp (void *data1, void *data2);
typedef bool  SlistDataEqu (void *data1, void *data2);
typedef void* SlistDataCopy(void *data);
typedef void  SlistDataFree(void *data);

typedef int SlistDataFind(void *data,void *user_data);


// Slist new
Slist *slist_create(void);
Slist *slist_create_full(SlistDataCmp *data_cmp, SlistDataEqu *data_equ, SlistDataCopy *data_copy, SlistDataFree *data_free);

// Slist free
void slist_destroy(Slist *list);  
void slist_destroy_deep(Slist *list);  

// Slist copy
Slist *slist_copy(Slist *list);  
Slist *slist_copy_deep(Slist *list);

// Slist clear
void slist_clear(Slist *list);
void slist_clear_deep(Slist *list);


size_t slist_count(Slist *list);

bool slist_isempty(struct Slist *list);


// SlistNode free
void slist_node_free(struct SlistNode *node);


// add_data --- !!!
int slist_add_data_first(Slist *list, void *data);  // prepend  O(1) 
int slist_add_data_last(Slist *list, void *data);   // append   O(1) or O(n)
int slist_add_data_index(Slist *list, size_t index, void *data); //O(n)                       !!                    

int slist_add_data_prev_node(Slist *list, SlistNode *anchor, void *data);
int slist_add_data_next_node_safe  (Slist *list, SlistNode *anchor, void *data); // O(n)
int slist_add_data_next_node_unsafe(Slist *list, SlistNode *anchor, void *data); // O(1)

int slist_add_data_sorted(struct Slist *list, void *data); // O(n)

// add_node

//一般和remove_one_by_node配合使用，这个方法摘除节点，然后插入。
//如果没有针对节点的插入，remove摘除方法没有含义了。

//前提条件是node不在链表中。
int slist_add_node_first(Slist *list, SlistNode *node);
int slist_add_node_last(Slist *list, SlistNode *node);

int slist_add_node_prev_node(Slist *list, SlistNode *anchor, SlistNode *node);
//获取anchor，一般使用get方法获取，保证anchor在链表中。
int slist_add_node_next_node(Slist *list,  SlistNode *anchor,  SlistNode *node);
int slist_add_node_next_node_unsafe(Slist *list,  SlistNode *anchor,  SlistNode *node);

int slist_add_node_sorted(Slist *list, SlistNode *node);

// remove --- !!! -- O(n)
int remove_one_by_data(struct Slist *list, void *data);

int remove_all_by_data(struct Slist *list, void *data);

int remove_by_node(struct Slist *list, SlistNode *node);

SlistNode *remove_node_by_index(Slist *list, size_t index);

void *remove_data_by_index(Slist *list, size_t index);

//get
//get_node的方法一般不是用来去data的，是配合remove来摘除节点。
SlistNode *slist_get_node_by_index(Slist *list, size_t index);
SlistNode *slist_get_node_by_data(Slist *list, void *data);
 
void *slist_get_data_by_index(Slist *list, size_t index);

long slist_get_index_by_data(Slist *list, void *data);

long slist_get_index_by_node(Slist *list, SlistNode *node);

SlistNode *slist_get_node_custom(Slist *list, SlistDataFind *data_find, void *user_data);

// last --- O(n) or O(1) -- ok
SlistNode *slist_last_node(Slist *list);
void *slist_last_data(Slist *list);

// first --- O(1) -- ok
SlistNode *slist_first_node(Slist *list);
void *slist_first_data(Slist *list);


//advanced method

// reverse --- O(n)
void slist_reverse(struct Slist *list);

// sort --- !!!
void slist_sort(struct Slist *list);

//sort merge
void slist_sort_merge(Slist *list1, Slist *list2);

//free list,append list to target.
void slist_concat(Slist *target, Slist *list);

#endif //__SLIST_H__

