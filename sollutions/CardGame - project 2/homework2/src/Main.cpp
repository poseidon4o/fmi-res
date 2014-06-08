
#include "Header.h"
#include <string>
using namespace std;

int main(){

	//Deck d("file.bin");
	Deck d;
	for (int i = 0; i < 15; i++){
		d.AddCard(Card(i + 10, i * 5, "card"));
	}
	//d.SaveToFile("file.bin");

	d.SaveBest(Deck::Criterion::Energy, "best.txt");

	return 0;
}

