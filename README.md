# Virtual_Memory_Allocator
First project(SDA)
#MARGHEANU CRISTINA-ANDREEA 313CA 2022-2023
In programul principal citesc comanda ce ne va preciza ce operatie trebuie sa
efectuam. Pentru fiecare comanda introdusa citesc parametrii necesari 
subprogramelor construite (adresele de inceput, dimensiunile).

Rezolvarea problemei foloseste implemnetarea listelor dublu inlantuite si
operatiile specifice ale acestora. Pentru simplificarea parcurgerii am 
introdus in structura listei si un nod *tail, ce va retine caracteristicile 
nodului din coada listei. 

Functiile folosite  ce privesc in mod direct notiunea de lista sunt:
- dll_create , ce aloca memorie pentru lista si ii initializeza componentele
-dll_get_nth_node parcurge fiecare nod in parte atata timp cat acesta exista, 
pana ajunge la indexul cerut(la pozitia cautata din lista), returnand nodul
aflat la acea pozitie. Daca nu exista un nod urmator, lista este goala.
-functie pentru stergerea unui anumit nod, ce intoarce un pointer spre acesta.
Am verificat cazurile de eroare, m-am pozitionat in lista la pozitia data de 
parametrul n, iar in functie de indexul dat mutam elementele din lista pentru
eliminarea celui dorit. In mod asemanator procedam si pentru adaugarea unui
nod. Modificam campul next atat pentru nodul in urma caruia vom adauga, dar
si pe cel al nodului adaugat. Daca vrem sa adaugam pe prima pozitie , acesta
devine head-ul liste, iar daca adaugam la final devine tail-ul listei. 
Complexitatea in aceste cazuri speciale este O(1) spre deosebire de O(n)
in momentul iterarii prin lista.
-dll_get_size ce intoarce numarul de elemente din lista intr-o complexitate
de O(1)

Suprogramele create ce privesc in mod direct comenzile din problema sunt:
-alloc_arena in cadrul careia initializam dimensiunea arenei si lista de 
blocuri ce urmeaza sa fie pozitionate in careu. Structura lor o sa fie de
lista inlantuita, asa ca initializarea se face folosind un subprogram de creare 
a listelor. 

-pentru dealocarea resurselor din cadrul unei arene, ne pozitionam la 
inceputul acesteia(in capul listei) si parcurgem intreaga lista de blocuri. 
Cand ne pozitionam la inceputul unui bloc verificam daca acesta contine mai 
multe miniblocuri. In caz afirmativ le stergem pe rand, avand grija sa 
eliminam si memoria alocata bufferului read-write. Dupa eliminarea tuturor
miniblocurilor din cadrul blocului curent, eliberam memoria alocata
listei de miniblocuri, dar si blocului si mergem la blocul urmator pana cand
le parcurgem pe toate. Astfel memoria alocata elementelor din arena a fost
dezalocata si putem incheia subprogramul. 

-pentru functia de alocare a unui bloc, verificam mai intai cazurile de 
eroare dupa care efectuam operatiile in functie de pozitia din lista 
in care vrem sa adaugam o noua entitate.  
Analog pentru eliberarea memoriei in functia free_block verificam daca nodul
se afla la inceputul sau finalul listei de blocuri. Pentru aflarea dimensiunii
unui bloc ne folosim de suprogramul count ce pleaca de la un nod primit
ca parametru si intoarce suma size-urilor pentru miniblocurile din acea
portiune.

-pentru functia de citire , stochez dimensiunea pe care trebuie sa o citesc.
Intr-un char plasam informatia din miniblock->rw_buffer si o afisam atata 
timp cat nu am ajuns la finalul textului, iterand in permanenta si prin
lista de miniblockuri. Analog pentru functia write, inlocuim operatia de 
afisare cu cea de alocare de memorie si copiere a informatiei date.

-functia pmap constituie o iteratie continua prin liste, afisand informatii
despre blocuri si miniblocuri. In finalul ei am realizat comparatiile necesare
pentru afisarea permisiunilor. Pentru aflarea numarului de miniblockuri 
am folosit subprogramul mini, ce aduna dimensiunea fiecarei liste de 
miniblocuri, corespunzatoare fiecarui bloc din arena.

-in functia mprotect , iteram prin liste pentru a ajunge la fiecare 
miniblock, caruia ii schimbam permisiunile prin adaugarea valorilor specifice
pentru fiecare permisiune in parte. Initial permisiunea pentru miniblockuri 
este initializata cu 0 pentru a nu depasi valoarea maxima de 7(RWX).
