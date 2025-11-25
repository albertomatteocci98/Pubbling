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
    int *adj;
} graph;

// inizializzo la matrice di adiacenza.
void init_adj(graph *G)
{
    for (int i = 0; i < G->num_nodes; i++)
    {
        G->adj[i] = 0; // inizializzo tutte le celle a 0
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
        if (G->adj[curr * n + next] == 1 && !visited[next])
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

// toglie il newline finale, se presente
void strip(char *s)
{
    int L = strlen(s);
    if (L > 0 && (s[L - 1] == '\n' || s[L - 1] == '\r'))
        s[L - 1] = '\0';
}

// elimina l’ultimo carattere se è '+' o '-'
// (serve per i nodi nel path)
void strip_pm(char *s)
{
    int L = strlen(s);
    if (L > 0 && (s[L - 1] == '+' || s[L - 1] == '-'))
        s[L - 1] = '\0';
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Uso: %s file.gfa\n", argv[0]);
        return 1;
    }

    // apro il file GFA in lettura:
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        printf("Errore apertura file\n");
        return 1;
    }

    char line[4096];
    int max_id = -1; // tiene traccia del nodo massimo visto nei S e nei L

    // Scorro il file per capire quanti nodi esistono:

    while (fgets(line, sizeof line, f))
    {
        strip(line); // rimuove newline
        // se la riga è vuota, passo alla prossima:
        if (line[0] == '\0')
            continue;

        char *type = strtok(line, "\t"); // estraggo il primo campo(S o L o P)
        if (!type)
            continue;

        if (strcmp(type, "S") == 0)
        {
            // se la riga inizia con S:
            char *id = strtok(NULL, "\t");
            if (id)
            {
                int v = atoi(id); // converti l'id in numero
                if (v > max_id)
                    max_id = v;
            }
        }
        else if (strcmp(type, "L") == 0)
        {
            // formato: L <from> <orient> <to> <orient>
            char *from = strtok(NULL, "\t");
            strtok(NULL, "\t"); // salta orientamento
            char *to = strtok(NULL, "\t");

            if (from)
            {
                int u = atoi(from);
                if (u > max_id)
                    max_id = u;
            }
            if (to)
            {
                int v = atoi(to);
                if (v > max_id)
                    max_id = v;
            }
        }
    }

    int N = max_id + 1; // numero totale nodi
    printf("Nodi totali = %d\n", N);

    // alloco una matrice di adiacenza NxN:
    int *adj = calloc(N * N, sizeof(int));

    int *Pi = NULL; // path "Pi" (array di nodi)
    int pi_len = 0; // lunghezza del path

    rewind(f); // ritorno ad inizio file

    while (fgets(line, sizeof line, f))
    {
        strip(line);
        char *type = strtok(line, "\t");
        if (!type)
            continue;
        // se la riga è di tipo L, carico l'arco:
        if (strcmp(type, "L") == 0)
        {
            // gestisce gli archi
            char *from = strtok(NULL, "\t");
            strtok(NULL, "\t"); // salta orient
            char *to = strtok(NULL, "\t");

            int u = atoi(from);
            int v = atoi(to);

            adj[u * N + v] = 1; // registriamo l’arco u -> v
        }
        // se la riga è di tipo P, leggo il path:
        else if (strcmp(type, "P") == 0)
        {
            // gestisce il path
            strtok(NULL, "\t");              // salto nome del path
            char *list = strtok(NULL, "\t"); // lista id separati da virgole

            // conto quanti nodi ci sono nella lista contando le virgole:
            int count = 1;
            for (char *p = list; *p; p++)
                if (*p == ',')
                    count++;

            Pi = malloc(sizeof(int) * count); // alloco l'array del path

            int k = 0;
            char *elem = strtok(list, ","); // un id per volta
            while (elem)
            {
                strip_pm(elem);           // rimuovo + o -
                Pi[k++] = atoi(elem);     // converto in numero
                elem = strtok(NULL, ","); // passo al prossimo id
            }
            pi_len = k;
        }
    }

    fclose(f);

    // stampo gli archi trovati:
    printf("Archi trovati:\n");
    for (int u = 0; u < N; u++)
        for (int v = 0; v < N; v++)
            if (adj[u * N + v])
                printf("%d -> %d\n", u, v);
    // stampo un cammino se esiste:
    if (Pi)
    {
        printf("Path Pi: ");
        for (int i = 0; i < pi_len; i++)
            printf("%d ", Pi[i]);
        printf("\n");
        free(Pi);
    }

    free(adj);
    return 0;
}
