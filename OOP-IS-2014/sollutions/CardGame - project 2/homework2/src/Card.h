#include <fstream>
#include <iostream>
///
/// Represents a card
///
class Card{
public:
	/// Getter for current attack
	int GetCurrentAttack() const;

	/// Getter for current life
	int GetCurrentLife() const;

	/// Energy needed to play card
	double GetEnergy() const;

	/// Getter for card name
	char* GetName() const;

	/// Setter for current attack
	void SetCurrentAttack(const int);

	/// Setter for current life
	void SetCurrentLife(const int);

	/// Setter fot card name
	void SetName(const char*);

	/// Constructors and destructor
	Card();
	Card(const Card&);
	Card& operator=(const Card&);
	Card(const int, const int, const char*);
	Card(std::ifstream&);
	~Card();

	/// Saves a card in a stream (binary)
	void SaveToFile(std::ofstream& file);

private:
	/// Card's initial attack
	int initialAttack;

	/// Card's initial life
	int initialLife;

	/// Card's current attack
	int currentAttack;

	/// Card's current life
	int currentLife;

	/// Card's name
	char* name;

	/// Initialization method used in constructors
	void Init(const int, const int, const char*);
};
