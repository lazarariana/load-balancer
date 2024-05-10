/* Copyright 2023 <Lazar-Andrei Ariana-Maria 312CA> */
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "load_balancer.h"
#include "utils.h"

unsigned int hash_function_servers(void *a)
{
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a)
{
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer *init_load_balancer()
{
    load_balancer *ld_balancer = NULL;

	ld_balancer = malloc(sizeof(load_balancer));
	DIE(!ld_balancer, "Not allocated");

	ld_balancer->hashring = NULL;
	ld_balancer->size = 0;

	return ld_balancer;
}

unsigned int det_index(load_balancer *main, int server_id,
					   unsigned int hash)
{
	unsigned int left = 0, right = main->size, mid, index = MAX_UINT;

    while (left < right) {
        mid = (left + right) / 2;
		if (hash <= main->hashring[mid].hash) {
			right = mid;
		} else {
			left = ++mid;
		}
    }
	index = left;
	while (index < main->size && hash == main->hashring[index].hash
		   && main->hashring[index].id <= server_id)
		index++;


	return index;
}

void first_server(load_balancer *main, int server_id)
{
	unsigned int hash = 0;
	hash = hash_function_servers(&server_id);
	main->hashring = malloc(sizeof(server));
	DIE(!main->hashring, "Not allocated\n");
	main->hashring[0].hash = hash;
	main->hashring[0].id = server_id;
	main->hashring[0].server_memory = init_server_memory();
	main->size = 1;
}

void rearrange_elements(load_balancer *main, int server_id, unsigned int index)
{
	unsigned int hash = 0;
	hash = hash_function_servers(&server_id);
	main->hashring = realloc(main->hashring, (main->size + 1) * sizeof(server));
	DIE(!main->hashring, "Not allocated\n");
	for (unsigned int i = main->size; i > index; i--)
		main->hashring[i] = main->hashring[i - 1];
	main->hashring[index].hash = hash;
	main->hashring[index].id = server_id;
	main->hashring[index].server_memory = init_server_memory();
	main->size++;
}

void loader_add_server(load_balancer *main, int server_id)
{
	unsigned int index = 0, rebalance_index = 0, hmax = 0;
	unsigned int hash = 0;
	hash = hash_function_servers(&server_id);
	ll_node_t *current = NULL;

	if (main->hashring && main->size > 0) {
		index = det_index(main, server_id, hash);
		rebalance_index = (index + 1) % (main->size + 1);
		rearrange_elements(main, server_id, index);
		hmax = main->hashring[rebalance_index].server_memory->objects->hmax;
		for (unsigned int i = 0; i < hmax; i++) {
			current =
			main->hashring[rebalance_index].server_memory->objects->buckets[i]->head;
			while (current) {
				info current_data = *(info *)current->data;
				char *key = (char *) malloc(strlen(current_data.key) + 1);
				DIE(!key, "Not allocated\n");
				memmove(key, current_data.key, strlen(current_data.key) + 1);
				char *value = (char *) malloc(strlen(current_data.value) + 1);
				if (!value) {
					free(key);
					DIE(!value, "Not allocated\n");
				}
				memmove(value, current_data.value,
						strlen(current_data.value) + 1);
				current = current->next;
				ht_remove_entry(main->hashring[rebalance_index]
								.server_memory->objects, key);
				loader_store(main, key, value, &server_id);
				free(key);
				free(value);
			}
		}
	} else {
		first_server(main, server_id);
	}
}

void loader_add_replicas(load_balancer *main, int server_id)
{
	unsigned int replica_id_1, replica_id_2;

	replica_id_1 = REPLICA + server_id;
	replica_id_2 = 2 * REPLICA + server_id;

	loader_add_server(main, replica_id_1);
	loader_add_server(main, replica_id_2);
}

void move_objects(load_balancer *main, server *aux, int server_id)
{
	ll_node_t *current = NULL;
	info current_info;

	for (unsigned int i = 0; i < aux->server_memory->objects->hmax; i++) {
		current = aux->server_memory->objects->buckets[i]->head;
		while (current) {
			current_info = *(info *)current->data;
			loader_store(main, current_info.key, current_info.value, &server_id);
			current = current->next;
		}
	}
}

void remove_replicas(load_balancer *main, server *aux, unsigned int index)
{
	if (index) {
		*aux = main->hashring[index - 1];
		for (unsigned int i = index - 1; i < main->size - 1; i++)
			main->hashring[i] = main->hashring[i + 1];
	} else {
		*aux = main->hashring[main->size - 1];
	}

	main->hashring = realloc(main->hashring, (main->size - 1) * sizeof(server));
}

void loader_remove_server(load_balancer *main, int server_id)
{
	unsigned int index = 0, hash = 0;
	server aux;

	if (!main->hashring || !main->size)
		return;

	hash = hash_function_servers(&server_id);
	index = det_index(main, server_id, hash);

	for (unsigned int i = 0; i <= 2; i++) {
		unsigned int var = i * MAX_SERVERS + server_id;
		hash = hash_function_servers(&var);
		index = det_index(main, var, hash);
		remove_replicas(main, &aux, index);
		main->size--;
		move_objects(main, &aux, i * MAX_SERVERS + server_id);
		free_server_memory(aux.server_memory);
	}
}

void loader_store(load_balancer* main, char* key, char* value, int *server_id)
{
	unsigned int hash = 0, index = 0;
	hash = hash_function_key(key);

	if (!main->hashring || !main->size)
		return;

	index = det_index(main, -1, hash);
	index %= main->size;
	server_store(main->hashring[index].server_memory, key, value);
	*server_id = main->hashring[index].id % REPLICA;
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id)
{
	unsigned int hash = 0, index = 0;
	char *value = NULL;

	if (!main->hashring || !main->size)
		return NULL;

	hash = hash_function_key(key);

	index = det_index(main, -1, hash);
	if (index >= main->size)
		index = 0;

	*server_id = main->hashring[index].id % REPLICA;
	value = server_retrieve(main->hashring[index].server_memory, key);

	return value;
}

void free_load_balancer(load_balancer *main)
{
	for (unsigned int i = 0; i < main->size; i++)
		free_server_memory(main->hashring[i].server_memory);

	free(main->hashring);
	free(main);
}
