#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAX_NODES 100
#define MAX_PATHS 1024

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
    int n = G->num_nodes;
    for (int i = 0; i < n * n; i++)
    {
        G->adj[i] = 0;
    }
}

/**
 count_paths:
 -Funzione ricorsiva (DFS) che conta i percorsi distinti tra due nodi.
 -La funzione interrompe la ricerca non appena il numero di percorsi trovati raggiunge o supera 2.
 -G: Puntatore al grafo.
 -curr: ID del nodo corrente nell'esplorazione.
 -pre: ID del nodo destinazione (Target).
 -visited: Array booleano per tenere traccia dei nodi visitati nel cammino corrente.
 -return int: 0, 1 o 2 (dove 2 indica "2 o più percorsi").
 */
int count_paths(graph *G, int curr, int pre, bool visited[])
{
    int n = G->num_nodes;
    if (curr == pre)
        return 1;

    visited[curr] = true;
    int total = 0;

    for (int i = 0; i < G->num_nodes; i++)
    {
        int next = i;
        if (G->adj[curr * n + next] == 1 && !visited[next])
        {
            total += count_paths(G, next, pre, visited);
            if (total >= 2)
                return 2;
        }
    }

    visited[curr] = false;
    return total;
}

/**
  hasmultp:
 -funzione che verifica se esistono più cammini tra due nodi.
 -Prepara l'array 'visited' e lancia la funzione ricorsiva count_paths.
 -G: Puntatore al grafo.
 -curr: Nodo di partenza (Sorgente).
 -pre: Nodo di arrivo (Destinazione).
 -return true: Se esistono almeno due percorsi distinti.
 -return false: Se esiste un solo percorso o nessuno.
 */
bool hasmultp(graph *G, int curr, int pre)
{
    bool visited[G->num_nodes];
    for (int i = 0; i < G->num_nodes; i++)
        visited[i] = false;

    int count = count_paths(G, curr, pre, visited);
    return (count >= 2);
}

/**
 -compute_multipath_matrix:
 -Costruisce e popola la matrice quadrata mp_matrix (N x N).
 -Itera su tutte le coppie di nodi (i, j) e imposta a 1 le celle corrispondenti a coppie collegate da 2 o più cammini
 -G: Puntatore al grafo.
 */
int *compute_multipath_matrix(graph *G)
{
    int n = G->num_nodes;
    int *matrix = calloc(n * n, sizeof(int));
    if (!matrix)
        return NULL;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
                continue;
            if (hasmultp(G, i, j))
            {
                matrix[i * n + j] = 1;
            }
        }
    }
    return matrix;
}

/**
 -MSP:
 -1. Scorre il path dal fondo verso l'inizio.
 -2. Consulta mp_matrix per trovare i punti di variazione (multipath).
 -3. Mantiene solo gli archi necessari a descrivere come i nodi sono collegati tra loro, scartando i nodi ridondanti.
 -G: Puntatore al grafo.
 - pi: Array di interi rappresentante il percorso (sequenza di nodi).
 - pi_lenght: Lunghezza del percorso.
 - num_edges: Puntatore a intero dove verrà scritto il numero di archi trovati (Output).
 - mp_matrix: La matrice di connettività calcolata in pre-elaborazione.
 -int*: Array dinamico contenente le coppie di nodi (u, v) degli archi selezionati.
 */
int *msp(graph *G, int *pi, int pi_lenght, int *num_edges, int *mp_matrix)
{
    int n = G->num_nodes;
    int max_edges = pi_lenght - 1;

    int *S = malloc(sizeof(int) * 2 * max_edges);
    if (S == NULL)
    {
        *num_edges = 0;
        return NULL;
    }

    int s_index = 0;
    int pre = pi[pi_lenght - 1];

    for (int j = pi_lenght - 2; j >= 0; j--)
    {
        int curr = pi[j];

        if (mp_matrix[curr * n + pre] == 1)
        {
            S[2 * s_index] = curr;
            S[2 * s_index + 1] = pi[j + 1];
            s_index++;
            pre = curr;
        }
    }

    // Inversione (swap) per ordine corretto
    for (int i = 0; i < s_index / 2; i++)
    {
        int left_from = S[2 * i];
        int left_to = S[2 * i + 1];
        int right_from = S[2 * (s_index - 1 - i)];
        int right_to = S[2 * (s_index - 1 - i) + 1];

        S[2 * i] = right_from;
        S[2 * i + 1] = right_to;
        S[2 * (s_index - 1 - i)] = left_from;
        S[2 * (s_index - 1 - i) + 1] = left_to;
    }

    *num_edges = s_index;
    return S;
}
// funzione che verrà usata per avere il minimo tra 2 e il numero di cammini.
int min_val(int a, int b)
{
    return (a < b) ? a : b;
}
// funzione che verrà utilizzata per verificare se un nodo è presente nel percorso.
bool is_in_path(int u, int *pi, int pi_len)
{
    for (int i = 0; i < pi_len; i++)
    {
        if (pi[i] == u)
            return true;
    }
    return false;
}

/**
 msp_2:
 -1. Parte dall'ultimo nodo e torna indietro verso l'inizio.
 -2. Non ha bisogno della matrice di prima: conta i percorsi mentre scorre i nodi.
 -3. Capisce che c'è un bivio quando la somma delle strade dei vicini è maggiore di 1.
 -4. Se trova un bivio sul percorso giusto, salva l'arco e riparte da lì come nuovo traguardo.
 -G: Il grafo.
 -pi: La lista dei nodi del percorso.
 -pi_len: Lunghezza del percorso.
 -num_edges: Variabile in cui scrivo quanti archi ho trovato alla fine.
 -return: La lista degli archi salvati.
 */
int *msp_2(graph *G, int *pi, int pi_len, int *num_edges)
{
    int n = G->num_nodes;
    int max_edges = pi_len > 0 ? pi_len : 1;

    int *S = malloc(sizeof(int) * 2 * max_edges);
    if (!S)
    {
        *num_edges = 0;
        return NULL;
    }
    int s_index = 0;
    int *node_paths = calloc(n, sizeof(int));
    if (!node_paths)
    {
        free(S);
        return NULL;
    }
    int pre = pi[pi_len - 1];
    node_paths[pre] = 1;

    for (int curr = n - 1; curr >= 0; curr--)
    {
        if (curr == pre)
            continue;
        int count = 0;
        for (int mid = 0; mid < n; mid++)
        {
            if (G->adj[curr * n + mid] == 1)
            {
                if (mid <= pre)
                {
                    count = min_val(2, count + node_paths[mid]);
                }
            }
        }

        node_paths[curr] = count;

        if (node_paths[curr] > 1 && is_in_path(curr, pi, pi_len))
        {
            bool saved = false;
            for (int mid = 0; mid < n; mid++)
            {
                if (G->adj[curr * n + mid] == 1)
                {
                    if (is_in_path(mid, pi, pi_len))
                    {
                        S[2 * s_index] = curr;
                        S[2 * s_index + 1] = mid;
                        s_index++;
                        saved = true;
                        break;
                    }
                }
            }
            if (saved)
            {
                pre = curr;
                node_paths[pre] = 1;
            }
        }
    }
    // 16. Inversione S
    for (int i = 0; i < s_index / 2; i++)
    {
        int t1 = S[2 * i];
        int t2 = S[2 * i + 1];
        S[2 * i] = S[2 * (s_index - 1 - i)];
        S[2 * i + 1] = S[2 * (s_index - 1 - i) + 1];
        S[2 * (s_index - 1 - i)] = t1;
        S[2 * (s_index - 1 - i) + 1] = t2;
    }

    *num_edges = s_index;
    free(node_paths);
    return S;
}

// toglie il newline finale, se presente.
void strip(char *s)
{
    int L = strlen(s);
    if (L > 0 && (s[L - 1] == '\n' || s[L - 1] == '\r'))
        s[L - 1] = '\0';
}

/**
 -Rimuove i suffissi di orientamento ('+' o '-') dalle stringhe dei nodi GFA.
 -Esempio: converte "13+" in "13" per permettere la conversione in intero (atoi).
 */
void strip_pm(char *s)
{
    int L = strlen(s);
    if (L > 0 && (s[L - 1] == '+' || s[L - 1] == '-'))
        s[L - 1] = '\0';
}

/**
 -Funzione principale.
 -1. Legge il file GFA.
 -2. Costruisce la matrice di adiacenza e carica i Path.
 -3. Esegue la pre-elaborazione (compute_multipath_matrix).
 -4. Itera su tutti i percorsi caricati applicando l'algoritmo MSP.
 -5. Stampa a video i risultati e libera la memoria.
 */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Uso: %s file.gfa\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        printf("Errore apertura file\n");
        return 1;
    }

    char line[4096];
    int max_id = -1;

    while (fgets(line, sizeof line, f))
    {
        strip(line);
        if (line[0] == '\0')
            continue;

        char *type = strtok(line, "\t");
        if (!type)
            continue;

        if (strcmp(type, "S") == 0)
        {
            char *id = strtok(NULL, "\t");
            if (id)
            {
                int v = atoi(id);
                if (v > max_id)
                    max_id = v;
            }
        }
    }

    int n = max_id + 1;
    printf("Nodi totali = %d\n", n);

    int *adj = calloc(n * n, sizeof(int));

    int *pi = NULL;
    int pi_len = 0;
    int **all_paths = malloc(sizeof(int *) * MAX_PATHS);
    int *all_lenghts = malloc(sizeof(int) * MAX_PATHS);
    int path_count = 0;
    if (!all_paths || !all_lenghts)
    {
        printf("Errore alloc Paths\n");
        return 1;
    }

    rewind(f);

    while (fgets(line, sizeof line, f))
    {
        strip(line);
        char *type = strtok(line, "\t");
        if (!type)
            continue;
        if (strcmp(type, "L") == 0)
        {
            char *from = strtok(NULL, "\t");
            strtok(NULL, "\t");
            char *to = strtok(NULL, "\t");

            int u = atoi(from);
            int v = atoi(to);

            adj[u * n + v] = 1;
        }
        else if (strcmp(type, "P") == 0)
        {
            strtok(NULL, "\t");
            char *list = strtok(NULL, "\t");
            int count = 1;
            for (char *p = list; *p; p++)
                if (*p == ',')
                    count++;

            pi = malloc(sizeof(int) * count);

            int k = 0;
            char *elem = strtok(list, ",");
            while (elem)
            {
                strip_pm(elem);
                pi[k++] = atoi(elem);
                elem = strtok(NULL, ",");
            }
            pi_len = k;
            all_paths[path_count] = pi;
            all_lenghts[path_count] = pi_len;
            path_count++;
        }
    }

    fclose(f);

    printf("Archi trovati:\n");
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (adj[u * n + v])
                printf("%d -> %d\n", u, v);
    if (!all_paths || !all_lenghts)
    {
        printf("Errore alloc Paths\n");
        return 1;
    }

    graph G = (graph){0};
    G.num_nodes = n;
    G.adj = adj;
    G.nodes = calloc(n, sizeof(node));
    if (!G.nodes)
    {
        printf("Errore allocazione G.nodes\n");
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        G.nodes[i].id_node = i;
        G.nodes[i].paths = NULL;
    }

    printf("Calcolo matrice multipath (pre-elaborazione)...\n");
    int *mp_matrix = compute_multipath_matrix(&G);
    if (!mp_matrix)
    {
        printf("Errore allocazione matrice multipath\n");
        return 1;
    }

    for (int p = 0; p < path_count; p++)
    {
        int *pi_curr = all_paths[p];
        int len = all_lenghts[p];
        printf("Path #%d: ", p);
        for (int i = 0; i < len; i++)
            printf("%d ", pi_curr[i]);
        printf("\n");

        // CHIAMATA MSP 1
        int num_edges = 0;
        int *S = msp(&G, pi_curr, len, &num_edges, mp_matrix);

        printf("  MSP -> %d archi:\n", num_edges);
        for (int e = 0; e < num_edges; e++)
        {
            printf("    %d -> %d\n", S[2 * e], S[2 * e + 1]);
        }
        free(S);

        // CHIAMATA MSP 2
        int num_edges2 = 0;
        int *S2 = msp_2(&G, pi_curr, len, &num_edges2);

        printf("  [MSP 2] -> %d archi:\n", num_edges2);
        for (int e = 0; e < num_edges2; e++)
        {
            printf("    %d -> %d\n", S2[2 * e], S2[2 * e + 1]);
        }
        free(S2);
    }

    // libero la memoria:
    free(mp_matrix);
    for (int p = 0; p < path_count; p++)
        free(all_paths[p]);
    free(all_paths);
    free(all_lenghts);
    free(G.nodes);

    free(adj);
    return 0;
}
