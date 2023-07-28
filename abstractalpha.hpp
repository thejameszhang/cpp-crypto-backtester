#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#ifndef ABSTRACTALPHA_H
#define ABSTRACTALPHA_H

// Abstract class definition
class AbstractAlpha {
public:
    // Default constructor
    AbstractAlpha();

    // Pure virtual function for backtesting
    virtual std::vector<long double> generate_trades() = 0;

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