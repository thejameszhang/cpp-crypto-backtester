#include "abstractclientbacktester.hpp"

// Default constructor.
AbstractClientBacktester::AbstractClientBacktester() 
: AbstractBacktester::AbstractBacktester() {

}

// Second constructor.
AbstractClientBacktester::AbstractClientBacktester(boost::gregorian::date& _start_date, 
boost::gregorian::date& _end_date, int _max_lookback)
: AbstractBacktester::AbstractBacktester(_start_date, _end_date, _max_lookback) {

}