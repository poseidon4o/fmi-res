#include "market.h"

#include <iostream>
#include <algorithm>
#include <iomanip>

static const int RANK_SIZE = 10;

/// Read data for new wallet and create one
/// @param market - ref to market context
/// @return - true on success, false otherwise
bool addWallet(Market & market) {
    double money = 0;
    char owner[OWNER_SIZE] = {0,};
    std::cin >> money;
    std::cin.getline(owner, OWNER_SIZE);

    Wallet wallet;
    bool created = createWallet(market, owner, money, wallet);
    if (!created) {
        return false;
    }

    std::cout << "New wallet with id " << wallet.id << std::endl;
    return true;
}

/// Read info and make an order
/// @param market - ref to market context
/// @return - true on success, false otherwise
bool makeOrder(Market & market) {
    char type[16];
    double coins;
    unsigned walletId;
    std::cin >> type >> coins >> walletId;

    if (!walletExists(walletId)) {
        return false;
    }

    Order::Type orderType;
    if (!strcmp(type, "SELL")) {
        orderType = Order::SELL;
    } else if (!strcmp(type, "BUY")) {
        orderType = Order::BUY;
    } else {
        return false;
    }

    const int walletIdx = getWalletIndex(market, walletId);
    if (walletIdx == -1) {
        return false;
    }

    Wallet & wallet = market.data.wallets[walletIdx];

    bool executed = false;
    char logName[ORDER_LOG_NAME_SIZE];

    if (orderType == Order::Type::SELL) {
        const double ballance = getCoinTotal(market, walletId);
        if (ballance < coins) {
            std::cerr << "Wallet with owner \"" << wallet.owner << "\" has only " << ballance << " coins, can't sell " << coins << " coins" << std::endl;
            return false;
        }
        executed = executeOrder(market, wallet, Order::Type::SELL, coins, logName);
    } else {
        const double required = toFiat(coins);
        if (wallet.fiatMoney < required) {
            std::cerr << "Wallet with owner \"" << wallet.owner << "\" has only " << wallet.fiatMoney << " money but needs" << required << " to buy " << coins << " coins" << std::endl;
            return false;
        }
        executed = executeOrder(market, wallet, Order::Type::BUY, coins, logName);
    }

    if (executed) {
        std::cout << "Order executed, log file: " << logName << std::endl;
    }

    return executed;
}

/// Read wallet id and print info for it
/// @param market - ref to market context
/// @return - true on success, false otherwise
bool walletInfo(Market & market) {
    unsigned walletId;
    std::cin >> walletId;

    if (!walletExists(walletId)) {
        return false;
    }

    const int index = getWalletIndex(market, walletId);
    if (index == -1) {
        return false;
    }

    const double coins = getCoinTotal(market, walletId);
    Wallet & wallet = market.data.wallets[index];

    std::cout << "Wallet with owner \"" << wallet.owner << "\" has " << wallet.fiatMoney << " fiat money and " << coins << " coins" << std::endl;
    return true;
}

/// Calculate and print info for RANK_SIZE welthies wallets
/// @param market - ref to market context
/// @return - true on success, false otherwise
bool attractInvestors(Market & market) {
    // we need to find X welthiest wallets, so we must calculate total coins for all wallets

    WalletInfo * totalsInfo = nullptr;
    if (!calculateAllWalletInfo(market, totalsInfo)) {
        return false;
    }

    // this will keep indices in totalsInfo of the X welthies wallets
    int ranking[RANK_SIZE] = {};
    for (int c = 0; c < RANK_SIZE; c++) {
        ranking[c] = -1;
    }

    // if we have fewer wallets we will only give info for existing wallets
    // we could further optimize and print all wallets in case there are fewer than rank size but this is not common case
    const int rankCount = std::min(RANK_SIZE, market.data.walletCount);

    // to get X top, we need X iterations of selection sort
    // when X is significantly smaller than the number of items we can store the "sorted" rank
    // in separate location and check it when selecting next elements
    for (int c = 0; c < rankCount; c++) {
        double maxCoins = 0.;
        int index = -1;
        for (int r = 0; r < market.data.walletCount; r++) {
            if (totalsInfo[r].coins > maxCoins) {
                // make sure this is not already saved
                bool saved = false;
                for (int k = 0; k < c; k++) {
                    if (ranking[k] == r) {
                        saved = true;
                        break;
                    }
                }

                if (!saved) {
                    maxCoins = totalsInfo[r].coins;
                    index = r;
                }
            }
        }

        ranking[c] = index;
    }

    // print the data we collected
    for (int c = 0; c < rankCount; c++) {
        const int walletIdx = ranking[c];
        const WalletInfo & info = totalsInfo[walletIdx];
        std::cout << "Investor \"" << market.data.wallets[walletIdx].owner << "\" has " << info.coins << "coins" << std::endl
                  << "\tmade with " << info.transactCount << " transactions" << std::endl
                  << "\tfirst transaction " << info.firstTransact << std::endl
                  << "\tlast transaction " << info.lastTransact << std::endl;
    }

    delete[] totalsInfo;
    return true;
}

/// Pretty-print money amount
/// @param coins - amount of money in coins
void printCoins(double coins) {
    const double fiat = toFiat(coins);
    std::cout << "{" << std::setprecision(6) << coins << " FMIC}=(" << std::setprecision(6) << fiat << " BGN)";
}

/// Pretty-print money amount
/// @param coins - amount of money in coins
void printFiat(double fiat) {
    const double coins = toCoins(fiat);
    std::cout << "{" << std::setprecision(6) << fiat << " BGN}=(" << std::setprecision(6) << fiat << " FMIC)";
}

/// Pretty-print all of the market data (wallets, transactions, orders)
/// Not part of the public commands, used mainly for debugging
/// @param market - ref to market context
void printMarket(Market & market) {
    std::cout << std::endl << "WALLETS:" << std::endl;
    for (int c = 0; c < market.data.walletCount; c++) {
        Wallet & wallet = market.data.wallets[c];
        std::cout << wallet.owner << "[" << wallet.id << "] ";
        printFiat(wallet.fiatMoney);
        std::cout << std::endl;
    }

    const unsigned systemId = -1;

    std::cout << std::endl << "TRANSCATIONS:" << std::endl;
    for (int c = 0; c < market.data.transactionCount; c++) {
        Transaction & transact = market.data.transactions[c];

        if (transact.senderId == systemId) {
            std::cout << "SYSTEM ---> ";
        } else {
            Wallet & sender = market.data.wallets[getWalletIndex(market, transact.senderId)];
            std::cout << sender.owner << "[" << sender.id << "] ---> ";
        }

        Wallet & recv = market.data.wallets[getWalletIndex(market, transact.receiverId)];
        printCoins(transact.fmiCoins);
        std::cout << " ---> " << recv.owner << "[" << recv.id << "]" << std::endl;
    }

    std::cout << std::endl << "ORDERS:" << std::endl;
    for (int c = 0; c < market.data.orderCount; c++) {
        Order & order = market.data.orders[c];
        Wallet & wallet = market.data.wallets[getWalletIndex(market, order.walletId)];
        std::cout << wallet.owner << "[" << wallet.id << "]" << (order.type == Order::Type::SELL ? " SELL " : " BUY ");
        printCoins(order.fmiCoins);
        std::cout << std::endl;
    }

    std::cout << "END" << std::endl;
}

enum Command {
    Invalid,
    AddWallet,
    MakeOrder,
    WalletInfo,
    AttractInvestors,
    PrintMarket,
    Quit,
};

/// Read and parse next command
/// @return - the command read
Command getCommand() {
    // all commands are shorter than 64
    char cmdString[64] = {0, };
    std::cin >> cmdString;
    if (!strcmp(cmdString, "add-wallet")) {
        return Command::AddWallet;
    } else if (!strcmp(cmdString, "make-order")) {
        return Command::MakeOrder;
    } else if (!strcmp(cmdString, "wallet-info")) {
        return Command::WalletInfo;
    } else if (!strcmp(cmdString, "attract-investors")) {
        return Command::AttractInvestors;
    } else if (!strcmp(cmdString, "quit")) {
        return Command::Quit;
    } else if (!strcmp(cmdString, "print-market")) {
        return Command::PrintMarket;
    }
    return Command::Invalid;
}

/// Read Eval Print Loop for commands, stops on quit command
/// @param market - ref to market context
void mainLoop(Market & market) {
    Command cmd = Command::Invalid;

    do {
        std::cout << "Commad> ";
        cmd = getCommand();
        bool success = false;

        switch (cmd) {
        case Command::Invalid:
            std::cout << "Unknown command, try again" << std::endl;
            break;
        case Command::Quit:
            std::cout << "Quitting and saving data..." << std::endl;
            break;
        case Command::AddWallet:
            success = addWallet(market);
            break;
        case Command::MakeOrder:
            success = makeOrder(market);
            break;
        case Command::WalletInfo:
            success = walletInfo(market);
            break;
        case Command::AttractInvestors:
            success = attractInvestors(market);
            break;
        case Command::PrintMarket:
            printMarket(market);
            success = true;
            break;
        }
        if (!success) {
            std::cerr << "Last command failed" << std::endl;
        }
        if (market.criticalError) {
            std::cerr << "Critical error, shutting down" << std::endl;
            return;
        }
    } while (cmd != Command::Quit);
}

int main() {
    Market market;
    if (!initMarket(market)) {
        std::cerr << "Failed to open files";
        return 1;
    }

    if (!loadMarketData(market)) {
        std::cerr << "Failed to load market data";
        return 1;
    }

    mainLoop(market);

    if (!saveMarketData(market)) {
        std::cerr << "Failed saving data to files. Files state unknown";
    }

    return 0;
}