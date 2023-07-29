#include "../abstractbacktester.hpp"

class AbstractClientBacktester : public AbstractBacktester {

public:
    
    // Second constructor for the AbstractClientBacktester.
    AbstractClientBacktester(std::string _start_date, 
                            std::string _end_date, 
                            int _max_lookback,
                            std::string _exchange, 
                            std::vector<std::string> _factors);
    
    // Note that this is still an abstract class, so not yet
    // implementing the virtual backtest function

};
