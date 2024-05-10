/* Copyright 2023 <Lazar-Andrei Ariana-Maria 312CA> */
#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

#include "server.h"
#define MAX_SERVERS 100000
#define MAX_UINT 4294967295
#define REPLICA 100000

struct load_balancer;
typedef struct load_balancer load_balancer;

typedef struct load_balancer
{
	server *hashring;
	size_t size;
} load_balancer;

unsigned int hash_function_servers(void *a);
unsigned int hash_function_key(void *a);

/**
*	se determina index-ul din  hashring al primului server cu hash mai mare
*	decat hash-ul elementului de inserat/sters din vectorul sortat. Utilizam
*	cautarea binara datorita faptului ca vectorul este sortat si usurintei de a
*	accesa elementele in fucntie de pozitia lor.
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: ID-ul serverului target
*	@arg3: hash-ul cautat in vector
*/
unsigned int det_index(load_balancer *main, int server_id,
							unsigned int hash);

/**
*	se insereaza primul server in hashring
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: ID-ul serverului target
*/
void first_server(load_balancer *main, int server_id);

/**
*	se deplaseaza la dreapta elementele din vectorul realocat cu o pozitie in
*	plus incepand de la indexul target. Pe pozitia ramasa libera, se insereaza
*	noul server, pastrand astfel array-ul sortat crescator.
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: ID-ul serverului target
*	@arg3: indexul identificat din array pe care dorim sa plasam un nou server
*/
void rearrange_elements(load_balancer *main, int server_id, unsigned int index);

/**
*	se adauga replicile servarului dupa formula replica_id *10^5 + server_id
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: ID-ul serverului target
*/
void loader_add_replicas(load_balancer *main, int server_id);

/**
*	pentru a nu pierde obiectele stocate pe server-ul pe care vrem sa il
*	stergem, mutam continutul stocat in hashtable in urmatorul server din
*	array. Sortarea crescatoare a vectorului asigura faptul ca niciun hash
*	nu va fi mai mare decat al server-ului.
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: variabila auxiliara care contine informatia inainte de stergere
*	@arg3: ID-ul serverului target
*/
void move_objects(load_balancer *main, server *aux, int server_id);

/**
*	se elimina si replicile server-ului target, pastrandu-se intr-o variabila
*	auxiliara hashtable-ul din memoria server-ului pentru a nu pierde
*	informatia. Pentru utilizarea eficienta a memoriei, se realoca la fiecare
*	pas vectorul.
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: variabila auxiliara in care pastram informatia inainte de stergere
*	@arg3: ID-ul serverului target
*/
void remove_replicas(load_balancer *main, server *aux, unsigned int index);

/*
	se aloca memoria pentru hashring
*/
load_balancer *init_load_balancer();

/**
*	se elibereaza memoria fiecarui server si ulterior servele si hashring-ul
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*/
void free_load_balancer(load_balancer *main);

/*
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: cheia ce reprezinta intrarea in hashtable
*	@arg3: valoarea asociata cheii
*	@arg4: ID-ul serverului target
*/
void loader_store(load_balancer *main, char *key, char *value, int *server_id);

/*
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: cheia ce reprezinta intrarea in hashtable
*	@arg3: ID-ul serverului target
*/
char *loader_retrieve(load_balancer *main, char *key, int *server_id);

/**
*	in cazul in care exista cel putin un element in server inainte de inserare
*	efectuam operatie de rebalansare. Sunt parsate toate obiectele din fiecare
*	server si mutate daca inserarea unui nou server anuleaza proprietatea
*	serverului curent de a avea primul hash din array mai mare decat hash-ul
*	fiecarui obiect.
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: ID-ul serverului target
*/
void loader_add_server(load_balancer *main, int server_id);

/**
*	se salveaza obiectele stocate in memoria serverului target intr-o variabila
*	auxiliara, astfel incat dupa eliminarea sa, sa fie mutate pe serverul
*	urmator din hashring
*	@arg1: load_balancer-ul ce distribuie serverele pe hashring
*	@arg2: ID-ul serverului target
*/
void loader_remove_server(load_balancer *main, int server_id);

#endif /* LOAD_BALANCER_H_ */
