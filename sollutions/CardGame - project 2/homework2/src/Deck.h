#include "Card.h"

///
/// Represents a deck of cards
///
class Deck{
public:
	/// Sort type for SaveBest
	static const enum Criterion { Attack, Life, Energy, Name };

	/// Getter for deck size
	size_t GetSize() const;

	/// Is the deck valid (has more than 5 cards)
	bool IsValid() const;

	/// Constructors and destructor
	Deck();
	Deck& operator=(const Deck&);
	Deck(const Deck&);
	Deck(const char*);
	~Deck();

	/// Adds a card to the end of the deck
	void AddCard(const Card&);

	/// Takes a card from the deck
	//Card& CardAt(size_t);
	Card& operator[](size_t);

	/// Clears the deck
	void Clear();

	/// Reorders the cards in the deck
	void Shuffle();

	/// Saves the deck to file
	void SaveToFile(const char*);

	/// Saves best cards by given parameters
	void SaveBest(Criterion, const char*);

private:
	/// Constants
	static const int numberOfBestCards = 5;
	static const int initialCapacity = 10;

	/// Meta data
	bool fromFile;
	char* fileName;

	/// Internal array representation
	Card* internalArr;

	/// Deck size
	size_t size;

	/// Max size
	size_t capacity;

	/// Extend the array if size == maxSize and a Card is added
	void ExtendInternal();

	/// Shrink the array if size <= maxSize / 4
	void ShrinkInternal();

	/// Initializer used by the constructors
	void Init(size_t, size_t);

	/// Saves best cards by Attack in a text file
	void SaveByDESC(std::ofstream&, int (Card::*func)() const);
	void SaveByASC(std::ofstream&, double (Card::*func)() const);

	/// Saves best cards by Life in a text file
	void SaveByLife(std::ofstream&);

	/// Saves best cards by Energy
	void SaveByEnergy(std::ofstream&);
};
