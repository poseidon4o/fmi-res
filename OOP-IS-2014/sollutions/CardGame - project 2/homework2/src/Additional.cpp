#include "Card.h"

/// Additional overloads
std::ostream& operator<<(std::ostream& stream, const Card& card){
	stream << "Card: " << card.GetName() << std::endl;
	stream << "\tAttack: " << card.GetCurrentAttack() << std::endl;
	stream << "\tLife: " << card.GetCurrentLife() << std::endl;
	stream << "\tEnergy: " << card.GetEnergy() << std::endl;

	return stream;
}