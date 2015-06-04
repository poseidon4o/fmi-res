#include "Header.h"
#include <fstream>

/// Getter for deck size
size_t Deck::GetSize() const{
	return this->size;
}

/// Tells if a deck is valid (has 5 or more cards)
bool Deck::IsValid() const{
	return this->GetSize() >= 5;
}

/// Default constructor
Deck::Deck(){
	this->Init(this->initialCapacity, 0);
}

/// Copy constructor
Deck& Deck::operator=(const Deck& another){
	if (this != &another){
		this->Init(another.capacity, another.GetSize());

		for (size_t i = 0; i < this->GetSize(); i++)
		{
			this->internalArr[i] = another.internalArr[i];
		}
	}
	return *this;
}

/// Copy constructor
Deck::Deck(const Deck& another){
	this->Init(another.capacity, another.GetSize());

	for (size_t i = 0; i < this->GetSize(); i++)
	{
		this->internalArr[i] = another.internalArr[i];
	}
}

/// Destructor
Deck::~Deck(){
	if (this->fromFile){
		this->SaveToFile(this->fileName); // TODO 
	}
	delete[] this->internalArr;
}

/// Method for adding a card to the end of the deck
void Deck::AddCard(const Card& card){
	if (this->capacity <= this->size){
		this->ExtendInternal();
	}
	this->internalArr[this->size] = card;
	this->size++;
}

/// Method for taking the last card from a deck
Card& Deck::operator[](size_t pos){
	if (this->size > pos){
		return 	this->internalArr[pos];
	}
	else{
		throw "Index out of range!";
	}
}

/// Method to clear the deck
void Deck::Clear(){
	delete[] this->internalArr;
	this->Init(this->initialCapacity, 0);
}

/// Shuffles the deck
void Deck::Shuffle(){
	throw new std::exception("Not implemented");
	// TODO!
}

/// Saves a deck to file
void Deck::SaveToFile(const char* fileName){

	std::ofstream file(fileName, std::ios::binary | std::ios::trunc);

	if (!file)
		std::cerr << "Deck cannot be saved : File cannot be opened!";
	if (!this->IsValid())
		std::cerr << "Deck cannot be saved : It is not a valid deck!";

	file.write((char*)&this->capacity, sizeof(this->capacity));
	file.write((char*)&this->size, sizeof(this->size));

	for (size_t i = 0; i < this->size && file; i++)
	{
		this->internalArr[i].SaveToFile(file);
	}

	if (!file){
		std::cerr << "Deck cannot be saved : File error!";
	}

	file.close();
}

/// Initializes a deck from file
Deck::Deck(const char* fileName){
	std::ifstream file = std::ifstream(fileName, std::ios::binary);
	if (!file.good()){
		std::cerr << "Deck cannot be initialized : File error";
		return;
	}

	this->fromFile = true;
	this->fileName = new char[strlen(fileName)+1];
	strcpy(this->fileName, fileName);

	file.read((char*)&this->capacity, sizeof(this->capacity));
	file.read((char*)&this->size, sizeof(this->size));

	if (!file){
		std::cerr << "Deck cannot be initialized : File error";
		return;
	}

	if (!this->IsValid()){
		std::cerr << "Deck cannot be initialized : It is not a valid deck!";
		return;
	}
	this->internalArr = new Card[this->capacity];

	for (size_t i = 0; i < this->size && file; i++)
	{
		this->internalArr[i] = Card(file);
		if (!file){
			std::cerr << "Deck cannot be initialized : File ended unexpectedly";
			return;
		}
	}

	if (!file){
		std::cerr << "Deck cannot be initialized : File error";
	}
	file.close();
}

/// Saves best cards by given parameters in file
void Deck::SaveBest(Criterion cr, const char* fileName){
	std::ofstream textFile(fileName);
	if (this->numberOfBestCards > this->GetSize()){
		std::cerr << "Not enough cards!";
		textFile.close();
		return;
	}

	switch(cr){
		case Criterion::Attack:
			this->SaveByDESC(textFile, &Card::GetCurrentAttack);
			break;
		case Criterion::Life:
			this->SaveByDESC(textFile, &Card::GetCurrentLife);
			break;
		case Criterion::Energy:
			this->SaveByASC(textFile, &Card::GetEnergy);
			break;
		case Criterion::Name:
			std::cerr << "Not implemented!";
			break;
	}
	textFile.close();
}

/// Extends internal array
void Deck::ExtendInternal(){
	Card* temp = new Card[this->capacity * 2];
	if (!temp){
		std::cerr << "Cannot extend deck!";
		return;
	}

	for (size_t i = 0; i < this->size; i++)
	{
		temp[i] = this->internalArr[i];
	}

	this->capacity *= 2;
	delete[] this->internalArr;
	this->internalArr = temp;
}

/// Shrinks internal array
void Deck::ShrinkInternal(){
	// ASK: What if there is not enough space to copy the data? 
	//		-> How to delete from given location to the end
	Card* temp = new Card[this->capacity / 2];

	for (size_t i = 0; i < this->size; i++)
	{
		temp[i] = this->internalArr[i];
	}

	delete[] this->internalArr;
	this->internalArr = temp;
	this->capacity /= 2;

}

/// Initialization method for the constructors
void Deck::Init(size_t capacity, size_t size){

	this->capacity = capacity;
	this->size = size;
	this->fromFile = false;

	this->internalArr = new Card[this->capacity];

	if (!this->internalArr){
		std::cerr << "Cannot initialize deck!";
	}

}

/// Saves best cards ordered by predicate DESC
void Deck::SaveByDESC(std::ofstream& file, int (Card::*func)() const){
	int temp, max;
	int pos = 0;
	Card tempCard;
	for (size_t i = 0; i < this->numberOfBestCards; i++)
	{
		max = (this->operator[](i).*func)();
		for (size_t j = i; j < this->GetSize(); j++)
		{
			temp = (this->operator[](j).*func)();
			if (temp > max){
				max = temp;
				pos = j;
			}
		}
		file << this->operator[](pos);
		tempCard = this->operator[](i);
		this->operator[](i) = this->operator[](pos);
		this->operator[](pos) = tempCard;
	}
}

/// Saves best cards ordered by Energy ASC
void Deck::SaveByASC(std::ofstream& file, double (Card::*func)() const){
	double temp, max;
	int pos = 0;
	Card tempCard;
	for (size_t i = 0; i < this->numberOfBestCards; i++)
	{
		max = (this->operator[](i).*func)();
		for (size_t j = i; j < this->GetSize(); j++)
		{
			temp = (this->operator[](j).*func)();
			if (temp < max){
				max = temp;
				pos = j;
			}
		}
		file << this->operator[](pos);
		tempCard = this->operator[](i);
		this->operator[](i) = this->operator[](pos);
		this->operator[](pos) = tempCard;
	}
}

/// TODO: Merge SaveByASC and SaveByDESC, template(int/double/char*) predicates?