#pragma once
#include "OrderBookEntry.h"
#include "Candlestick.h"
#include "CSVReader.h"
#include <string>
#include <vector>

class OrderBook
{
    public:
        /** construct, reading a csv data file */
        OrderBook(std::string filename);
        /** return vector of all known products in the dataset*/
        std::vector<std::string> getKnownProducts();

        /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product);
                                              
        /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);
        
        

        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);

        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        /** returns the highest price of the orders */
        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        /** returns the lowest price of the orders */
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        /** returns the mean price of the orders */
        static double getMeanPrice(std::vector<OrderBookEntry>& orders);
        /** returns the total volume for the orders */
        static double getVolume(std::vector<OrderBookEntry>& orders);
        /** returns the date/time into an integer */
        int convertTimeToIntHrs(std::string inputString);
        /** return the date/time into hour format */
        std::string convertTimeToHrs(std::vector<OrderBookEntry>& orders);
        /** returns a vector of vector of orders according to timeframes and other filters*/
        std::vector<std::vector<OrderBookEntry>> getTimeframes(std::string product, OrderBookType type);
        /** returns a vector of Candlestick data according to the filtered order type */
        std::vector<Candlestick> getCandlestickData(std::string product, OrderBookType type);

    private:
        std::vector<OrderBookEntry> orders;


};