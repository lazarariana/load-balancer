/* Copyright 2023 <Lazar-Andrei Ariana-Maria 312CA> */
#ifndef SERVER_H_
#define SERVER_H_

#define KLEN 128
#define VALLEN 65536
#define HMAX 128
#include "hashtable.h"

struct server_memory;
typedef struct server_memory server_memory;

typedef struct server_memory {
	hashtable_t *objects;
}server_memory;

typedef struct server
{
	int id;
	unsigned int hash;
	server_memory *server_memory;
} server;


/*
	se aloca spatiu pentru memoria serverului organizata ca hashtable
*/
server_memory *init_server_memory();

/**
*	se elimina o intrare din hashtable-ul din memoria serverului, eliberand
*	corespunzator memoria
*	@arg1: serverul target
*/
void free_server_memory(server_memory *server);

/**
*	se adauga o noua intrare in hashtable-ul din memoria serverului
*	@arg1: serverul target
*	@arg2: cheia ce reprezinta noua intrare in hashtable
*	@arg3: valoarea ce va corespunde cheii
*/
void server_store(server_memory *server, char *key, char *value);

/**
 *	se elimina o pereche cheie-valoare asigurandu-ne ca eliberam corect memoria
 *	@arg1: server target
 *	@arg2: cheia target
 */
void server_remove(server_memory *server, char *key);

/**
 *	returneza valoarea stocata la cheia respectiva.
 *	@arg1: Server target.
 *	@arg2: intrarea cautata din hashtable.
 *	Return: un string ce reprezinta valoarea asociata cheii care se
 *	initializeaza cu NULL pentru a trata cazul nu exista     
 */
char *server_retrieve(server_memory *server, char *key);

#endif /* SERVER_H_ */
