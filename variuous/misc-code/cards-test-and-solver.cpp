#define _CRT_SECURE_NO_WARNINGS
#include <random>
#include <cstdint>
#include <string>
#include <limits>
#include <cstdio>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <functional>

using namespace std;

enum class Color { Lowest, Red = Lowest, Black, Blue, White, Green, Biggest = Green, Invalid = 100 };
const char * ColorNames[5] = { "Red", "Black", "Blue", "White", "Green" };

typedef uint16_t id_t;

struct Player {
    id_t id;
    char name[128];
};

struct Deck {
    id_t ownerId;
    Color color;
    id_t cards[60];
};

struct Card {
    id_t id;
    char name[64];
    Color color;
};

id_t NID = 0; // next id

enum class DeckErr { None, InvalidPlayer, InvalidCard };

struct {
    unordered_map<id_t, Card> m_cards;
    unordered_map<id_t, Player> m_players;
    vector<Deck> m_decks[5];

    unordered_map<string, id_t> player_to_id;
    unordered_map<string, id_t> card_to_id;

    fstream input, output, interleaved;

    void setOuts(const string & in, const string & out, const string & inter) {
        input = fstream(in, ios::out | ios::trunc);
        output = fstream(out, ios::out | ios::trunc);
        interleaved = fstream(inter, ios::out | ios::trunc);
    }

    operator bool() {
        return input && output && interleaved;
    }

    void clear() {
        m_cards.clear();
        m_players.clear();

        fill(m_decks, m_decks + 5, vector<Deck>());

        player_to_id.clear();
        card_to_id.clear();
    }

    bool playerExists(const char * name) {
        return player_to_id.find(name) != player_to_id.end();
    }

    bool cardExists(const char * name) {
        return card_to_id.find(name) != card_to_id.end();
    }

    Color nameToColor(const char * name) {
        for (int c = 0; c < 5; ++c) {
            if (!strcmp(name, ColorNames[c])) {
                return static_cast<Color>(c);
            }
        }
        return Color::Invalid;
    }

    void addPlayer(const char * name) {
        input << "cp" << endl << name << endl;
        interleaved << "cp" << endl << name << endl;

        if (!playerExists(name)) {
            Player pl = { NID++,{ 0, } };
            strncpy(pl.name, name, 127);
            m_players[pl.id] = pl;
            player_to_id[name] = pl.id;
            output << ">Player \"" << name << "\" created" << endl;
            interleaved << ">Player \"" << name << "\" created" << endl;
        } else {
            output << ">Player \"" << name << "\" already exists" << endl;
            interleaved << ">Player \"" << name << "\" already exists" << endl;
        }
    }

    void addCard(const char * name, Color color) {
        input << "cc" << endl << ColorNames[static_cast<int>(color)] << ' ' << name << endl;
        interleaved << "cc" << endl << ColorNames[static_cast<int>(color)] << ' ' << name << endl;

        if (!cardExists(name)) {
            Card card = { NID++,{ 0, }, color };
            strncpy(card.name, name, 63);
            m_cards[card.id] = card;
            card_to_id[card.name] = card.id;
            output << ">Card \"" << name << "\" created" << endl;
            interleaved << ">Card \"" << name << "\" created" << endl;
        } else {
            output << ">Card \"" << name << "\" already exists" << endl;
            interleaved << ">Card \"" << name << "\" already exists" << endl;
        }
    }


    DeckErr makeDeck(const char * ownerName, const char * cards[60]) {
        input << "cd" << endl << ownerName << endl;
        interleaved << "cd" << endl << ownerName << endl;
        for (int c = 0; c < 60; ++c) {
            input << cards[c] << endl;
            interleaved << cards[c] << endl;
        }

        auto playerId = player_to_id.find(ownerName);
        if (playerId == player_to_id.end()) {
            output << ">Cant create deck for player \"" << ownerName << "\" - player doesnt exist" << endl;
            interleaved << ">Cant create deck for player \"" << ownerName << "\" - player doesnt exist" << endl;
            return DeckErr::InvalidPlayer;
        }

        int colors[5] = { 0, };
        id_t card_ids[60] = { 0, };
        for (int c = 0; c < 60; ++c) {
            auto cardId = card_to_id.find(cards[c]);
            if (cardId == card_to_id.end()) {
                output << ">Cant create deck - card \"" << cards[c] << "\" does not exist" << endl;
                interleaved << ">Cant create deck - card \"" << cards[c] << "\" does not exist" << endl;
                return DeckErr::InvalidCard;
            }
            card_ids[c] = cardId->second;
            colors[static_cast<int>(m_cards[cardId->second].color)]++;
        }

        Deck dck = { playerId->second, static_cast<Color>(std::max_element(begin(colors), colors + 5) - colors),{ 0, } };
        memcpy(dck.cards, card_ids, 60 * sizeof(id_t));
        m_decks[static_cast<int>(dck.color)].push_back(dck);

        output << ">Deck created for player \"" << ownerName << "\"" << endl;
        interleaved << ">Deck created for player \"" << ownerName << "\"" << endl;
        return DeckErr::None;
    }

    bool report(const string & name = "") {
        input << "report" << endl;
        interleaved << "report" << endl;
        ofstream report(name.empty() ? "report" : name.c_str(), ios::trunc);
        if (!report) {
            return false;
        }

        vector<int> hist(numeric_limits<id_t>::max(), 0);

        for (int c = 0; c < 5; ++c) {
            report << ColorNames[c] << endl;
            for (const auto & deck : m_decks[c]) {
                for (int r = 0; r < 60; r++) {
                    hist[deck.cards[r]]++;
                }
                const auto & card = m_cards[deck.cards[0]];
                const auto & player = m_players[deck.ownerId];

                report << "\tOwner: " << player.name << " Card: " << card.name << " Color: " << ColorNames[static_cast<int>(card.color)] << endl;
            }
        }

        const auto cardId = max_element(hist.begin(), hist.end()) - hist.begin();
        const auto & card = m_cards[cardId];

        report << "Most common card: " << card.name << " Color: " << ColorNames[static_cast<int>(card.color)];
        output << ">report generated" << endl;
        interleaved << ">report generated" << endl;
        return !!report;
    }

    void end() {
        input << "quit" << endl;
        interleaved << "quit" << endl;
        output << ">command \"quit\" not recognized - exiting" << endl;
        interleaved << ">command \"quit\" not recognized - exiting" << endl;
    }
} Solution;


int main() {
    vector<pair<Color, string>> cards;
    vector<string> players;

    ifstream data("d:/cards.txt");

    while (data) {
        string color, name;
        data >> color;
        auto cardColor = Solution.nameToColor(color.c_str());
        if (cardColor < Color::Lowest || cardColor > Color::Biggest) {
            continue;
        }
        getline(data, name);
        name.erase(0, name.find_first_not_of(' '));
        name.erase(name.find_last_not_of(' ') + 1);
        cards.push_back(make_pair(cardColor, name));
    }

    for (int c = 0; c < 200; ++c) {
        char name[128] = { 0. };
        sprintf(name, "Player #%d", c);
        players.push_back(name);
    }

    default_random_engine eng(42);
    uniform_int_distribution<int> cardDist(0, cards.size() - 1);
    uniform_int_distribution<int> plDist(0, players.size() - 1);

    auto getPlayer = [&]() -> const string {
        return players[plDist(eng)];
    };

    auto getCard = [&]() -> const pair<Color, string> {
        return cards[cardDist(eng)];
    };

    // slow
    auto exisingCard = [&]() -> const string {
        uniform_int_distribution<int> cDist(0, Solution.m_cards.size() - 1);
        auto iter = Solution.m_cards.begin();
        advance(iter, cDist(eng));
        return iter->second.name;
    };

    auto existingPlayer = [&]() -> const string {
        uniform_int_distribution<int> pDist(0, Solution.m_players.size() - 1);
        auto iter = Solution.m_players.begin();
        advance(iter, pDist(eng));
        return iter->second.name;
    };

    // reaaaaly slow
    auto existingDeck = [&]() -> const vector<string> {
        vector<string> res;
        for (int c = 0; c < 60; ++c) {
            res.push_back(exisingCard());
        }
        return res;
    };

    // tests, atlast!!
    vector<pair<string, function<void(void)>>> tests = {
        {"empty", [&] {
        }},
        {"minimal", [&] {
            for (int c = 0; c < 60; ++c) {
                const auto & card = getCard();
                Solution.addCard(card.second.c_str(), card.first);
            }
            Solution.addPlayer(getPlayer().c_str());
        }},
        {"duplicate-player", [&] {
            const auto & pl = getPlayer();
            Solution.addPlayer(pl.c_str());
            Solution.addPlayer(pl.c_str());
        }},
        {"duplicate-card", [&] {
            const auto & card = getCard();
            Solution.addCard(card.second.c_str(), card.first);
            Solution.addCard(card.second.c_str(), card.first);
        }},
        {"medium", [&] {
            for (int c = 0; c < 200; ++c) {
                const auto & card = getCard();
                Solution.addCard(card.second.c_str(), card.first);
            }
            for (int c = 0; c < 4; ++c) {
                Solution.addPlayer(getPlayer().c_str());
            }

            for (int c = 0; c < 8; ++c) {
                const char * cards[60];

                // we will need the ptrs to persist for a bit
                vector<string> cardNames = existingDeck();
                transform(cardNames.begin(), cardNames.end(), cards, mem_fn(&string::c_str));

                Solution.makeDeck(existingPlayer().c_str(), cards);
            }
        }},
        { "big", [&] {
            for (int c = 0; c < 2000; ++c) {
                const auto & card = getCard();
                Solution.addCard(card.second.c_str(), card.first);
            }

            for (int c = 0; c < 20; ++c) {
                Solution.addPlayer(getPlayer().c_str());
            }

            for (int c = 0; c < 40; ++c) {
                const char * cards[60];

                // we will need the ptrs to persist for a bit
                vector<string> cardNames = existingDeck();
                transform(cardNames.begin(), cardNames.end(), cards, mem_fn(&string::c_str));

                Solution.makeDeck(existingPlayer().c_str(), cards);
            }
        }},
        {"all", [&] {
            for (auto & c : cards) {
                Solution.addCard(c.second.c_str(), c.first);
            }

            for (auto & p : players) {
                Solution.addPlayer(p.c_str());
            }

            for (auto & pl : Solution.m_players) {
                // 4 decks per player
                for (int c = 0; c < 4; ++c) {
                    const char * cards[60];

                    // we will need the ptrs to persist for a bit
                    vector<string> cardNames = existingDeck();
                    transform(cardNames.begin(), cardNames.end(), cards, mem_fn(&string::c_str));

                    Solution.makeDeck(pl.second.name, cards);
                }
            }
        }}
    };

    // run em
    for (auto & test : tests) {
        auto name = "D:/tests/" + test.first;
        auto run = test.second;

        Solution.clear();
        Solution.setOuts(name + "-input.txt", name + "-output.txt", name + "-interleaved.txt");
        if (!Solution) {
            cout << "Solution failed for test " << name;
            continue;
        }

        run();

        if (!Solution.report(name + "-report.txt")) {
            cout << "Report failed for " << name << endl;
        } else {
            cout << "Test " << name << " generated..." << endl;
        }
        Solution.end();
    }

    cin.get();
}