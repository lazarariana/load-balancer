/* Copyright 2023 <Lazar-Andrei Ariana-Maria 312CA> */
#ifndef HASHTABLE_H_
#define HASHTABLE_H_
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
	void (*key_val_free_function)(void*);
};

int compare_function_ints(void *a, void *b);
int compare_function_strings(void *a, void *b);
unsigned int hash_function_int(void *a);
unsigned int hash_function_string(void *a);
void key_val_free_function(void *data);
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*));
int ht_has_key(hashtable_t *ht, void *key);
void *ht_get(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht);
unsigned int ht_get_size(hashtable_t *ht);
unsigned int ht_get_hmax(hashtable_t *ht);

#endif /* HASHTABLE_H_ */
