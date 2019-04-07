#include "classi.h" 

namespace KMeansCluster { 

	// function
	
		std::vector<int> MapToVector(const std::map<std::string, int>& mappa) {
			std::vector<int> v;
			for (std::map<std::string, int>::const_iterator iter = mappa.begin(); iter != mappa.end(); ++iter) {
				v.push_back(iter->second);
			}
			return v;
		}

	// end function

	// class Document

		int Document::newID = 0;

		void Document::CleanText(std::string& testo, const std::string& stopwordFile) {
			// Trasforma tutte le lettere minuscole in maiuscole
			for (int i = 0; i < testo.length(); ++i) {
				if (isupper(testo[i])) 
					testo[i] = tolower(testo[i]);
			}

			// Sostituisce i simboli (£, °, «, ») in (.) che verranno rimossi successivamente
			std::string toRemove("£°«»’'");
			for (auto i : toRemove) {
				std::replace(testo.begin(), testo.end(), i, ' ');
			}
				
			// Rimuove la punteggiatura
			testo.erase(remove_if(testo.begin(), testo.end(), &ispunct), testo.end()); 

			// Rimuove i numeri
			testo.erase(remove_if(testo.begin(), testo.end(), &isdigit), testo.end()); 

			// Tokenizza la stringa in un vettore di sub-stringhe
			std::vector <std::string> tokens;
			std::istringstream testostream(testo);
			std::string token; 
			while (testostream >> token)
				tokens.push_back(token);

			// Apre file esterno contenente le stopwords
			std::ifstream stopword(stopwordFile);
			if (!stopword.good()) {
				std::cerr << "Impossibile aprire il file." << std::endl;
				exit(1);
			}

			// Elimina le stopwords
			std::string parola;
			while (stopword >> parola) {
				// std::cout << parola << std::endl;
				for (int i = 0; i < tokens.size(); ++i) {
					if (parola.compare(tokens[i]) == 0)
						tokens.erase(tokens.begin() + i);
				}
			}

			// De-tokenizza 
			testo.clear();
			for (int i = 0; i < tokens.size(); ++i) {
				testo += tokens[i] + " "; 
			}	
		}

		void Document::AddNotPresentWord(const std::string& word) {
			bagOfWords[word] = 0;
		}

		Document::Document(const std::string& nomeFile) : ID(newID++) {
			nomeDoc = nomeFile;
			std::ifstream file(nomeFile);
			if (!file.good()) {
				std::cerr << "Impossibile aprire il file." << std::endl;
				exit(1);
			}

			// scrivo il file in una stringa 'testo'
			std::string testo;
			std::string parola;
			while (file >> parola) {
				testo += " " + parola;
			}

			CleanText(testo, "stopwords-it.txt");

			std::istringstream buf(testo);
			for (std::string parola; buf >> parola;) {
				bagOfWords[parola]++;
			}
		}

		Document::Document(const Document& documento) : 
			ID(newID++), nomeDoc(documento.Nome()), bagOfWords(documento.Bag()) {

		}

		void Document::PrintBag() const {
			for (auto i : this->bagOfWords) {
				std::cout << i.first << " -> " << i.second << std::endl; 
			}
		}

	// end class Document

	// class DocumentCollection

		DocumentCollection::DocumentCollection() {
		}

		DocumentCollection::DocumentCollection(const std::vector<Document*>& docVec_) : docVec(docVec_) {
		}

		DocumentCollection::DocumentCollection(const DocumentCollection& docVec_) : docVec(docVec_.DocVec()) {	
		}

		DocumentCollection::~DocumentCollection() {
			docVec.clear();
		}

		void DocumentCollection::Refresh() {
			std::vector<std::string> lista = this->ListOfWords();
			for (auto i : this->docVec) {
				std::map<std::string, int> mappa = i->Bag();
				for (int j = 0; j < lista.size(); ++j) {
					if (mappa.find(lista[j]) == mappa.end()) {
						i->AddNotPresentWord(lista[j]);
					}
				}
			}
		}
		
		void DocumentCollection::Add(Document* document) {
			docVec.push_back(document);
			for (auto i : document->Bag()) {
				if (std::find(this->listOfWords.begin(), this->listOfWords.end(), i.first) == this->listOfWords.end()) {
					this->listOfWords.push_back(i.first);
				}
			} 
			this->Refresh();
		}

		void DocumentCollection::PrintCollection() const {
			for (int i = 0; i < this->NumberOfDocs(); ++i) {
				std::cout << "TESTO #" << docVec[i]->GetID() << std::endl;
				docVec[i]->PrintBag();
				std::cout << std::endl;
			}
		}

		std::map<std::string, int> DocumentCollection::Baricentro() const {
			std::map<std::string, int> mappa;
			for (int i = 0; i < this->NumberOfDocs(); ++i){
				std::map<std::string, int> mappaDoc = docVec[i]->Bag();
				for (std::map<std::string, int>::const_iterator iter = mappaDoc.begin(); iter != mappaDoc.end(); ++iter){
					mappa[iter->first] += iter->second;
				}
			}
			for (std::map<std::string, int>::const_iterator iter = mappa.begin(); iter != mappa.end(); ++iter){
				mappa[iter->first] = mappa[iter->first] / this->NumberOfDocs();
			}
			return mappa;
		}


	// end class DocumentCollection

	// class KMeans
		KMeans::KMeans(const int& K_, const DocumentCollection& docCollection_) : K(K_), docCollection(docCollection_) {
			srand((unsigned)time(0)); 
		} 

		KMeans::~KMeans() {}

		double KMeans::CosineSimilarity(const std::map<std::string, int>& mappa1, const std::map<std::string, int>& mappa2) const {

			std::vector<int> v1 = MapToVector(mappa1);
			std::vector<int> v2 = MapToVector(mappa2);
			double ab = 0;
			double aa = 0;
			double bb = 0;
			for (int i = 0; i < v1.size(); ++i) {
				aa += v1[i]*v1[i];
				bb += v2[i]*v2[i];
				ab += v1[i]*v2[i];
			}
			
			return (ab/(sqrt(aa)*sqrt(bb)));
		}

		std::vector<std::vector<int>> KMeans::Cluster() const {	

			std::cout << "***** CLUSTER *****" << std::endl << std::endl << std::endl;

			// lista delle parole della collezione di documenti
			std::vector<std::string> lista = docCollection.ListOfWords();
			
			// numero di documenti della collezione
			int numDocs = docCollection.NumberOfDocs();

			// scelgo K documenti in modo casuale (centri)
			std::vector<std::map<std::string, int>> centri(K);
			std::vector<int> indexCentri(K);
			for (int i = 0; i < K; ++i) {
				int randNumber;
				do {
					randNumber = rand() % numDocs;
				} while (std::find(indexCentri.begin(), indexCentri.end(), randNumber) != indexCentri.end());
				centri[i] = docCollection[randNumber].Bag();
				indexCentri[i] = randNumber;
			}	

			// centri[0] = docCollection[1].Bag();
			// centri[1] = docCollection[5].Bag();
			// centri[2] = docCollection[6].Bag();

			// calcolo la similarità coseno di ogni documento rispetto ad ogni centro
			std::vector<std::vector<double>> similarita(numDocs, std::vector<double>(K));
			for (int i = 0; i < numDocs; ++i){
				for (int j = 0; j < K; ++j){
					similarita[i][j] = this->CosineSimilarity(docCollection[i].Bag(), centri[j]);
				}
			}

			// per ogni documento individuo il gruppo di appartenenza in base alla similarità
			std::vector<int> docSimilarity(numDocs);
			for (int i = 0; i < numDocs; ++i){
				double temp = similarita[i][0];
				int gruppo = 0;
				for (int j = 1; j < similarita[i].size(); ++j){
					if (similarita[i][j] > temp) {
						temp = similarita[i][j];
						gruppo = j;
					}
				}
				docSimilarity[i] = gruppo;
			}

			std::cout << std::endl <<"MAXSIMILARITA" << std::endl;
			for (int i = 0; i < docSimilarity.size(); ++i){
				std::cout << "DOC " << i << ": " << docSimilarity[i] << std::endl;
			}

			// creo il vettore di vettori contenente, per ogni gruppo, gli indici dei documenti appartenenti a quel gruppo
			std::vector<std::vector<int>> cluster(K);
			for (int i = 0; i < docSimilarity.size(); ++i) {
				for (int j = 0; j < K; ++j) {	
					if (docSimilarity[i] == j)
						cluster[j].push_back(i);
				}
			}
			
			std::cout << std::endl << std::endl << std::endl << std::endl;

			// calcolo il baricentro di ogni gruppo
			for (int i = 0; i < K; ++i) {
				DocumentCollection* gruppoi = new DocumentCollection();
				for (int j = 0; j < cluster[i].size(); ++j) {
					gruppoi->Add(&docCollection[cluster[i][j]]);
				}
				//gruppoi->PrintCollection();
				centri[i] = gruppoi->Baricentro();
				delete gruppoi;
			}	

			for (int i = 0; i < centri.size(); ++i){
				std::cout << std::endl << "BARICENTRO " << i << std::endl;
				for (std::map<std::string, int>::const_iterator iter = centri[i].begin(); iter != centri[i].end(); ++iter){
					std::cout << iter->first << " -> " << iter->second << std::endl;
				}
			}

			// *******************************************************************************

			

			// calcolo la similarità coseno di ogni documento rispetto ad ogni centro e prendo la più grande (valore più vicino a 1)
			for (int i = 0; i < numDocs; ++i){
				for (int j = 0; j < K; ++j){
					similarita[i][j] = this->CosineSimilarity(docCollection[i].Bag(), centri[j]);
				}
			}

			// per ogni documento individuo il gruppo di appartenenza in base alla similarità
			for (int i = 0; i < numDocs; ++i){
				double temp = similarita[i][0];
				int gruppo = 0;
				for (int j = 1; j < similarita[i].size(); ++j){
					if (similarita[i][j] > temp) {
						temp = similarita[i][j];
						gruppo = j;
					}
				}
				docSimilarity[i] = gruppo;
			}

			std::cout << std::endl <<"MAXSIMILARITA" << std::endl;
			for (int i = 0; i < docSimilarity.size(); ++i){
				std::cout << "DOC " << i << ": " << docSimilarity[i] << std::endl;
			}

			// creo il vettore di vettori contenente, per ogni gruppo, gli indici dei documenti appartenenti a quel gruppo
			for (int i = 0; i < K; ++i){
				cluster[i].clear();	
			}
			for (int i = 0; i < docSimilarity.size(); ++i) {
				for (int j = 0; j < K; ++j) {
					if (docSimilarity[i] == j)
						cluster[j].push_back(i);
				}
			}
			
			std::cout << std::endl << std::endl << std::endl << std::endl;

			// calcolo il baricentro di ogni gruppo
			for (int i = 0; i < K; ++i) {
				DocumentCollection* gruppoi = new DocumentCollection();
				for (int j = 0; j < cluster[i].size(); ++j) {
					gruppoi->Add(&docCollection[cluster[i][j]]);
				}
				//gruppoi->PrintCollection();
				centri[i] = gruppoi->Baricentro();
				delete gruppoi;
			}	

			for (int i = 0; i < centri.size(); ++i){
				std::cout << std::endl << "BARICENTRO " << i << std::endl;
				for (std::map<std::string, int>::const_iterator iter = centri[i].begin(); iter != centri[i].end(); ++iter){
					std::cout << iter->first << " -> " << iter->second << std::endl;
				}
			}


			return cluster;


		}
	// end class KMeans

} // end namespace KMeansCluster

