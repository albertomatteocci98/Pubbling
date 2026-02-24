Il codice implementa il Min-Size Pebbling Algorithm per un grafo aciclico orientato (DAG).
L’algoritmo scorre il cammino Pi all’indietro e, per ogni nodo, verifica (tramite le funzioni hasMultiplePaths e countPaths) se esistono almeno due cammini distinti che collegano il nodo corrente a quello di riferimento. In tal caso, aggiunge l’arco corrispondente alla lista S.
Nodi, grafi e archi sono definite come delle struct.
-hasMultiplePaths controlla se tra due nodi ci sono almeno due cammini diversi.
-countPaths li conta realmente, esplorando il grafo in profondità (ricorsione) e fermandosi appena ne trova due.
