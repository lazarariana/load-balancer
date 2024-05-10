Copyright 2023 <Lazar-Andrei Ariana-Maria 312CA>

# Load-balancer

## 1. Overview componente
```
	Sistemul este organizat sub forma unei ierarhii cu 3 componente:
Client, Load Balancer si o lista de servere organizata sub forma unui
array alocat dinamic, sortat crescator.
```
## 2. Operatii permise
```
	Client suporta 2 operatii, stocarea unui produs prin asocierea valorii
sale unei chei si returnarea valorii stocate la un anumit ID.
	Load Balancer dispune de 4 operatii: stocarea unui produs pe unul
dintre servere, extragerea valorii asociate unui key de pe un server,
adaugarea, respectiv stergerea, unui server din sistem. Pentru a pastra
elementele stocate crescator, obiectele trebuie rebalansate la fiecare
insertie/stergere a unui nou server in sistem. Un server este identificat prin
ID-ul si hash-ul calculate, iar memoria sa este organizata in structura de
hashtable.
	Server poate realiza 3 operatii: stocarea unui obiect in memorie,
returnarea valorii asociate unui key sau stergerea unui obiect din memorie.
```
## 3. Implementare
```
	Organizam serverele utilizand consistent hashing, pentru a evita
supraincarcarea unui server. Un obiect este plasat in hashtable atunci cand
este identificat
primul server al carui hash este mai mare decat al sau. De aceea este necesara
rebalansarea obiectelor atunci cand numarul de servere este modificat pentru a
pastra ordinea crescatoare a elementelor.
	Fiecare server dispune de 2 replici adaugate pe hashring dupa formula
eticheta = replica_id * 10^5  + server_id;. Atunci cand este eliminat un
server, se elimina si replicile sale.
```

## 4. Structuri definite

### load_balancer
```
	Contine hashring-ul reprezentat de array-ul alocat dinamic, cat si
campul size ce stocheaza lungimea vectorului la fiecare pas, redimensionarea
realizandu-se element cu element.
```

### server_memory
```
	Contine hashtable-ul ce stocheaza obiectele in memoria serverului.
```

### server
```
	Deoarece in orice operatie este efectuata in fucntie de ID-ul si
hash-ul serverelor, utilizam 2 campuri pentru a stoca datele necesare
identificarii eficiente a unui server. De asemenea, orice server are asociata o
zona de memorie organizata de tip server_memory.
```

## 5.Functionare load-balancer

### 1. add_server
```
	In cazul in care exista cel putin un element in server inainte de
inserare efectuam operatie de rebalansare. Sunt parsate toate obiectele din
fiecare server si mutate daca inserarea unui nou server anuleaza proprietatea
serverului curent de a avea primul hash din array mai mare decat hash-ul
fiecarui obiect.	
```

### 2. remove_server
```
	Se salveaza obiectele stocate in memoria serverului target intr-o
variabila auxiliara, astfel incat dupa eliminarea sa, sa fie mutate pe serverul
urmator din hashring	
```

### 3. store
```
	Se adauga o noua intrare in hashtable-ul din memoria serverului target.
```

### 4. retrive
```
	Se returneza valoarea stocata la cheia respectiva sau NULL daca nu
exista.
```

### 5. free
```
	Se elibereaza memoria fiecarui server si ulterior servele si
hashring-ul
```
