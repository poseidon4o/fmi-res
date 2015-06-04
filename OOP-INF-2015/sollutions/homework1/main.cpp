#include <iostream>
#include <fstream>

using namespace std;

struct CarInfo {
    const char * name;
    int hp;
};

const CarInfo CARS[] = {
    { "Lambordgini Murcielago", 670 },
    { "Mercedes - AMG", 503 },
    { "Pagani Zonda R", 740},
    { "Bugatti Veyron", 1020}
};

struct CarRecord {
    char name[24];
    int carId;
    unsigned regId;
};

const int MAX_RECORDS = 1000;
CarRecord database[MAX_RECORDS];
int dbSize = 0;

void load();
void save();
void report();
void addRecord();
void menu();

int main() {
    load();
    return 0;
}


void load() {
    ifstream dbFile("db-save.dat", ios::in | ios::binary);
    if (!dbFile) {
        return;
    }

    int saveSize = 0;
    if (!dbFile.read((char*)&saveSize, sizeof(saveSize))) {
        cerr << "Failed to read save file";
        exit(1);
    }

    if (!dbFile.read((char*)database, saveSize * sizeof(CarRecord))) {
        cerr << "Failed to load records - failed to read";
        exit(1);
    }
    dbSize = saveSize;
}

void save() {
    ofstream dbFile("db-save.dat", ios::trunc | ios::binary);
    if (!dbFile) {
        cerr << "Failed to create save file";
        exit(1);
    }

    if (!dbFile.write((const char *)&dbSize, sizeof(dbSize))) {
        cerr << "Failed to write to save file";
        exit(1);
    }

    if (!dbFile.write((const char*)database, dbSize * sizeof(CarRecord))) {
        cerr << "Failed to write to save file";
        exit(1);
    }

    dbFile.close();
}
