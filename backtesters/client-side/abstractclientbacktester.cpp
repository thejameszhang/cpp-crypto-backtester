#include "abstractclientbacktester.hpp"

// Default constructor.
AbstractClientBacktester::AbstractClientBacktester() 
: AbstractBacktester::AbstractBacktester() {

}

// Second constructor.
AbstractClientBacktester::AbstractClientBacktester(boost::posix_time::ptime& _start_date, 
boost::posix_time::ptime& _end_date, int _max_lookback)
: AbstractBacktester::AbstractBacktester(_start_date, _end_date, _max_lookback) {

}