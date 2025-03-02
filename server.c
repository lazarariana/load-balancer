/* Copyright 2023 <Lazar-Andrei Ariana-Maria 312CA> */
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "server.h"
#include "utils.h"

server_memory *init_server_memory()
{
	server_memory *server = NULL;
	server = malloc(sizeof(server_memory));
	DIE(!server, "Not allocated");

	server->objects = ht_create(HMAX, hash_function_string,
										compare_function_strings,
										key_val_free_function);

	return server;
}

void server_store(server_memory *server, char *key, char *value) {
	ht_put(server->objects, key, strlen(key) + 1, value, strlen(value) + 1);
}

char *server_retrieve(server_memory *server, char *key) {
	return ht_get(server->objects, key);
}

void server_remove(server_memory *server, char *key) {
	ht_remove_entry(server->objects, key);
}

void free_server_memory(server_memory *server) {
	ht_free(server->objects);
	free(server);
}
