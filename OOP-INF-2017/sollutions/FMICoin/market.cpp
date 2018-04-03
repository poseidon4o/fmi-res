#include "market.h"

#include <cassert>
#include <iostream>
#include <algorithm>

/// File name to save/load wallet info
static const char * WALLET_FILE = "wallets.dat";

/// File name to save/load transactions info
static const char * TRANSACTION_FILE = "transactions.dat";

/// File name to save/load orders info
static const char * ORDER_FILE = "orders.dat";

/// Wallet id used to seed money into new wallets
static const unsigned SYSTEM_WALLET_ID = -1;

/// This is used to generate ids for new wallets
static unsigned NEXT_WALLET_ID = 0;

/// Static convertion rate between FMICoins and fiat money
static const double CONVERTION_RATE = 375.0;

/// Assume the acceptable epsilon for fiat money is 1% of 1 cent
static const double FIAT_EPS = (0.01 / 100.0) / CONVERTION_RATE;

/// Keep the acceptable epsilon at the same absolute value
static const double COIN_EPS = FIAT_EPS * CONVERTION_RATE;

/// Check if value of coins is acceptably zero
/// @param coins - the amount of coins
/// @return - true if the amount is "zero"
static bool isCoinsZero(double coins) {
    return coins >= -COIN_EPS && coins <= COIN_EPS;
}

/// Double capacity or init it to some constant
/// @param current - the current capacity
/// @return - new capacity
static int doubleOrInitCapacity(int current) {
    if (current) {
        return current * 2;
    }
    return 16;
}

/// Load data from a file
/// @param file - ref to open file to load all data from
/// @param loadedData [out] - ref to pointer for data to be stored
/// @param count [out] - ref to number of loaded items
/// @return true on success, false otherwise
static bool loadDataFromFile(std::fstream & file, char *& loadedData, int & count) {
    file.seekg(0, std::ios::end);
    const int bytesInFile = file.tellg();
    if (!file.is_open() || bytesInFile == 0) {
        count = 0;
        loadedData = new(std::nothrow) char[0];
        return loadedData != nullptr;
    }

    loadedData = new(std::nothrow) char[bytesInFile];
    if (!loadedData) {
        return false;
    }

    file.seekg(0, std::ios::beg);
    file.read(loadedData, bytesInFile);
    count = file.gcount();
    assert(file.gcount() == bytesInFile && "Failed to read full file");
    return true;
}

/// Load wallets from file and init NEXT_WALLET_ID
/// @param market - ref to market context
/// @return - true on success, false otherwise
static bool loadWallets(Market & market) {
    char *& readPtr = reinterpret_cast<char *&>(market.data.wallets);
    int fileSize = 0;
    if (!loadDataFromFile(market.walletFile, readPtr, fileSize)) {
        market.criticalError = true;
        return false;        
    }
    assert(fileSize % sizeof(Wallet) == 0 && "Corrupt data in wallets file");
    market.data.walletCapacity = market.data.walletCount = fileSize / sizeof(Wallet);

    market.walletFile.close();
    market.walletFile.clear();
    // we will modify and add wallets, so truncate the file to write everything again
    market.walletFile.open(WALLET_FILE, std::ios::out | std::ios::binary | std::ios::trunc);

    if (market.data.walletCount) {
        NEXT_WALLET_ID = market.data.wallets[market.data.walletCount - 1].id + 1;
    }

    return market.walletFile.is_open();
}

/// Load transaction from file
/// @param market - ref to market context
/// @return - true on success, false otherwise
static bool loadTransactions(Market & market) {
    char *& readPtr = reinterpret_cast<char *&>(market.data.transactions);
    int fileSize = 0;
    if (!loadDataFromFile(market.transactionFile, readPtr, fileSize)) {
        market.criticalError = true;
        return false;        
    }
    assert(fileSize % sizeof(Transaction) == 0 && "Corrupt data in transactions file");
    market.data.transactionCapacity = market.data.transactionCount = fileSize / sizeof(Transaction);
    market.data.initialTransactionCount = market.data.transactionCount; // save this so we can write only new ones

    market.transactionFile.close();
    // we only create transactions, so we will append
    market.transactionFile.open(TRANSACTION_FILE, std::ios::out | std::ios::binary | std::ios::app);

    return market.transactionFile.is_open();
}

/// Load orders from file
/// @param market - ref to market context
/// @return - true on success, false otherwise
static bool loadOrders(Market & market) {
    char *& readPtr = reinterpret_cast<char *&>(market.data.orders);
    if (!loadDataFromFile(market.orderFile, readPtr, market.orderSize)) {
        market.criticalError = true;
        return false;        
    }
    assert(market.orderSize % sizeof(Order) == 0 && "Corrupt data in transactions file");
    market.data.orderCapacity = market.data.orderCount = market.orderSize / sizeof(Order);

    market.orderFile.close();
    // orders will likely change completely, so we will truncate existing ones
    market.orderFile.open(ORDER_FILE, std::ios::out | std::ios::binary | std::ios::trunc);

    return market.orderFile.is_open();
}

/// Save data in file, starting from the beggining of the file
/// @param file - ref to opened file
/// @param data - pointer to the data in memory
/// @param size - number of bytes to write
/// @return - true on success, false otherwise
static bool saveData(std::fstream & file, const char * data, int size) {
    file.seekp(0, std::ios::beg);
    file.write(data, size);
    file.flush();
    return !!file;
}

/// Allocate space for atleast @count new transactions in market
/// @param market - ref to market context
/// @param count - number of new transactions
/// @return - true on success, false otherwise
static bool ensureTransactionSpace(Market & market, int count) {
    int cap = market.data.transactionCapacity;
    
    // we have enough space
    if (market.data.transactionCount + count < cap) {
        return true;
    }

    // double capacity untill we can have this many new transactions
    do {
        cap = doubleOrInitCapacity(cap);
    } while (market.data.transactionCount + count >= cap);

    Transaction * newTransact = new(std::nothrow) Transaction[cap];
    if (!newTransact) {
        market.criticalError = true;
        return false;
    }

    memcpy(newTransact, market.data.transactions, market.data.transactionCount * sizeof(Transaction));
    delete[] market.data.transactions;
    market.data.transactions = newTransact;
    market.data.transactionCapacity = cap;
    return true;
}

/// Allocate space for atleast @count new orders in market
/// @param market - ref to market context
/// @param count - number of new orders
/// @return - true on success, false otherwise
static bool ensureOrderSpace(Market & market, int count) {
    int cap = market.data.orderCapacity;

    if (market.data.orderCount + count < cap) {
        return true;
    }

    do {
        cap = doubleOrInitCapacity(cap);
    } while (market.data.orderCount + count >= cap);

    Order * newOrders = new(std::nothrow) Order[cap];
    if (!newOrders) {
        market.criticalError = true;
        return false;
    }

    memcpy(newOrders, market.data.orders, market.data.orderCount * sizeof(Order));
    delete[] market.data.orders;
    market.data.orders = newOrders;
    market.data.orderCapacity = cap;
    return true;
}

/// Create log file and log data for order
/// @param market - ref to market context
/// @param walletId - the id of the wallet creating the order
/// @param firstTransactionIdx - the index of the first transaction created for this order
/// @param logFileName [out] - destination to write the name of the log file
/// @return - true on success, false otherwise
static bool logOrderTransactions(Market & market, unsigned walletId, int firstTransactionIdx, char logFileName[ORDER_LOG_NAME_SIZE]) {
    const int totalTranctions = market.data.transactionCount - firstTransactionIdx;

    char logName[ORDER_LOG_NAME_SIZE] = { 0, };
    snprintf(logName, sizeof(logName), "transaction-%u-%lld.log", walletId, currentTime());
    std::ofstream logFile(logName, std::ios::out | std::ios::trunc);

    if (!logFile) {
        return false;
    }

    double coins = 0;
    for (int c = firstTransactionIdx; c < market.data.transactionCount; c++) {
        Transaction & transact = market.data.transactions[c];
        coins += transact.fmiCoins;

        const int senderIndex = getWalletIndex(market, transact.senderId);
        assert(senderIndex != -1 && "Could not find sender for order");
        const int recieverIndex = getWalletIndex(market, transact.receiverId);
        assert(recieverIndex != -1 && "Could not find reciever for order");

        logFile << "Sender: " << market.data.wallets[senderIndex].owner
            << "\tReciever: " << market.data.wallets[recieverIndex].owner
            << "\tcoins: " << transact.fmiCoins << std::endl;
    }

    logFile << "Transactoion count: " << totalTranctions << std::endl
        << "Oders total cost: " << coins << std::endl;

    strncpy(logFileName, logName, ORDER_LOG_NAME_SIZE);
    logFile.close();
    return true;
}

/// Execute all transaction caused by new order
/// @param market - ref to market context
/// @param newOrder [in|out] - ref to new order, it's fmiCoins will be modified if needed
/// @return - true on success, false otherwise
static bool executeOrderTransactions(Market & market, Order & newOrder) {
    double coinsRemaining = newOrder.fmiCoins;
    int transactCount = 0;

    // find how many transaction we must make
    for (int c = 0; c < market.data.orderCount; c++) {
        Order & currentOrder = market.data.orders[c];
        transactCount++;
        coinsRemaining = std::max(0.0, coinsRemaining - currentOrder.fmiCoins);
        if (isCoinsZero(coinsRemaining)) {
            break;
        }
    }

    // make space for transactions
    if (!ensureTransactionSpace(market, transactCount)) {
        return false;
    }

    // make space for order
    if (!isCoinsZero(coinsRemaining) && !ensureOrderSpace(market, 1)) {
        return false;
    }

    // make transactions
    const int firstTransaction = market.data.transactionCount;
    int removedOrders = 0;
    for (int c = 0; c < market.data.orderCount; c++) {
        Order & currentOrder = market.data.orders[c];

        unsigned senderId = -1, recieverId = -1;
        const double transactCoins = std::min(newOrder.fmiCoins, currentOrder.fmiCoins);

        if (newOrder.type == Order::Type::SELL) {
            // newOrder.id -> currentOrder.id
            senderId = newOrder.walletId;
            recieverId = currentOrder.walletId;
        } else {
            // currentOrder.id -> newOrder.id
            senderId = currentOrder.walletId;
            recieverId = newOrder.walletId;
        }

        bool done = false;
        if (newOrder.fmiCoins >= currentOrder.fmiCoins) {
            removedOrders++;
            newOrder.fmiCoins -= currentOrder.fmiCoins;
        } else {
            currentOrder.fmiCoins -= newOrder.fmiCoins;
            newOrder.fmiCoins = 0;
            done = true;
        }

        market.data.wallets[senderId].fiatMoney += toFiat(transactCoins);
        market.data.wallets[recieverId].fiatMoney -= toFiat(transactCoins);

        const bool madeTransaction = makeTransaction(market, recieverId, senderId, transactCoins);
        assert(madeTransaction && "Failed to make transaction after ensureTransactionSpace returned true");
        if (done) {
            break;
        }
    }

    if (removedOrders > 0) {
        // we removed some orders, but we want to keep them in order, so move remaining forward
        const int newOrderCount = market.data.orderCount - removedOrders;
        memcpy(market.data.orders, market.data.orders + removedOrders, newOrderCount * sizeof(Order));
        market.data.orderCount = newOrderCount;
    }

    return true;
}

// PUBLIC FUNCTIONS BELOW

double toFiat(double coins) {
    return coins * CONVERTION_RATE;
}

double toCoins(double fiat) {
    return fiat / CONVERTION_RATE;
}

long long currentTime() {
    return time(nullptr);
}

bool initMarket(Market & market) {
    memset(&market.data, 0, sizeof(market.data)); // everything should be 0
    market.criticalError = false;

    const std::ios::open_mode readMode = std::ios::in | std::ios::binary;

    market.walletFile.open(WALLET_FILE, readMode);
    market.transactionFile.open(TRANSACTION_FILE, readMode);
    market.orderFile.open(ORDER_FILE, readMode);

    // if open failed, then we will handle when loading data
    return true;
}

bool loadMarketData(Market & market) {
    return loadWallets(market) && loadTransactions(market) && loadOrders(market);
}

bool saveMarketData(Market & market) {
    if (!saveData(market.walletFile, reinterpret_cast<const char *>(market.data.wallets), market.data.walletCount * sizeof(Wallet))) {
        return false;
    }

    const Transaction * newTransactions = market.data.transactions + market.data.initialTransactionCount;
    const int newTransactionsCount = market.data.transactionCount - market.data.initialTransactionCount;
    if (!saveData(market.transactionFile, reinterpret_cast<const char *>(newTransactions), newTransactionsCount * sizeof(Transaction))) {
        return false;
    }

    if (!saveData(market.orderFile, reinterpret_cast<const char *>(market.data.orders), market.data.orderCount * sizeof(Order))) {
        return false;
    }

    return true;
}

bool makeTransaction(Market & market, unsigned recieverId, unsigned senderId, double coins) {
    const Transaction transact = {currentTime(), senderId, recieverId, coins};

    if (!ensureTransactionSpace(market, 1)) {
        return false;
    }

    market.data.transactions[market.data.transactionCount++] = transact;
    return true;
}


bool walletExists(unsigned walletId) {
    return walletId < NEXT_WALLET_ID;
}

double getCoinTotal(Market & market, unsigned walletId) {
    double totalCoins = 0;
    for (int c = 0; c < market.data.transactionCount; c++) {
        Transaction &transact = market.data.transactions[c];
        if (transact.receiverId == walletId) {
            totalCoins += transact.fmiCoins;
        } else if (transact.senderId == walletId) {
            totalCoins -= transact.fmiCoins;
        }
    }
    return totalCoins;
}

bool createWallet(Market & market, char owner[OWNER_SIZE], double fiatMoney, Wallet & newWallet) {
    Wallet wallet = {"", NEXT_WALLET_ID, fiatMoney};
    strncpy(wallet.owner, owner, OWNER_SIZE);
    
    if (market.data.walletCount == market.data.walletCapacity) {
        const int newCap = doubleOrInitCapacity(market.data.walletCapacity);
        Wallet * newWallets = new(std::nothrow) Wallet[newCap];
        if (!newWallets) {
            return false;
        }
        memcpy(newWallets, market.data.wallets, market.data.walletCount * sizeof(Wallet));
        delete[] market.data.wallets;
        market.data.wallets = newWallets;
        market.data.walletCapacity = newCap;
    }

    if (!makeTransaction(market, wallet.id, SYSTEM_WALLET_ID, toCoins(wallet.fiatMoney))) {
        return false;
    }

    NEXT_WALLET_ID++;
    market.data.wallets[market.data.walletCount++] = wallet;
    newWallet = wallet;
    return true;
}

int getWalletIndex(Market & market, unsigned walletId) {
    if (market.data.walletCount <= walletId) {
        return -1;
    }
    assert(market.data.wallets[walletId].id == walletId && "Missmatch in wallet id and wallet index!");
    return walletId;
}

bool executeOrder(Market & market, Wallet & wallet, Order::Type type, double coins, char logName[ORDER_LOG_NAME_SIZE]) {
    logName[0] = '\0'; // asume logging failed

    Order newOrder = {type, wallet.id, coins};
    const int firstTransactionIdx = market.data.transactionCount;

    // the types of all order in the market will be all SELL or either all BUY, because any order of different type will
    // be either "consumed" whole by existing orders of the opposite type
    //        or consume all existing orders and be added in the market
    if (market.data.orderCount > 0 && market.data.orders[0].type != type) {
        // fulfil any orders we can
        if (!executeOrderTransactions(market, newOrder)) {
            return false;
        }
    } else {
        // we have orders of the same type, so we just need to add the order
        if (!ensureOrderSpace(market, 1)) {
            return false;
        }
    }

    // regardless of the value of logged, return true and caller will deduce if loggin was cuccessful
    // by inspecting logName, if it is empty string - logging failed
    const bool logged = logOrderTransactions(market, wallet.id, firstTransactionIdx, logName);
    assert(logged && "Failed to write log file but successed in executing order");

    // dont need to add it
    if (isCoinsZero(newOrder.fmiCoins)) {
        return true;
    }

    market.data.orders[market.data.orderCount++] = newOrder;
    return true;
}

bool calculateAllWalletInfo(Market & market, WalletInfo *& info) {
    const int walletCount = market.data.walletCount;

    WalletInfo * totalsInfo = new(std::nothrow) WalletInfo[walletCount];
    if (!totalsInfo) {
        market.criticalError = true;
        return false;
    }

    const long long timeNow = currentTime();
    for (int c = 0; c < walletCount; c++) {
        totalsInfo[c] = {0.0, 0, timeNow, 0};
    }

    const int transactCount = market.data.transactionCount;
    // gather the data
    for (int c = 0; c < transactCount; c++) {
        Transaction & transact = market.data.transactions[c];

        const int recieverIdx = getWalletIndex(market, transact.receiverId);
        assert(recieverIdx != -1 && "Transaction has invalid receiverId");

        WalletInfo & recvInfo = totalsInfo[recieverIdx];

        recvInfo.coins += transact.fmiCoins;
        recvInfo.transactCount += 1;
        recvInfo.firstTransact = std::min(recvInfo.firstTransact, transact.time);
        recvInfo.lastTransact = std::max(recvInfo.lastTransact, transact.time);

        if (transact.senderId != SYSTEM_WALLET_ID) {
            const int senderIdx = getWalletIndex(market, transact.senderId);
            assert(senderIdx!= -1 && "Transaction has invalid receiverId");

            WalletInfo & sendInfo = totalsInfo[senderIdx];

            sendInfo.coins -= transact.fmiCoins;
            sendInfo.transactCount += 1;
            sendInfo.firstTransact = std::min(sendInfo.firstTransact, transact.time);
            sendInfo.lastTransact = std::max(sendInfo.lastTransact, transact.time);
        }
    }

    info = totalsInfo;
    return true;
}
