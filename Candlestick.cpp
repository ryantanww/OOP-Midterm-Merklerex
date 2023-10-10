#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <iomanip>
#include "Candlestick.h"



Candlestick::Candlestick( std::string _timestamp,
                          std::string _product,
                          double _open,
                          double _close,
                          double _high,
                          double _low,
                          double _volume)
{
  timestamp = _timestamp;
  product = _product;
  open = _open;
  close = _close;
  high = _high;
  low = _low;
  volume = _volume;
}

void Candlestick::printStat()
{// check for product type to get different decimal points and print the data out 
  if (product == "ETH/BTC" || product == "DOGE/BTC" || product == "DOGE/USDT")
  {
    std::cout << std::fixed << std::setprecision(15) << std::setw(20) << std::left << timestamp 
              << std::fixed << std::setprecision(15) << std::setw(20) << std::right  << product 
              << std::fixed << std::setprecision(15) << std::setw(20) << std::right  << open 
              << std::fixed << std::setprecision(15) << std::setw(20) << std::right  << high 
              << std::fixed << std::setprecision(15) << std::setw(20) << std::right  << low 
              << std::fixed << std::setprecision(15) << std::setw(20) << std::right  << close 
              << std::fixed << std::setprecision(2) << std::setw(20) << std::right  << volume << std::endl;
  } else
  {
    std::cout << std::fixed << std::setprecision(15) << std::setw(20) << std::left << timestamp 
              << std::fixed << std::setprecision(2) << std::setw(20) << std::right  << product 
              << std::fixed << std::setprecision(2) << std::setw(20) << std::right  << open 
              << std::fixed << std::setprecision(2) << std::setw(20) << std::right  << high 
              << std::fixed << std::setprecision(2) << std::setw(20) << std::right  << low 
              << std::fixed << std::setprecision(2) << std::setw(20) << std::right  << close 
              << std::fixed << std::setprecision(2) << std::setw(20) << std::right  << volume << std::endl;
  }
  
}

void Candlestick::plotCandlestickChart(std::vector<Candlestick>& candlestickData) 
{
  double maxPrice = candlestickData[0].high;
  double minPrice = candlestickData[0].low;
  std::string product = candlestickData[0].product;

  // check for highest high and lowest low in the data
  for(Candlestick& candlestick : candlestickData)
  {
    if (candlestick.high > maxPrice)maxPrice = candlestick.high;
    if (candlestick.low < minPrice)minPrice = candlestick.low;
  }

  double plotHeight = 100;
  // the difference between the y axis data
  double interval = ((maxPrice - minPrice) / plotHeight);

  // plot data based on plot height and plot price
  for (int i = plotHeight; i>=0; --i)
  {
    // obtain the y axis plot data
    double plotPrice = ((maxPrice - minPrice) * i / plotHeight) + minPrice;
    // check for product type to get different decimal points and print the data out 
    if (product == "ETH/BTC" || product == "DOGE/BTC" || product == "DOGE/USDT")
    {
      std::cout << std::fixed << std::setprecision(10) << std::setw(12) << plotPrice << " | ";
    } else
    {
      std::cout << std::fixed << std::setprecision(2) << std::setw(12) << plotPrice << " | ";
    }
    // apply space after the y axis
    std::cout << "   ";

    // check through data size and apply the necessary plotting
    for (int j = 0; j < candlestickData.size(); ++j)
    {
      double openPrice = candlestickData[j].open;
      double closePrice = candlestickData[j].close;
      double highPrice = candlestickData[j].high;
      double lowPrice = candlestickData[j].low;
      
      // check if open price and close price is equal to each other
      if (openPrice == closePrice)
      {
        // plot the required characters based on the conditions
        if ( plotPrice + interval >= openPrice && plotPrice < openPrice)
        {
          std::cout << "|-------------|";
        } else if (plotPrice <= highPrice && plotPrice > closePrice || 
            plotPrice >= lowPrice && plotPrice < closePrice)
        {
          std::cout << "     #####     ";
        }  else
        {
          std::cout << "               ";
        }
        // check if open is less than close 
      } else if (openPrice < closePrice)
      {
        // plot the required characters based on the conditions
        if ( plotPrice + interval >= closePrice && plotPrice < closePrice)
        {
          std::cout << "|-------------|";
        } else if ( plotPrice + interval >= openPrice && plotPrice < openPrice)
        {
          std::cout << "|-------------|";
        } else if (plotPrice <= highPrice && plotPrice > closePrice || 
            plotPrice >= lowPrice && plotPrice < openPrice)
        {
          std::cout << "     #####     ";
        } else if (plotPrice > openPrice && plotPrice < closePrice)
        {
          std::cout << "|   ^^^^^^^   |";
        } else
        {
          std::cout << "               ";
        }
        // check is open is greater than close
      } else if (openPrice > closePrice)
      {
        // plot the required characters based on the conditions
        if ( plotPrice + interval >= closePrice && plotPrice < closePrice)
        {
          std::cout << "|-------------|";
        } else if ( plotPrice + interval >= openPrice && plotPrice < openPrice)
        {
          std::cout << "|-------------|";
        } else if (plotPrice <= highPrice && plotPrice > openPrice || 
            plotPrice >= lowPrice && plotPrice < closePrice)
        {
          std::cout << "     #####     ";
        } else if (plotPrice < openPrice && plotPrice > closePrice)
        {
          std::cout << "|   vvvvvvv   |";
        } else
        {
          std::cout << "               ";
        }
      }
      std::cout << "      ";
    }
    std::cout << std::endl;
  }

  // plot the x axis
  std::cout << "----------------";
  for (const Candlestick& candlestick : candlestickData) 
  {
    std::cout << "---------|---------";
    std::cout << "--";
  }
  std::cout << "------------------------------";
  std::cout << std::endl;
  std::cout << "                     ";
  // plot the timeframes
  for (const Candlestick& candlestick : candlestickData) 
  {
    std::cout << candlestick.timestamp.substr(11, 8) << "             ";
  }
  std::cout << std::endl;
  std::cout << std::endl;
}


void Candlestick::plotVolumeGraph(std::vector<Candlestick>& candlestickData) 
{
  double maxVolume = candlestickData[0].volume;
  double minVolume = candlestickData[0].volume;

  for(Candlestick& volume : candlestickData)
  {
    // check for highest volume and lowest volume in the data
    if (volume.volume > maxVolume)maxVolume = volume.volume;
    if (volume.volume < minVolume)minVolume = volume.volume;
  }

  double plotHeight = 50;
  // the difference between the y axis data
  double interval = ((maxVolume - minVolume) / plotHeight);
  
  // plot data based on plot height and plot volume
  for (int i = plotHeight; i>=0; --i)
  {
    // obtain the y axis plot data
    float plotVolume = ((maxVolume) * i / plotHeight);
    std::cout << std::fixed << std::setprecision(2) << std::setw(18) << plotVolume << " | ";
    std::cout << " ";
    // check through data size and apply the necessary plotting
    for (int j = 0; j < candlestickData.size(); ++j)
    {
      float vol = candlestickData[j].volume;

      // plot the required characters based on the conditions
      if (plotVolume + interval >= vol && plotVolume < vol || plotVolume == vol)
      {
        std::cout << "_____________";
      }  else if (plotVolume < vol)
      {
        std::cout << "|           |";
      } else 
      {
        std::cout << "             ";
      }
      std::cout << " ";
    }
    std::cout << std::endl;;
  }
  // plot the x axis
  std::cout << "-----------------------";
  for (const Candlestick& candlestick : candlestickData) 
  {
    std::cout << "-----|-----";
    std::cout << "---";
  }
  std::cout << "------------------------";
  std::cout << std::endl;
  std::cout << "                        ";
  // plot the timeframe
  for (const Candlestick& candlestick : candlestickData) 
  {
    std::cout << candlestick.timestamp.substr(11, 8) << "      ";
  }
  std::cout << std::endl;
  std::cout << std::endl;
    
}