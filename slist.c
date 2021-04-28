#include "slist.h"

#include <stdlib.h>
#include <assert.h>

struct SlistNode{
	struct SlistNode *next;
	void *data;
};

struct Slist {
	struct SlistNode *head;
	struct SlistNode *tail;
	size_t count;
	
	SlistDataCmp  *data_cmp;
	SlistDataEqu  *data_equ;
	SlistDataCopy *data_copy;
	SlistDataFree *data_free;
};

static SlistNode *slist_node_create(void *data);
static bool slist_node_is_exist(Slist *list, SlistNode *node);
static void slist_add_node_first_internal(Slist *list, SlistNode *node);
static void slist_add_node_last_internal (Slist *list, SlistNode *node);

// Slist new 
Slist *slist_create()
{
	return slist_create_full(NULL, NULL, NULL, NULL);
}

Slist *slist_create_full(SlistDataCmp *data_cmp, SlistDataEqu *data_equ, SlistDataCopy *data_copy, SlistDataFree *data_free)
{
	Slist *list = NULL;
	
	list = (Slist *)malloc(sizeof(Slist));
	if (list == NULL) return NULL;
	
	assert(list != NULL);
	
	list->head = (SlistNode *)malloc(sizeof(SlistNode));
	if (list->head == NULL) {
		free(list);
		return NULL;
	}
	
	assert(list->head != NULL); 
	
	list->head->data = NULL;
	list->head->next = NULL;
	
	list->tail = NULL;
	list->count = 0;
	
	list->data_cmp  = data_cmp;
	list->data_equ  = data_equ;
	list->data_copy = data_copy;
	list->data_free = data_free;
	
	return list;
}

// Slist free 
void slist_destroy(Slist *list)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(list->count == 0);
	
	free(list->head);
	free(list);
	
	return;
}

void slist_destroy_deep(Slist *list)
{
	assert(list != NULL);
	assert(list->head != NULL);
	
	slist_clear_deep(list);
	
	slist_destroy(list);
	
	return;
}

// Slist copy 
Slist *slist_copy(Slist *list)
{
	Slist *new_list = NULL;
	SlistNode *p = NULL, *new_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	new_list = slist_create_full(list->data_cmp, 
							     list->data_equ, 
							     list->data_copy, 
							     list->data_free);
	if (new_list == NULL) return NULL;
	
	assert(new_list != NULL);
	assert(new_list->head != NULL);
	
	p = list->head->next;
	while (p) {
		new_node = (SlistNode *)malloc(sizeof(SlistNode));
		if (new_node == NULL) {
			slist_destroy_deep(new_list);
			return NULL;
		}
		
		assert(new_node != NULL);
		
		new_node->data = p->data;
		new_node->next = NULL; 
		
		slist_add_node_last_internal(new_list, new_node);
		
		p = p->next;
	}
	
	assert(new_list->count == list->count);
	
	return new_list;
}
 
Slist *slist_copy_deep(Slist *list)
{
	Slist *new_list = NULL;
	SlistNode *p = NULL, *new_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	new_list = slist_create_full(list->data_cmp, 
							     list->data_equ, 
							     list->data_copy, 
							     list->data_free);
	if (new_list == NULL) return NULL;
	
	assert(new_list != NULL);
	assert(new_list->head != NULL);
	
	p = list->head->next;
	while (p) {
		new_node = (SlistNode *)malloc(sizeof(SlistNode));
		if (new_node == NULL) {
			slist_destroy_deep(new_list);
			return NULL;
		}
		
		assert(new_node != NULL);
		
		new_node->data = list->data_copy(p->data);
		new_node->next = NULL; 
		
		slist_add_node_last_internal(new_list, new_node);
		
		p = p->next;
	}
	
	assert(new_list->count == list->count);
	
	return new_list;
}

// Slist clear 
void slist_clear(Slist *list);
void slist_clear_deep(Slist *list)
{
	SlistNode *node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	while (list->head->next) {
		node = list->head->next;
		list->head->next = node->next;
		list->data_free(node);
		list->count--;
	}
	
	assert(list->count == 0);
	
	return;
}


size_t slist_count(Slist *list)
{
	assert(list != NULL);
	assert(list->head != NULL);
	
	return list->count;
}

bool slist_isempty(struct Slist *list)
{
	assert(list != NULL);
	assert(list->head != NULL);
	
	return list->count == 0;
}


// SlistNode free
void slist_node_free(struct SlistNode *node)
{
	assert(node != NULL);
	
	free(node);
	
	return;
}

static SlistNode *slist_node_create(void *data)
{
	SlistNode *node = NULL;
	
	node = (SlistNode *)malloc(sizeof(SlistNode));
	if (node == NULL) return NULL;
	
	assert(node != NULL);
	
	node->data = data;
	node->next = NULL;
	
	return node;
}

// add_data --- !!!
int slist_add_data_first(Slist *list, void *data)  // prepend  O(1) 
{
	SlistNode *new_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	new_node = slist_node_create(data);
	if (new_node == NULL) return -1;
	
	assert(new_node != NULL);
	
	slist_add_node_first_internal(list, new_node);
	
	return 0;
}

int slist_add_data_last(Slist *list, void *data)   // append   O(1) or O(n)
{
	SlistNode *new_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	new_node = slist_node_create(data);
	if (new_node == NULL) return -1;
	
	assert(new_node != NULL);
	
	slist_add_node_last_internal(list, new_node);
	
	return 0;
}

int slist_add_data_index(Slist *list, size_t index, void *data) //O(n)
{
	SlistNode *new_node = NULL, *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	if (index > list->count) return -1;
	
	assert(index <= list->count);
	
	new_node = slist_node_create(data);
	if (new_node == NULL) return -2;
	
	assert(new_node != NULL);
	
	for(p = list->head; index > 0; index--, p = p->next);
	
	new_node->next = p->next;
	p->next = new_node;
	list->count++;
	
	if (list->tail->next) /* maintain tail pointer */
		list->tail = list->tail->next;
		
	assert(list->tail->next == NULL);
	
	return 0;
}

int slist_add_data_prev_node(Slist *list, SlistNode *anchor, void *data)
{
	SlistNode *new_node = NULL, *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	assert(anchor != NULL);
	
	new_node = slist_node_create(data);
	if (new_node == NULL) return -1;
	
	assert(new_node != NULL);
	
	p = list->head;
	while (p->next) {
		if (p->next == anchor) {
			new_node->next = p->next;
			p->next = new_node;
			list->count++;
			
			return 0;
		}
	}
	
	return -2;
}

int slist_add_data_next_node_safe  (Slist *list, SlistNode *anchor, void *data) // O(n)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(anchor != NULL);
	
	if (!slist_node_is_exist(list, anchor)) return -1;
	
	if (slist_add_data_next_node_unsafe(list, anchor, data) == -1) return -2;
	
	return 0;
}

int slist_add_data_next_node_unsafe(Slist *list, SlistNode *anchor, void *data) // O(1)
{
	SlistNode *new_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	assert(anchor != NULL);
	
	new_node = slist_node_create(data);
	if (new_node == NULL) return -1;
	
	assert(new_node != NULL);
	
	new_node->next = anchor->next;
	anchor->next = new_node;
	list->count++;
	
	if (list->tail->next) /* maintain tail pointer */
		list->tail = list->tail->next;
	
	assert(list->tail->next == NULL);
	
	return 0;
}

int slist_add_data_sorted(struct Slist *list, void *data); // O(n)

// add_node

//一般和remove_one_by_node配合使用，这个方法摘除节点，然后插入。
//如果没有针对节点的插入，remove摘除方法没有含义了。

static bool slist_node_is_exist(Slist *list, SlistNode *node)
{
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	assert(node != NULL);
	
	p = list->head->next;
	while (p) {
		if (p == node) return true;
		p = p->next;
	}
	
	return false;
}

static void slist_add_node_first_internal(Slist *list, SlistNode *node)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(node != NULL);
	
	node->next = list->head->next;
	list->head->next = node;
	list->count++;
	
	if (list->tail == NULL) /* maintain tail pointer */
		list->tail = list->head->next;
	
	assert(list->tail->next == NULL);
	
	return;
}

static void slist_add_node_last_internal(Slist *list, SlistNode *node)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(node != NULL);
	
	if(list->tail == NULL) {
		slist_add_node_first_internal(list, node);
	} else {
		node->next = NULL;
		list->tail->next = node;
		list->count++;
		
		list->tail = list->tail->next;
	}
	
	assert(list->tail->next == NULL);
	
	return;
}

//前提条件是node不在链表中。
int slist_add_node_first(Slist *list, SlistNode *node)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(node != NULL);
	
	if (slist_node_is_exist(list, node)) return -1;
	
	slist_add_node_first_internal(list, node);
	
	return 0;
}

//前提条件是node不在链表中。
int slist_add_node_last(Slist *list, SlistNode *node)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(node != NULL);
	
	if (slist_node_is_exist(list, node)) return -1;
	
	slist_add_node_last_internal(list, node);
	
	return 0;
}

int slist_add_node_prev_node(Slist *list, SlistNode *anchor, SlistNode *node)
{
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	assert(anchor != NULL);
	assert(node != NULL);
	
	if (slist_node_is_exist(list, node)) return -1;
	
	p = list->head;
	while (p->next) {
		if (p->next == anchor) {
			node->next = p->next;
			p->next = node;
			list->count++;
			return 0;
		}
	}
	
	return -1;	 
}

//获取anchor，一般使用get方法获取，保证anchor在链表中。
int slist_add_node_next_node(Slist *list,  SlistNode *anchor,  SlistNode *node)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(anchor != NULL);
	assert(node != NULL);
	
	if (!slist_node_is_exist(list, anchor)) return -1;
	
	if (slist_add_node_next_node_unsafe(list, anchor, node) == -1) return -2;
	
	return 0;
}

int slist_add_node_next_node_unsafe(Slist *list,  SlistNode *anchor,  SlistNode *node)
{
	assert(list != NULL);
	assert(list->head != NULL);
	assert(anchor != NULL);
	assert(node != NULL);
	
	if (slist_node_is_exist(list, node)) return -1;
	
	node->next = anchor->next;
	anchor->next = node;
	list->count++;
	
	if (list->tail->next) /* maintain tail pointer */
		list->tail = list->tail->next;
	
	assert(list->tail->next == NULL);
	
	return 0;
}

int slist_add_node_sorted(Slist *list, SlistNode *node);


// remove --- !!! -- O(n)
int remove_one_by_data(struct Slist *list, void *data)
{
	SlistNode *p = NULL, *free_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	p = list->head;
	while (p->next) {
		if (list->data_equ(p->next->data, data)) {
			free_node = p->next;
			p->next = free_node->next;
			list->count--;
			
			if (list->tail == free_node)  /* maintain tail pointer */
				list->tail = p;
			
			assert(list->tail->next == NULL);
			
			list->data_free(free_node->data); 
			free(free_node);
			return 0;
		}
		p = p->next;
	}
	
	return -1;
}

int remove_all_by_data(struct Slist *list, void *data)
{
	int ret = -1;
	void *copy_data = NULL;
	SlistNode *p = NULL, *free_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	copy_data = list->data_copy(data); /* must copy data !!! */
	
	p = list->head;
	while (p->next) {
		if (list->data_equ(p->next->data, copy_data)) {
			free_node = p->next;
			p->next = free_node->next;
			list->count--;
			
			if (list->tail == free_node)  /* maintain tail pointer */
				list->tail = p;
			
			assert(list->tail->next == NULL);
			
			list->data_free(free_node->data); 
			free(free_node);
			
			ret = 0;
			continue;
		}
		p = p->next;
	}
	list->data_free(copy_data); /* must free copy_data !!! */
	
	return ret;
}

int remove_by_node(struct Slist *list, SlistNode *node)
{
	SlistNode *p = NULL, *free_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	assert(node != NULL);
	
	p = list->head;
	while (p->next) {
		if (p->next == node) {
			free_node = p->next;
			p->next = free_node->next;
			list->count--;
			
			if (list->tail == free_node)  /* maintain tail pointer */
				list->tail = p;
			
			assert(list->tail->next == NULL);
			
			list->data_free(free_node->data);
			free(free_node);
			return 0;
		}
		p = p->next;
	}
	
	return -1;
}

SlistNode *remove_node_by_index(Slist *list, size_t index)
{
	SlistNode *p = NULL, *ret_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	if (index >= list->count) return NULL;
	
	for(p = list->head; index > 0; index--, p = p->next);
	
	ret_node = p->next; 
	p->next = ret_node->next;
	list->count--;
	
	if (list->tail == ret_node)  /* maintain tail pointer */
		list->tail = p;

	assert(list->tail->next == NULL);
	
	return ret_node;
}

void *remove_data_by_index(Slist *list, size_t index)
{
	void *ret_data = NULL;
	SlistNode *p = NULL, *free_node = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	if (index >= list->count) return NULL;
	
	for(p = list->head; index > 0; index--, p = p->next);
	
	free_node = p->next;
	p->next = free_node->next;
	list->count--;
	
	if (list->tail == free_node)  /* maintain tail pointer */
		list->tail = p;

	assert(list->tail->next == NULL);
	
	ret_data = free_node->data;
	free(free_node);
	
	return ret_data;
}

//get
//get_node的方法一般不是用来去data的，是配合remove来摘除节点。
SlistNode *slist_get_node_by_index(Slist *list, size_t index)
{
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	if (index >= list->count) return NULL;
	
	for (p = list->head->next; index > 0; index--, p = p->next);
	
	return p;
}

SlistNode *slist_get_node_by_data(Slist *list, void *data)
{
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	p = list->head->next;
	while (p) {
		if (list->data_equ(p->data, data)) return p;
		p = p->next;
	}
	
	return NULL;
}
 
void *slist_get_data_by_index(Slist *list, size_t index)
{
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	if (index >= list->count) return NULL;
	
	for (p = list->head->next; index > 0; index--, p = p->next);
	
	return p->data;
}

long slist_get_index_by_data(Slist *list, void *data)
{
	long index = 0;
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	p = list->head->next;
	while (p) {
		if (list->data_equ(p->data, data)) return index;
		p = p->next;
		index++;
	}
	
	return -1;
}

long slist_get_index_by_node(Slist *list, SlistNode *node)
{
	long index = 0;
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	p = list->head->next;
	while (p) {
		if (p == node) return index;
		p = p->next;
		index++;
	}
	
	return -1;
}

SlistNode *slist_get_node_custom(Slist *list, SlistDataFind *data_find, void *user_data)
{
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	assert(data_find != NULL);
	
	p = list->head->next;
	while (p) {
		if (data_find(p->data, user_data) == 0) break;
		p = p->next;
	}
	
	return p;
}

// last --- O(n) or O(1) -- ok
SlistNode *slist_last_node(Slist *list)
{
	assert(list != NULL);
	assert(list->head != NULL);
	
	return list->tail;
}

void *slist_last_data(Slist *list)
{
	void *ret_data = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	if (list->tail != NULL) 
		ret_data = list->tail->data;
	
	return ret_data;
}

// first --- O(1) -- ok
SlistNode *slist_first_node(Slist *list)
{
	assert(list != NULL);
	assert(list->head != NULL);
	
	return list->head->next;
}

void *slist_first_data(Slist *list)
{
	void *ret_data = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	if (list->head->next != NULL) 
		ret_data = list->head->next->data;
	
	return ret_data;
}


//advanced method

// reverse --- O(n)
void slist_reverse(struct Slist *list)
{
	SlistNode *p = NULL;
	
	assert(list != NULL);
	assert(list->head != NULL);
	
	list->tail = list->head->next;
	while (list->tail->next) {
		p = list->tail->next;
		list->tail->next = p->next;
		
		p->next = list->head->next;
		list->head->next = p;
	}
	
	return;
}

// sort --- !!!
void slist_sort(struct Slist *list);

//sort merge
void slist_sort_merge(Slist *list1, Slist *list2);

//free list,append list to target.
void slist_concat(Slist *target, Slist *list)
{
	SlistNode *p = NULL;
	
	assert(target != NULL);
	assert(target->head != NULL);
	assert(list != NULL);
	assert(list->head != NULL);
	
	while (list->head->next) {
		p = list->head->next;
		list->head->next = p->next;
		list->count--;
		
		slist_add_node_last(target, p);
	}
	
	assert(list->count == 0);
	
	slist_destroy(list);
	
	return;
}


