#ifndef CLASSI_H
#define CLASSI_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib> 
#include <ctime>
#include <cmath>

namespace KMeansCluster {

	class DocumentCollection;

	class Document {

		friend class DocumentCollection; // per AddNotPresentWord
 
		private:
			// class members
			static int newID;
			const int ID;
			std::string nomeDoc;
			std::map <std::string, int> bagOfWords; //parole->numero_occorrenze

			// metodi
			void CleanText(std::string& testo, const std::string& stopwordFile); // ripulisce il testo da stopwords, punteggiatura, numeri e parti inutili
			void AddNotPresentWord(const std::string& word); // usato quando documento viene aggiunto ad un DocumentCollection
		
		public:
			// costruttori
			Document(const std::string& nomeFile);
			Document(const Document& documento); // copy-constructor

			// metodi
			inline int GetID() const { return ID; }
			inline std::map <std::string, int> Bag() const { return bagOfWords; }
			inline std::string Nome() const { return nomeDoc; }
			void PrintBag() const;

	}; // end class Document

	class DocumentCollection {

		private:
			// class members
			std::vector<Document*> docVec;
			std::vector<std::string> listOfWords;

			// metodi
			void Refresh();

		public:
			// costruttori
			DocumentCollection();
			DocumentCollection(const std::vector<Document*>& docVec_);
			DocumentCollection(const DocumentCollection& docVec_);

			// distruttore
			~DocumentCollection();

			// metodi
			inline std::vector<Document*> DocVec() const { return docVec; }
			inline int NumberOfDocs() const { return docVec.size(); }
			inline std::vector<std::string> ListOfWords() const { return listOfWords; };
			void Add(Document* document);
			void PrintCollection() const;
			std::map<std::string, int> Baricentro() const;

			// operator overloading
			inline Document& operator[] (int i) const { return *docVec[i]; }
			inline Document& at(int i) const { return *docVec[i]; }

	}; // end class DocumentCollection

	class Clustering {

		private:
			// class members

		public:
			// costruttore
			Clustering(){};

			// distruttore
			virtual ~Clustering(){};

			// metodi
			virtual std::vector<std::vector<int>> Cluster() const = 0;

	}; // end class Clustering

	class KMeans : public Clustering {

		private:
			// class members
			int K;
			DocumentCollection docCollection;

		public:
			// costruttore
			KMeans(const int& K_, const DocumentCollection& docCollection_);

			// distruttore
			~KMeans();

			// metodi
			std::vector<std::vector<int>> Cluster() const;
			double CosineSimilarity(const std::map<std::string, int>& mappa1, const std::map<std::string, int>& mappa2) const; // similarità coseno tra due documenti

	}; // end class KMeans

	std::vector<int> MapToVector(const std::map<std::string, int>& mappa);

} // end namespace KMeanCluster

#endif // end CLASSI_H