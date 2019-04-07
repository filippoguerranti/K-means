# K-means
Progetto PPS
K-means Clustering:
http://en.wikipedia.org/wiki/Kmeans

Il clustering di documenti, data una collezione documentale, li raccoglie in gruppi (cluster). 
I documenti all'interno di un singolo gruppo devono avere contenuto tendenzialmente omogeneo.

a) Classe DOCUMENT
		Contiene la rappresentazione bag-of-words parola->numero_occorrenze.

b) Classe DOCUMENTCOLLECTION
		La collezione documentale viene realizzata tramite una classe DocumentCollection 
		che contiene un vettore di documenti, ognuno realizzato tramite un istanza di classe Document.

c) Classe astratta CLUSTERING
		Implementa l'interfaccia di un generico algoritmo di Clustering. 
		In particolare, dovra' essere presente un metodo Cluster() 
		che ritorni un vettore di vettori di indici dei documenti.

d) Classe KMEANS
		Figlia di Clustering.
		L'algoritmo Kmeans parte scegliendo K documenti in modo casuale nella collezione: questi sono detti centri. 
		Una prima creazione di gruppi avviene assegnando i documenti nella collezione al centro più vicino.
		L'algoritmo quindi ricalcola i centri calcolando i baricentri dei documenti assegnati ad un centro. 
		Quindi si itera il processo, si riassegnano i documenti al centro più vicino, ecc.
		L'algoritmo termina quando le assegnazioni dei documenti non cambiano tra due iterazioni successive.



PUNTO A) [https://www.experfy.com/blog/k-means-clustering-in-text-data]
	- 	ottenuti i documenti occorre "ripulirli" (data cleansing).
		Rimuovere punteggiatura, numeri, stopwords, URLs.
	- 	creare una bag-of-word parola->numero_occorrenze 

PUNTO B)
