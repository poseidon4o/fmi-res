#pragma once

// for strncpy
#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <ctime>

/// Max size of owner field
const int OWNER_SIZE = 256;

/// Max size of the log file name when creating order
const int ORDER_LOG_NAME_SIZE = 128;

struct Wallet {
    char owner[OWNER_SIZE];
    unsigned id;
    double fiatMoney;
};

struct Transaction {
    long long time;
    unsigned senderId;
    unsigned receiverId;
    double fmiCoins;
};

struct Order {
    enum Type { SELL, BUY } type;
    unsigned walletId;
    double fmiCoins;
};

/// Context-like structure containing all data needed for normal runtime
struct Market {
    struct Data {
        Wallet * wallets; ///< All wallets loaded and created runtime
        int walletCount; ///< Number of wallets
        int walletCapacity; ///< Actual size of @wallets array

        Transaction * transactions; ///< All transactions loaded and created runtime
        int transactionCount; ///< Number of transactions
        int transactionCapacity; ///< Actual size of @transactions array
        int initialTransactionCount; ///< Number of transactions loaded from file on startup

        Order * orders; ///< All active orders
        int orderCount; ///< Number of active orders
        int orderCapacity; ///< Actual size of @orders array
    };

    Data data; ///< All market data

    /// All files are kept open during the whole runtime of the program
    /// Also their seek and get positions are reset before use

    std::fstream walletFile; ///< File reference to wallets.dat
    std::fstream transactionFile; ///< File reference to transactions.dat
    std::fstream orderFile; ///< File reference to orders.dat
    int orderSize; ///< The initial size of orders.dat, used when saving to decide if new file will be created

    bool criticalError; ///< Set to true when error that causes program termination is encountered (e.g allocation fail)
};

/// Info structer used to gather info for all wallets
struct WalletInfo {
    double coins; ///< Number of coins
    int transactCount; ///< Number of transaction to and from this wallet
    long long firstTransact; ///< Time of the first transaction
    long long lastTransact; ///< Time of the last transaction
};

/// Check if wallet id is valid
/// @param walletId - id to check
/// @return - true if ID is in valid range, false otherwise
bool walletExists(unsigned walletId);

/// Get index of a wallet by its walletID
/// @param market - ref to market context
/// @param walletId - the id to find
/// @return - index of the wallet in market.data.wallets, or -1 if not found
int getWalletIndex(Market & market, unsigned walletId);

/// Create wallet in the market and seed FMICoins to it
/// @param market - ref to market context
/// @param owner - name of the wallet owner
/// @param fiatMoney - amount of money in the wallet
/// @param newWallet [out] - ref to write wallet if create succesfully
/// @return - true on success, false otherwise
bool createWallet(Market & market, char owner[OWNER_SIZE], double fiatMoney, Wallet & newWallet);

/// Make a transaction between two wallet IDs
/// @param market - ref to market context
/// @param recieverId - id of wallet getting the coins
/// @param senderId - id of wallet sending the coins
/// @param coins - amount of coins to send
/// @return - true if transaction was successful, false otherwise
bool makeTransaction(Market & market, unsigned recieverId, unsigned senderId, double coins);

/// Calculate total amount of coins for given walletId
/// @param market - ref to market context
/// @param walletId - the wallet id to check
/// @return - amount of coins this wallet id owns
double getCoinTotal(Market & market, unsigned walletId);

/// Create and execute order
/// @param market - ref to market context
/// @param wallet - ref to wallet creating the order
/// @param type - the type of the order (SELL or BUY)
/// @param coins - amount of coins in the order
/// @param logName [out] - the name of the log file if logging was succesful, empty string otherwise
/// @return - true on success, false otherwise
bool executeOrder(Market & market, Wallet & wallet, Order::Type type, double coins, char logName[ORDER_LOG_NAME_SIZE]);

/// Save the market data to appropriate files
/// @param market - ref to market context
/// @return - true on success, false otherwise
bool saveMarketData(Market & market);

/// Load data in market from save files
/// @param market - ref to market to load data into
/// @return - true on success, false otherwise
bool loadMarketData(Market & market);

/// Initialize empty market context
/// @param market - ref to market context
/// @return - true on success, false otherwise
bool initMarket(Market & market);

/// Calculate wallet info for all wallets in market
/// @param market - refo to market context
/// @param info [out] - pointer that will contain all info, size is the same as market.data.walletCount
///                     *MUST* be free by caller on success
/// @return - true on success, false otherwise
bool calculateAllWalletInfo(Market & market, WalletInfo *& info);

/// Convert FMICoin amount in fiatMonet ammount
/// @param coins - amount of coins
/// @return - amount of fiat money
double toFiat(double coins);

/// Convert fiat money to FMICoins
/// @param fiat - amount of fiat money
/// @return - amount of coins
double toCoins(double fiat);

/// Get current time as int64
/// @return - current time in seconds
long long currentTime();
