/* Copyright 2023 <Lazar-Andrei Ariana-Maria 312CA> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "list.h"
#include "hashtable.h"

#define MAX_STRING_SIZE	256
#define HMAX 10


int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

void key_val_free_function(void *data)
{
	info* data_info = data;
	free(data_info->key);
	free(data_info->value);
}

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*))
{
	hashtable_t* ht = malloc(sizeof(*ht));
    DIE(!ht, "Not allocated\n");
	ht->buckets = malloc(hmax * sizeof(*ht->buckets));
	DIE(!ht->buckets, "Not allocated\n");
	ht->compare_function = compare_function;
	ht->hash_function = hash_function;
	ht->key_val_free_function = key_val_free_function;
	ht->size = 0;
	ht->hmax = hmax;
	for (unsigned int i = 0; i < hmax; i++) {
		ht->buckets[i] = ll_create(sizeof(info));
		DIE(!ht->buckets[i], "Not allocated");
	}
	return ht;
}

int ht_has_key(hashtable_t *ht, void *key)
{
	unsigned int hash = ht->hash_function(key) % ht->hmax;
	ll_node_t* node;

	node = ht->buckets[hash]->head;
	while (node != NULL) {
		info *node_info = node->data;
		if (ht->compare_function(node_info->key, key) == 0)
			return 1;
		node = node->next;
	}

	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	unsigned int hash = ht->hash_function(key) % ht->hmax;
	ll_node_t* node;
	node = ht->buckets[hash]->head;
	while (node != NULL) {
		info* node_info = node->data;
		if (ht->compare_function(node_info->key, key) == 0)
			return node_info->value;
		node = node->next;
	}

	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int hash = ht->hash_function(key) % ht->hmax;
	info elem;
	ll_node_t* node = ht->buckets[hash]->head;
    ll_node_t* found = NULL;

	for (unsigned int  i = 0; i < ht->buckets[hash]->size; ++i) {
		if (ht->compare_function((void*)key, (void*)((info *)node->data)->key) == 0) {
			found = node;
		}
		node = node->next;
	}

	if (!found) {
		elem.key = malloc(key_size);
		DIE(!elem.key, "Not allocated");
		memcpy(elem.key, key, key_size);

		elem.value = malloc(value_size);
		DIE(!elem.value, "Not allocated");
		memcpy(elem.value, value, value_size);

		ll_add_nth_node(ht->buckets[hash], 0, &elem);
	} else {
		memcpy(((info *)found->data)->value, value, value_size);
	}

	ht->size++;
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned int cnt = ht->hash_function(key) % ht->hmax;
    ll_node_t* node = ht->buckets[cnt]->head;
    ll_node_t* found;
    unsigned int index;

	for (unsigned int  i = 0; i < ht->buckets[cnt]->size; ++i) {
		if (ht->compare_function(key, ((info *)node->data)->key) == 0) {
			found = node;
            index  = i;
		}
		node = node->next;
	}

	if (found) {
		ll_node_t* rm = ll_remove_nth_node(ht->buckets[cnt], index);
		free(((info *)rm->data)->value);
		free(((info *)rm->data)->key);
		free(rm->data);
		free(rm);
		ht->size--;
	}
}

void ht_free(hashtable_t *ht)
{
	ll_node_t *current;
	for (unsigned int i = 0; i < ht->hmax; i++) {
		if (ht->buckets[i]->head != NULL) {
			current = ht->buckets[i]->head;
			while (current != NULL) {
				free(((info *)current->data)->value);
				free(((info *)current->data)->key);
				current = current->next;
			}
		}
		ll_free(&ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}

