#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Candlestick.h"


MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}


void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 print candlestick data
    std::cout << "6: Print candlestick data " << std::endl;
    // 7 continue   
    std::cout << "7: Continue " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries_asks = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries_asks.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries_asks) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries_asks) << std::endl;

        std::vector<OrderBookEntry> entries_bids = orderBook.getOrders(OrderBookType::bid, 
                                                                p, currentTime);
        std::cout << "Bids seen: " << entries_bids.size() << std::endl;
        std::cout << "Max Bids: " << OrderBook::getHighPrice(entries_bids) << std::endl;
        std::cout << "Min Bids: " << OrderBook::getLowPrice(entries_bids) << std::endl;
    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}

void MerkelMain::printCandlestickData()
{
    std::cout <<"Enter product and order type(e.g. ETH/BTC,ASK)"<< std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');

    if (tokens.size() != 2)
    {
        std::cout << "MerkelMain::printCandlestickData Bad input! " << input << std::endl;
    } else
    {
        std::string product = tokens[0];
        OrderBookType orderType = OrderBookEntry::stringToOrderBookType(tokens[1]);

        // get candlestick data with filters
        std::vector<Candlestick> candles = orderBook.getCandlestickData(product, orderType);

        // print headers
        std::cout <<std::endl;
        std::cout << std::setw(20) << std::left << "Timeframes"
                << std::setw(20) << std::right  << "Product"
                << std::setw(20) << std::right  << "Open"
                << std::setw(20) << std::right  << "High"
                << std::setw(20) << std::right  << "Low"
                << std::setw(20) << std::right  << "Close" 
                << std::setw(20) << std::right  << "Volume" << std::endl;
        
        // print stats for each data in the vector
        for (Candlestick& candle : candles)
        {
            candle.printStat();
        }

        std::cout <<std::endl;

        // print out the candlestick chart
        Candlestick::plotCandlestickChart(candles);

        std::cout <<std::endl;

        // print out the volume chart
        Candlestick::plotVolumeGraph(candles);
    }
}

    


        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);
}
 
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-7" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-7" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        printCandlestickData();
    }   
    if (userOption == 7) 
    {
        gotoNextTimeframe();
    }    
}
