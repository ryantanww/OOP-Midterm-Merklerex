#pragma once

#include <string>
#include <vector>

class Candlestick
{
    public:
        Candlestick(std::string timestamp,
                    std::string product,
                    double open,
                    double close,
                    double high,
                    double low,
                    double volume);
        
        /** print the candlestick data */
        void printStat();
        /** plot a candlestick chart according to the data stored in the vector */
        static void plotCandlestickChart(std::vector<Candlestick>& candlestickData);
        /** plot a volume chart according to the data stored in the vector */
        static void plotVolumeGraph(std::vector<Candlestick>& candlestickData);

        std::string timestamp;
        std::string product;
        double open;
        double close;
        double high;
        double low;
        double volume;
    
    

};
