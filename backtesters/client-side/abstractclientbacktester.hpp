#include "../abstractbacktester.hpp"

class AbstractClientBacktester : public AbstractBacktester {

public:

    // Default constructor for the AbstractClientBacktester.
    AbstractClientBacktester();
    
    // Second constructor for the AbstractClientBacktester.
    AbstractClientBacktester(boost::gregorian::date& _start_date, 
                       boost::gregorian::date& _end_date, 
                        int _max_lookback);
    
    // Note still not implementing backtest function.

    // 
    

private:
    int data_reload_interval = 5000;
};
