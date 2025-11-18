#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_NODES 100

typedef struct
{
    int id_node;
    int *paths;
} node;

typedef struct
{
    int num_nodes;
    node *nodes;
    int **adj;
} graph;

// inizializzo la matrice di adiacenza.
void init_adj(graph *G)
{
    for (int i = 0; i < G->num_nodes; i++)
    {
        for (int j = 0; j < G->num_nodes; j++)
        {
            G->adj[i][j] = 0; // inizializzo tutte le celle a 0
        }
    }
}

int count_paths(graph *G, int curr, int pre, bool visited[])
{
    if (curr == pre)
        return 1; // CASO BASE: se il nodo corrente è quello di destinazione, ritorna 1

    visited[curr] = true; // setto visited[curr] a true in modo tale che so che il nodo corrente è stato attraversato.
    int total = 0;

    // esploro tutti gli archi uscenti del nodo corrente
    for (int i = 0; i < G->num_nodes; i++)
    {
        int next = i; // in next viene salvato l'arco in posizione i
        if (G->adj[curr][next] == 1 && !visited[next])
        {
            total += count_paths(G, next, pre, visited); // se il nodo non è visitato, esploro il cammino ricorsivamente.
            if (total >= 2)                              // se total è 2, quindi ho 2 cammini, mi fermo.
                return 2;
        }
    }

    visited[curr] = false;
    return total;
}

bool hasmultp(graph *G, int curr, int pre)
{
    bool visited[G->num_nodes];
    for (int i = 0; i < G->num_nodes; i++)
        visited[i] = false;

    int count = count_paths(G, curr, pre, visited);
    return (count >= 2);
}

int *msp(graph *G, int *Pi, int pi_lenght, int *num_edges)
{

    int max_edges = pi_lenght - 1;

    // Ogni arco occupa 2 int: (from, to)
    int *S = malloc(sizeof(int) * 2 * max_edges);
    if (S == NULL)
    {
        *num_edges = 0;
        return NULL;
    }

    int s_index = 0; // numero di archi nella lista S
    int pre = Pi[pi_lenght - 1];

    for (int j = pi_lenght - 2; j >= 0; j--)
    {
        int curr = Pi[j];
        if (hasmultp(G, curr, pre))
        {
            S[2 * s_index] = curr;          // from
            S[2 * s_index + 1] = Pi[j + 1]; // to
            s_index++;
            pre = curr;
        }
    }

    for (int i = 0; i < s_index / 2; i++)
    {
        int left_from = S[2 * i];
        int left_to = S[2 * i + 1];

        int right_from = S[2 * (s_index - 1 - i)];
        int right_to = S[2 * (s_index - 1 - i) + 1];

        // scambio arco i con arco s_index-1-i
        S[2 * i] = right_from;
        S[2 * i + 1] = right_to;
        S[2 * (s_index - 1 - i)] = left_from;
        S[2 * (s_index - 1 - i) + 1] = left_to;
    }

    *num_edges = s_index; // numero di archi
    return S;
}

int main()
{
    graph G;
    G.num_nodes = 5;
    // alloco matrice di adiacenza
    G.adj = malloc(sizeof(int *) * G.num_nodes);
    for (int i = 0; i < G.num_nodes; i++)
    {
        G.adj[i] = malloc(sizeof(int) * G.num_nodes);
    }
    // 3. inizializzo la matrice
    init_adj(&G);
    // 4. imposto gli archi del grafo
    G.adj[0][1] = 1;
    G.adj[1][2] = 1;
    G.adj[0][3] = 1;
    G.adj[3][2] = 1;

    // 5. creo un cammino
    int Pi[] = {0, 1, 2};
    int pi_len = 3;
    int num_edges;
    int *S = msp(&G, Pi, pi_len, &num_edges);

    // 6. stampo gli archi
    for (int k = 0; k < num_edges; k++)
    {
        printf("Arco %d: %d -> %d\n", k, S[2 * k], S[2 * k + 1]);
    }
    free(S);
    for (int i = 0; i < G.num_nodes; i++)
        free(G.adj[i]);
    free(G.adj);
}
