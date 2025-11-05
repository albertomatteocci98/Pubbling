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
            if (total >= 2) // se total è 2, quindi ho 2 cammini, mi fermo, altrimenti
                return 2;
        }
    }

    visited[curr] = false; // backtrack
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

    // 02. for j ← |V(Pi)| − 1 to 1
    for (int j = lunghezza_Pi - 2; j >= 0; j--)
    {
        // 03. curr ← V(Pi)[j]
        int curr = Pi[j];

        // 04. if there are multiple paths from curr to pre then
        if (hasMultiplePaths(G, curr, pre))
        {
            // 05. append edge (V(Pi)[j], V(Pi)[j + 1]) to S
            S[s_index].daNodo = curr;
            S[s_index].versoNodo = Pi[j + 1];
            s_index++;

            // 06. pre ← curr
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
    G.numNodes = 4;
    G.nodes = malloc(sizeof(node) * G.numNodes);

    // Nodo 0 → [1, 2]
    int uscenti0[] = {1, 2};
    G.nodes[0].idNode = 0;
    G.nodes[0].archiUscenti = uscenti0;
    G.nodes[0].numUscenti = 2;

    // Nodo 1 → [3]
    int uscenti1[] = {3};
    G.nodes[1].idNode = 1;
    G.nodes[1].archiUscenti = uscenti1;
    G.nodes[1].numUscenti = 1;

    // Nodo 2 → [3]
    int uscenti2[] = {3};
    G.nodes[2].idNode = 2;
    G.nodes[2].archiUscenti = uscenti2;
    G.nodes[2].numUscenti = 1;

    // Nodo 3 → [] (nessun arco uscente)
    G.nodes[3].idNode = 3;
    G.nodes[3].archiUscenti = NULL;
    G.nodes[3].numUscenti = 0;

    //CAMMINO DI INPUT

    int nodiCammino[] = {0, 1, 3};
    int lunghezzaCammino = sizeof(nodiCammino) / sizeof(nodiCammino[0]);

    //ESECUZIONE DELL'ALGORITMO

    int numArchi = 0;
    edge *S = Min_Size_Pebbling_Algorithm(&G, nodiCammino, lunghezzaCammino, &numArchi);

    // OUTPUT

    printf("\nGuardian list (archi selezionati):\n");
    for (int i = 0; i < numArchi; i++)
        printf("(%d → %d)\n", S[i].daNodo, S[i].versoNodo);

    //int numArchi = 0;
    edge *S2 = Min_Size_Pebbling_Algorithm(&G, nodiCammino, lunghezzaCammino, &numArchi);

    printf("\n--- DEBUG ---\n");
    printf("Numero di archi trovati: %d\n", numArchi);
    printf("Contenuto di S:\n");

    for (int i = 0; i < numArchi; i++) {
        printf("S[%d] = (%d -> %d)\n", i, S[i].daNodo, S[i].versoNodo);
}

printf("--- FINE DEBUG ---\n");


    //PULIZIA:
    free(S);
    free(G.nodes);
    return 0;
}



