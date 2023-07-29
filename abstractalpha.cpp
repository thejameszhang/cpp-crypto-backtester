#include "abstractalpha.hpp"

// Researchers must implement this function.
std::vector<ld> 
AbstractAlpha::generate_trades(std::unordered_map<std::string, std::vector<std::vector<ld>>>) {
    throw std::logic_error("Implement this function.");
}

// Lookback getter.
int AbstractAlpha::get_lookback() {
    return this->lookback;
}

// Saves current alpha object.
void AbstractAlpha::save(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(this), sizeof(*this));
    }
}

// Loads the current alpha oject.
void AbstractAlpha::load(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (inFile) {
        inFile.read(reinterpret_cast<char*>(this), sizeof(*this));
    }
}