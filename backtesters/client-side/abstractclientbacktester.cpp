#include "abstractclientbacktester.hpp"

// Default constructor.
AbstractClientBacktester::AbstractClientBacktester(std::string _start_date, 
                        std::string _end_date, int _max_lookback, 
                        std::string _exchange, 
                        std::vector<std::string> _factors) : 
AbstractBacktester::AbstractBacktester(_start_date, _end_date, _max_lookback, 
_exchange, _factors) {

}
