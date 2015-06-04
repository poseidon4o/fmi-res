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
    { "Pagani Zonda R", 740 },
    { "Bugatti Veyron", 1020 }
};
const int CAR_COUNT = sizeof(CARS) / sizeof(CarInfo);
const int MAX_NAME = 24;

struct CarRecord {
    char name[MAX_NAME];
    int carId;
    unsigned regId;
};

const int MAX_RECORDS = 1000;
CarRecord database[MAX_RECORDS];
int dbSize = 0;

void load();
void save();
void report();

bool addRecord(); // validates and adds new record
bool hasRegId(int regId); // checks if there is a record with given regId

void menu();

int main() {
    load();
    menu();
    return 0;
}

void menu() {
    while (true) {
        cout
            << "1. Make report\n"
            << "2. Add record\n"
            << "3. Save database\n"
            << "4. Load database\n"
            << "5. Quit\n"
            << "Option=";
        int option;
        cin >> option;
        switch (option)
        {
        case 1:
            report();
            break;
        case 2:
            addRecord();
            break;
        case 3:
            save();
            break;
        case 4:
            load();
            break;
        case 5:
            return;
        }
    }
}

bool hasRegId(int regId) {
    for (int c = 0; c < dbSize; ++c) {
        if (database[c].regId == regId) {
            return true;
        }
    }
    return false;
}

bool addRecord() {
    if (dbSize == MAX_RECORDS) {
        cerr << "Max number of records reached\n";
        return false;
    }

    cout << "Input regNumber, carId, ownerName\n";
    CarRecord record;
    cin >> record.regId >> record.carId;
    cin.getline(record.name, MAX_NAME);
    if (record.carId < 0 || record.carId > CAR_COUNT) {
        cerr << "Invalid car id\n";
        return false;
    }
    if (record.regId < 0 || record.regId > 9999) {
        cerr << "Invalid reg id [0, 9999]\n";
        return false;
    }

    if (hasRegId(record.regId)) {
        cerr << "Already a record with given reg id\n";
        return false;
    }

    database[dbSize++] = record;
    return true;
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

void report() {
    ofstream reportFile("car-report.txt", ios::out | ios::trunc);
    if (!reportFile) {
        cerr << "Failed to create report file";
        exit(1);
    }
    int carPopularity[CAR_COUNT] = { 0, };
    double avgHP = 0.0;

    for (int c = 0; c < dbSize; ++c) {
        ++carPopularity[database[c].carId];
        avgHP += CARS[database[c].carId].hp;
    }
    avgHP /= dbSize;

    // find max index of carPopularity
    int maxId = 0, maxVal = carPopularity[0];
    for (int c = 1; c < CAR_COUNT; ++c) {
        if (carPopularity[c] > maxVal) {
            maxVal = carPopularity[c];
            maxId = c;
        }
    }

    reportFile
        << "Most popular car " << CARS[maxId].name << "\n"
        << "Average HP: " << avgHP << "\n";

    // array that will hold all owners, so we dont repeat lines
    const char * processedOwners[MAX_RECORDS] = { NULL, };
    int ownersCount = 0;

    for (int c = 0; c < dbSize; ++c) {
        const char * currentOwner = database[c].name;

        bool skip = false;
        for (int r = 0; r < ownersCount; ++r) {
            if (!strcmp(currentOwner, processedOwners[r])) {
                // we have found owner, skip him
                skip = true;
                break;
            }
        }
        if (skip) {
            continue;
        }

        processedOwners[ownersCount++] = currentOwner;
        int totalHp = 0;
        for (int r = 0; r < dbSize; ++r) {
            if (!strcmp(currentOwner, database[c].name)) {
                totalHp += CARS[database[c].carId].hp;
            }
        }

        reportFile << "Name: " << currentOwner << " total HP: " << totalHp << "\n";
    }
}