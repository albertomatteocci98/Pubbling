#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_NODES 100


    typedef struct{
        int idNode;
        int *archiEntranti;
        int *archiUscenti;
        int numEntranti;
        int numUscenti;
    }node;

    typedef struct{
        int numNodes;
        node *nodes;
    }graph;

    typedef struct{
        int daNodo;
        int versoNodo;
    }edge;

   graph Gi;
    node nodes={{2,4,3,5,7,9}};

int countPaths(graph *G, int curr, int pre, bool visited[]) {
    if (curr == pre)
        return 1; // CASO BASE: se il nodo corrente è quello di destinazione, ritorna 1

    visited[curr] = true;//setto visited[curr] a true in modo tale che so che il nodo corrente è stato attraversato.
    int total = 0;

    // esploro tutti gli archi uscenti del nodo corrente
    node *n = &G->nodes[curr]; //dichiaro il nodo n alla posizione curr del grafo G.
    for (int i = 0; i < n->numUscenti; i++) {
        int next = n->archiUscenti[i]; //in next viene salvato l'uscente
        if (!visited[next]) {
            total += countPaths(G, next, pre, visited); //se il nodo non è visitato, esploro il cammino ricorsivamente.
            if (total >= 2) // se total è 2, quindi ho 2 cammini, mi fermo.
                return 2;
        }
    }

    visited[curr] = false;
    return total;
}

bool hasMultiplePaths(graph *G, int curr, int pre) {
    bool visited[G->numNodes];
    for (int i = 0; i < G->numNodes; i++)
        visited[i] = false;

    int count = countPaths(G, curr, pre, visited);
    return (count >= 2);
}


    edge *Min_Size_Pebbling_Algorithm(graph *G, int *Pi, int lunghezza_Pi, int *numArchi)
{
    // inizializzo l'array di archi S:
    edge *S = malloc(sizeof(edge) * lunghezza_Pi);
    int s_index = 0;// indice per scorrere l'array di archi S

    // inizializzo pre:
    int pre = Pi[lunghezza_Pi - 1];


    for (int j = lunghezza_Pi - 2; j >= 0; j--)
    {

        int curr = Pi[j];


        if (hasMultiplePaths(G, curr, pre))
        {
            S[s_index].daNodo = curr;
            S[s_index].versoNodo = Pi[j + 1];
            s_index++;
            pre = curr;
        }
    }

    // 07. reverse di S (per poterlo poi restituire al contrario).
    for (int i = 0; i < s_index / 2; i++)
    {
        edge temp = S[i];
        S[i] = S[s_index - 1 - i];
        S[s_index - 1 - i] = temp;
    }

    *numArchi = s_index; // numero di archi effettivi
    return S;             // restituisce la lista S
}

int main(void)
{
    //CREAZIONE GRAFO:
graph G;
G.numNodes = 6;
G.nodes = malloc(sizeof(node) * G.numNodes);


static int u0[] = {1,2};
G.nodes[0].idNode = 0; G.nodes[0].archiUscenti = u0; G.nodes[0].numUscenti = 2;


static int u1[] = {3,4};
G.nodes[1].idNode = 1; G.nodes[1].archiUscenti = u1; G.nodes[1].numUscenti = 2;


static int u2[] = {1,4};
G.nodes[2].idNode = 2; G.nodes[2].archiUscenti = u2; G.nodes[2].numUscenti = 2;


static int u3[] = {5};
G.nodes[3].idNode = 3; G.nodes[3].archiUscenti = u3; G.nodes[3].numUscenti = 1;

static int u4[] = {5};
G.nodes[4].idNode = 4; G.nodes[4].archiUscenti = u4; G.nodes[4].numUscenti = 1;

G.nodes[5].idNode = 5; G.nodes[5].archiUscenti = NULL; G.nodes[5].numUscenti = 0;

// Cammino Pi:
int Pi[] = {0,1,3,5};
int lenPi = sizeof(Pi)/sizeof(Pi[0]);

int numArchi = 0;
edge *S = Min_Size_Pebbling_Algorithm(&G, Pi, lenPi, &numArchi);

// stampa risultato:
printf("numArchi = %d\n", numArchi);
for (int i = 0; i < numArchi; ++i) {
    printf("S[%d] = (%d -> %d)\n", i, S[i].daNodo, S[i].versoNodo);
}


    free(S);
    free(G.nodes);
    return 0;
}



