#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

typedef long double ld;

#ifndef ABSTRACTALPHA_H
#define ABSTRACTALPHA_H

// Abstract class definition
class AbstractAlpha {
public:
    // Default constructor
    AbstractAlpha();

    // Pure virtual function for backtesting
    virtual std::vector<ld> 
    generate_trades(std::unordered_map<std::string, std::vector<std::vector<ld>>>) = 0;

    // Lookback getter.
    int get_lookback();

    // Save current alpha object.
    void save(const std::string& filename);

    // Load the current alpha object.
    void load(const std::string& filename);
    
    // Virtual destructor (needed for polymorphic behavior)
    virtual ~AbstractAlpha() {}

private:
    std::string name;
    int lookback;
};

#endif // ABSTRACTALPHA_H