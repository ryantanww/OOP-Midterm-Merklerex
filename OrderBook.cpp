#include "OrderBook.h"
#include "CSVReader.h"
#include "Candlestick.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>


/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string,bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }
    
    // now flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}

/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                                 std::string product)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product)
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}

/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                                 std::string product, 
                                                 std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product && 
            e.timestamp == timestamp )
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}


std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp) 
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
// asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, 
                                                 product, 
                                                 timestamp);
// bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, 
                                                 product, 
                                                 timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales; 

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    for (OrderBookEntry& ask : asks)
    {
    //     for bid in bids:
        for (OrderBookEntry& bid : bids)
        {
    //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
    //             sale = new order()
    //             sale.price = ask.price
            OrderBookEntry sale{ask.price, 0, timestamp, 
                product, 
                OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType =  OrderBookType::asksale;
                }
            
    //             # now work out how much was sold and 
    //             # create new bids and asks covering 
    //             # anything that was not sold
    //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # can do no more with this ask
    //                 # go onto the next ask
    //                 break
                    break;
                }
    //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # we adjust the bid in place
    //                 # so it can be used to process the next ask
    //                 bid.amount = bid.amount - ask.amount
                    bid.amount =  bid.amount - ask.amount;
    //                 # ask is completely gone, so go to next ask                
    //                 break
                    break;
                }


    //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount && 
                   bid.amount > 0)
                {
    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # update the ask
    //                 # and allow further bids to process the remaining amount
    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # some ask remains so go to the next bid
    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;             
}

double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

double OrderBook::getMeanPrice(std::vector<OrderBookEntry>& orders)
{

    double totalPrice = 0;
    double totalAmount = 0;
    double meanPrice = 0;
    // for each order in vector calculate the total price and total amount
    for (OrderBookEntry& e : orders)
    {
        totalPrice += e.amount * e.price;
        totalAmount += e.amount;
    }
    // return the mean price of total price/ total amount
    meanPrice = totalPrice/totalAmount;

    return meanPrice;
}

double OrderBook::getVolume(std::vector<OrderBookEntry>& orders)
{
    double totalVolume = 0;
    // for each order in vector calculate the total amount
    for (OrderBookEntry& e : orders)
    {
        totalVolume += e.amount;
    }
    // return as volume
    return totalVolume;
}

int OrderBook::convertTimeToIntHrs(std::string timestamp) {

    // read the string provided as a stream
    std::stringstream timeString(timestamp);

    std::stringstream formattedTimestamp;
    // structure holding a calendar date and time with different individual year, month.., seconds etc.
    std::tm t = {};
    
    // parse the string stream into the structure object in the expected format
    timeString >> std::get_time(&t, "%Y/%m/%d %H");
    
    // parse the structure into another string stream
    formattedTimestamp << std::put_time(&t, "%Y%m%d%H");
    
    // format the string stream to an integer
    int timestampInt = std::stoi(formattedTimestamp.str());

  return timestampInt;
}

std::string OrderBook::convertTimeToHrs(std::vector<OrderBookEntry>& orders)
{

    std::string hours = orders[0].timestamp.substr(0, 13) + ":00:00";

    return hours;
}

std::vector<std::vector<OrderBookEntry>> OrderBook::getTimeframes(std::string product, OrderBookType type)
{
    // create a vector of vector to store the timeframes in a given range
    std::vector<std::vector<OrderBookEntry>> allTimeframes;
    // create a vector of orders to store timestamps in a given range
    std::vector<OrderBookEntry> timeframes;

    // get the orders based on type and product
    std::vector<OrderBookEntry> orderEntries = getOrders(type, product);

    // loop through each orders in the orderEntries
    for(int i = 1; i <= orderEntries.size(); ++i)
    {   
        OrderBookEntry prevData = orderEntries[i-1];
        OrderBookEntry currData = orderEntries[i];
        // convert both to integer for easier comparison
        int prevTimestampInInt = convertTimeToIntHrs(prevData.timestamp);
        int currTimestampInInt = convertTimeToIntHrs(currData.timestamp);
        // always append the previous data point into the timeframes vector
        timeframes.push_back(prevData);
        // check if timestamps are not the same
        if(prevTimestampInInt != currTimestampInInt)
        {
            // append the timeframes vector into the allTimeframes 
            allTimeframes.push_back(timeframes);
            // clear the timeframes vector to create the next vector in allTimeframes
            timeframes.clear();
        }
        
    }

    // store the last timeframe if it contains any data
    if (!timeframes.empty())
    {
        allTimeframes.push_back(timeframes);
    }


    return allTimeframes;
}

std::vector<Candlestick> OrderBook::getCandlestickData(std::string product, OrderBookType type)
{
    std::vector<Candlestick> candleData;
    std::string timestamp = "";
    double open = 0;
    double close = 0;
    double high = 0;
    double low = 0;
    double volume = 0;
    double prevClose = 0;

    // get the orders based on timeframes, type and product
    std::vector<std::vector<OrderBookEntry>> orderEntries = getTimeframes(product, type);

    // loop through each vector in orderEntries
    for (std::vector<OrderBookEntry>& timeframes : orderEntries)
    {
        // vector to store the data in 
        Candlestick candle{timestamp, product, open, high, low, close, volume};
        // check if open is not the same as the previous close
        if (candle.open != prevClose)
        {
            // open is same as previous close
            candle.open = prevClose;
            
        } else
        {
            // calculate the mean price for the timeframe and store in open
            candle.open = getMeanPrice(timeframes);
        }
        // calculate all the necessary calculations and store in their variable
        candle.timestamp = convertTimeToHrs(timeframes);
        candle.high = getHighPrice(timeframes);
        candle.low = getLowPrice(timeframes);
        candle.close = getMeanPrice(timeframes);
        candle.volume = getVolume(timeframes);
        // append the data into the vector 
        candleData.push_back(candle);
        // apply previous close to be the current close 
        prevClose = candle.close;
    }

    return candleData;
}


