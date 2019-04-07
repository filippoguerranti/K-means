#include "classi.h"
#include <iostream>
#include <vector>
#include <cmath>
#define EOL3 std::cout << std::endl; std::cout << std::endl; std::cout << std::endl;

using namespace KMeansCluster;

int main() {

 
	DocumentCollection docVec;

	// for (int i = 0; i < 6; ++i) {
		docVec.Add(new Document("testobreve1.txt"));
		docVec.Add(new Document("testobreve2.txt"));
		docVec.Add(new Document("testobreve3.txt"));
		docVec.Add(new Document("testobreve4.txt"));
		docVec.Add(new Document("testobreve5.txt"));
		docVec.Add(new Document("testobreve6.txt"));
		docVec.Add(new Document("testobreve7.txt"));
	//}

	EOL3;

	//docVec.PrintCollection();
	std::vector<std::string> lista = docVec.ListOfWords();
	
	EOL3;

	int k = 3;
	Clustering* kmeans = new KMeans(k, docVec); 

	std::vector<std::vector<int>> cluster = kmeans->Cluster();

	EOL3;
	for (int i = 0; i < cluster.size(); ++i){
		std::cout << "Gruppo " << i << std::endl;
		for (int j = 0; j < cluster[i].size(); ++j){
			std::cout << "Doc " << cluster[i][j] << " ";
		}
		std::cout << std::endl;
	}
}