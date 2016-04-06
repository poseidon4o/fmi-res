#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

typedef short id_t;

const id_t INVALID_ID = -1;
const id_t ID_COUNT = (1 << 15) - 1;
const int DECK_SIZE = 60;
const int CARD_NAME_SIZE = 64;
const int PLAYER_NAME_SIZE = 128;

id_t NEXT_PLAYER_ID = 0;
id_t NEXT_CARD_ID = 0;

const char * deck_file_name = "decks.dat";
const char * player_file_name = "players.dat";
const char * card_file_name = "cards.dat";
const char * report_file_name = "report.txt";

enum Color {
	Red, Black, Blue, White, Green, ColorCount, Invalid = -1
};

const char * color_names[ColorCount] = {"Red", "Black", "Blue", "White", "Green"};

struct Player {
	id_t id;
	char name[PLAYER_NAME_SIZE];
};

struct Card {
	id_t id;
	char name[CARD_NAME_SIZE];
	Color color;
};

struct Deck {
	id_t owner;
	Color color;
	id_t cards[DECK_SIZE];
};


Player * players = nullptr;
int player_count = 0;
Card * cards = nullptr;
int card_count = 0;

// returns false only if program can't continue
bool load();
// returns false on any error
bool save();

// commands
void create_deck();
void create_card();
void create_player();
void report();

// helpers
Color name_to_color(const char * color_name);
bool load_players();
bool load_cards();
bool save(const char * file_name, const char * data, int size);

int main() {
	if (load()) {
		cerr << "Failed while loading players & cards" << endl;
	}

	char cmd[8];
	while (cin >> cmd) {
		if (!strcmp(cmd, "cd")) {
			create_deck();
		} else if (!strcmp(cmd, "cc")) {
			create_card();
		} else if (!strcmp(cmd, "cp")) {
			create_player();
		} else if (!strcmp(cmd, "report")) {
			report();
		} else {
			cerr << "Command " << cmd << " not recognised - quitting" << endl;
			break;
		}
	}

	if (!save()) {
		cerr << "Failed to save files" << endl;
	}
	return 0;
}

////////////////////////
/// Loading & Saving
////////////////////////

bool load() {
	return load_players() && load_cards();	
}

bool save() {
	bool result = save(player_file_name, (const char*)players, player_count) && save(card_file_name, (const char *)cards, card_count);
	delete[] cards;
	delete[] players;
	return result;
}

////////////////////////
/// Commands
////////////////////////

void create_deck() {
	Player owner = {INVALID_ID, ""};
	Card input_cards[DECK_SIZE];
	for (int c = 0; c < 60; ++c) {
		input_cards[c].id = INVALID_ID;
		input_cards[c].color = Invalid;
	}

	Deck deck = {owner.id, Invalid, {0, }};

	// input
	cin.ignore();
	cin.getline(owner.name, PLAYER_NAME_SIZE);
	for (int c = 0; c < DECK_SIZE; ++c) {
		cin.getline(input_cards[c].name, CARD_NAME_SIZE);
	}

	// check owner
	for (int c = 0; c < player_count; ++c) {
		if (!strcmp(owner.name, players[c].name)) {
			owner = players[c];
			break;
		}
	}

	if (owner.id == INVALID_ID) {
		cerr << "Failed to find owner" << endl;
		return;
	}
	deck.owner = owner.id;

	// check cards
	int color_histogram[ColorCount] = {0, };
	for (int c = 0; c < DECK_SIZE; ++c) {
		for (int r = 0; r < card_count; ++r) {
			if (!strcmp(input_cards[c].name, cards[r].name)) {
				input_cards[c] = cards[r]; // get color & id
			}
		}
		if (input_cards[c].id == INVALID_ID) {
			cerr << "Failed to find card " << input_cards[c].name << endl;
			return;
		}

		color_histogram[input_cards[c].color]++;
		deck.cards[c] = input_cards[c].id;
	}

	// find most common color
	deck.color = Red;
	for (int c = 0; c < ColorCount; ++c) {
		if (color_histogram[deck.color] < color_histogram[c]) {
			deck.color = (Color)c;
		}
	}

	// append to file
	ofstream deck_file(deck_file_name, ios::binary | ios::app | ios::ate);
	if (!deck_file.write((const char *)&deck, sizeof(deck))) {
		cerr << "Failed to add new deck" << endl;
		return;
	}
}

void create_card() {
	char color_name[16], card_name[2048];
	Card card = {INVALID_ID, "", Invalid};

	// input
	cin >> color_name;
	cin.ignore();
	cin.getline(card_name, 2048);
	strncpy(card.name, card_name, CARD_NAME_SIZE);
	card.name[CARD_NAME_SIZE - 1] = 0;
	card.color = name_to_color(color_name);

	// check for existence
	for (int c = 0; c < card_count; ++c) {
		if (!strcmp(card.name, cards[c].name)) {
			card.id = cards[c].id;
			break;
		}
	}

	if (card.id != INVALID_ID || card.color == Invalid) {
		cerr << "Failed to create card" << endl;
		return;
	}

	// allocate
	Card * new_cards = new (nothrow)Card[card_count + 1];
	if (!new_cards) {
		cerr << "Failed to create card" << endl;
		return;
	}
	memcpy(new_cards, cards, card_count * sizeof(Card));
	// add new player
	card.id = NEXT_CARD_ID++;
	new_cards[card_count++] = card;

	delete[] cards;
	cards = new_cards;
}

void create_player() {
	Player player = {INVALID_ID, ""};

	// input
	cin.ignore();
	cin.getline(player.name, PLAYER_NAME_SIZE);

	// check for existence
	for (int c = 0; c < player_count; ++c) {
		if (!strcmp(player.name, players[c].name)) {
			player.id = players[c].id;
			break;
		}
	}

	if (player.id != INVALID_ID) {
		cerr << "Failed to create player" << endl;
		return;
	}

	// allocate space
	Player * new_players = new (nothrow)Player[player_count + 1];
	if (!new_players) {
		cerr << "Failed to create player" << endl;
		return;
	}

	memcpy(new_players, players, player_count * sizeof(Player));
	// add new player
	player.id = NEXT_PLAYER_ID++;
	new_players[player_count++] = player;
	
	delete[] players;
	players = new_players;
}

void report() {
	ifstream deck_file(deck_file_name, ios::binary | ios::ate);

	int deck_count = deck_file.tellg() / sizeof(Deck);
	deck_file.seekg(0, ios::beg);

	Deck * decks = nullptr;
	if (deck_count > 0) {
		decks = new (nothrow)Deck[deck_count];
		if (!decks) {
			deck_count = 0;
		} else {
			deck_file.read((char*)decks, deck_count * sizeof(Deck));
		}
	}

	// will be used to count card occurances in decks card_histogram[CARD_ID]
	int * card_histogram = new (nothrow)int[ID_COUNT];
	if (!card_histogram) {
		delete[] decks;
		return;
	}
	// init with 0's
	memset(card_histogram, 0, ID_COUNT * sizeof(int));

	ofstream report_file(report_file_name, ios::trunc);
	if (!report_file) {
		cerr << "Failed to create report";
		delete[] decks;
		delete[] card_histogram;
		return;
	}

	// for each color output it's decks
	for (Color color = Red; color < ColorCount; color = (Color)(color + 1)) {
		report_file << color_names[color] << endl;
		for (int c = 0; c < deck_count; ++c) {
			const Deck & deck = decks[c];

			if (deck.color != color) {
				continue;
			}

			// find owner
			Player pl = {deck.owner, ""};
			for (int r = 0; r < player_count; ++r) {
				if (players[r].id == pl.id) {
					pl = players[r];
					break;
				}
			}

			// find first card
			Card first_card = {deck.cards[0], "", Invalid};
			for (int r = 0; r < card_count; ++r) {
				if (first_card.id == cards[r].id) {
					first_card = cards[r];
					break;
				}
			}

			// update histogram
			for (int r = 0; r < DECK_SIZE; ++r) {
				card_histogram[deck.cards[r]]++;
			}

			report_file << "    Owner: " << pl.name << " Card: " << first_card.name << " Color: " << color_names[first_card.color] << endl;
		}
	}

	// find max in card_histogram and find the corresponding Card
	Card most_common = {0, "", Invalid};
	for (id_t c = 0; c < ID_COUNT; ++c) {
		if (card_histogram[most_common.id] < card_histogram[c]) {
			most_common.id = c;
		}
	}

	for (int c = 0; c < card_count; ++c) {
		if (most_common.id == cards[c].id) {
			most_common = cards[c];
		}
	}

	if (most_common.id == INVALID_ID) {
		report_file << "Most common card:  Color: Red";
	} else {
		report_file << "Most common card: " << most_common.name << " Color: " << color_names[most_common.color];
	}

	// clean up
	delete[] decks;
	delete[] card_histogram;
}


////////////////////////
/// Helpers
////////////////////////

Color name_to_color(const char * color_name) {
	for (int c = 0; c < ColorCount; ++c) {
		if (!strcmp(color_names[c], color_name)) {
			return (Color)c;
		}
	}
	return Invalid;
}

bool load_players() {
	fstream player_file(player_file_name, ios::in | ios::binary);
	if (!player_file) {
		return true;
	}

	// get count
	player_file.seekg(0, ios::end);
	player_count = player_file.tellg() / sizeof(Player);
	players = new (nothrow) Player[player_count];
	if (!players) {
		delete[] players;
		player_count = 0;
		return false;
	}

	// read
	player_file.seekg(0, ios::beg);
	if (!player_file.read((char*)players, player_count * sizeof(Player))) {
		delete[] players;
		player_count = 0;
		return false;
	}
	
	// init next id
	for (int c = 0; c < card_count; ++c) {
		NEXT_PLAYER_ID = max(NEXT_PLAYER_ID, cards[c].id);
	}
	++NEXT_PLAYER_ID;

	return true;
}

bool load_cards() {
	fstream cards_file(card_file_name, ios::in | ios::binary);
	if (!cards_file) {
		return true;
	}

	// get count
	cards_file.seekg(0, ios::end);
	card_count = cards_file.tellg() / sizeof(Player);
	cards = new (nothrow) Card[card_count];
	if (!cards) {
		delete[] cards;
		card_count = 0;
		return false;
	}

	// read
	cards_file.seekg(0, ios::beg);
	if (!cards_file.read((char*)cards, card_count * sizeof(Player))) {
		delete[] cards;
		card_count = 0;
		return false;
	}

	// init next id
	for (int c = 0; c < card_count; ++c) {
		NEXT_CARD_ID = max(NEXT_CARD_ID, cards[c].id);
	}
	++NEXT_CARD_ID;

	return true;
}

bool save(const char * file_name, const char * data, int size) {
	ofstream file(file_name, ios::out | ios::trunc | ios::binary);
	if (!file) {
		return false;
	}

	if (!file.write(data, size)) {
		return false;
	}

	return true;
}