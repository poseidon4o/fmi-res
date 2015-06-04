#include "Header.h"

///
/// All members of the Card class
///

/// Getter for current attack
int Card::GetCurrentAttack() const{
	return this->currentAttack;
}

/// Getter for current life
int Card::GetCurrentLife() const{
	return this->currentLife;
}

/// Energy needed to play card
/// Energy = Life / 100 + Attack / 20
double Card::GetEnergy() const{
	return 
		((double)this->GetCurrentLife() / 100) + 
		((double)this->GetCurrentAttack() / 20);
}

/// Getter for card name
char* Card::GetName() const{
	char* returnName = new char[strlen(this->name)+1];
	strcpy(returnName, this->name);
	return returnName;
}

/// Setter for current attack
void Card::SetCurrentAttack(const int attack){
	this->currentAttack = attack;
}

/// Setter for current life
void Card::SetCurrentLife(const int life){
	this->currentLife = life;
}

/// Setter for card name
void Card::SetName(const char* name){
	if (this->name != NULL) 
		delete this->name;
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}

/// Default constructor
/// Parameters:
///		Life	= -1
///		Attack	= -1
///		Name	= ""
Card::Card(){
	this->Init(-1, -1, " ");
}

/// Constructor
/// Card (initialLife, initialAttack, Name)
Card::Card(const int life, const int attack, const char* name){
	this->Init(attack, life, name);
}

/// Copy constructor
Card::Card(const Card & another){
	this->Init(another.GetCurrentAttack(), another.GetCurrentLife(), another.GetName());
}

/// Operator= constructor
Card& Card::operator=(const Card & another){
	if (this != &another)
		this->Init(another.GetCurrentAttack(), another.GetCurrentLife(), another.GetName());
	
	return *this;
}

/// Binary data constructor
Card::Card(std::ifstream& file){
	int nameLen = 0;

	file.read((char*)&this->initialAttack, sizeof(int));
	file.read((char*)&this->currentAttack, sizeof(int));
	file.read((char*)&this->initialLife, sizeof(int));
	file.read((char*)&this->currentLife, sizeof(int));
	file.read((char*)&nameLen, sizeof(int));
	this->name = new char[nameLen+1];
	file.read((char*)this->name, nameLen);
	this->name[nameLen] = '\0';
	if (!file){
		std::cerr << "File not read correctly!";
	}
}

/// Destructor
Card::~Card(){
	delete this->name;
}

/// Initialization method used by constructors
/// Init (initialAttack, initialLife, Name)
void Card::Init(const int attack, const int life, const char* name){
	
	this->initialAttack = attack;
	this->initialLife = life;

	this->SetCurrentAttack(this->initialAttack);
	this->SetCurrentLife(this->initialLife);
	this->name = NULL;
	this->SetName(name);
}

/// Saves a card to binary file
void Card::SaveToFile(std::ofstream& file){
	file.write((char*)&this->initialAttack, sizeof(this->initialAttack));
	file.write((char*)&this->currentAttack, sizeof(this->currentAttack));
	file.write((char*)&this->initialLife, sizeof(this->initialLife));
	file.write((char*)&this->currentLife, sizeof(this->currentLife));
	int nameLen = strlen(this->name);
	file.write((char*)&nameLen, sizeof(nameLen));
	file.write((char*)this->name, nameLen);

	if (!file){
		std::cerr << "Card not saved correctly!";
	}

}
