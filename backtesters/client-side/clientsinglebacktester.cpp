#include "clientsinglebacktester.hpp"

std::pair<std::vector<std::vector<ld>>, std::vector<std::vector<ld>>>
ClientSingleBacktester::backtest(AbstractAlpha* alpha) {
    /*
    Backtests a single alpha object in a serverless environment in a memory-
    efficient manner by querying chunks of data from the MySQL database.
    Suppose the close prices and holdings are a MxN matrix where M is the 
    number of minutes, and N is the number of currencies. The trader's returns 
    are computed as the element-wise multiplication of the returns 
    (percent change) of the close prices and the holdings.
    :param alpha: instance derived from the AbstractAlpha class
    :return: {2d vector of returns, 2d vector of holdings}
    */

    // First get some general information about the backtester and alpha.
    const std::string end_date = get_end_date();
    int max_lookback = get_max_lookback();
    int alpha_lookback = alpha->get_lookback();
    int data_reload_interval = ClientSingleBacktester::get_data_reload_interval();
    
    if (alpha_lookback > max_lookback)
        throw std::logic_error("Alpha lookback cannot be longer than backtester max lookback");

    // Load the initial data chunk and the chunk end date.
    std::unordered_map<std::string, std::vector<std::vector<ld>>> chunk;
    chunk = ClientSingleBacktester::get_data_chunk(this->current_date);
    std::string chunk_end = time_shift(this->current_date, data_reload_interval);

    std::vector<ld> alpha_holdings;
    std::unordered_map<std::string, std::vector<std::vector<ld>>> alpha_data;
    int alpha_lookback = alpha->get_lookback();

    // Suppose backtester max_lookback is 5. Then rows 0-4 in the 2d vectors
    // of the chunked data are all for lookback. The 5th row is the first
    // trading day, so we set curr = max_lookback. To get the necessary
    // lookback and current data for the trading day, it would be 
    // vector.slice(curr - alpha_lookback, curr + 1);
    int curr = max_lookback;

    // Iterate through the entire backtesting period
    while (this->current_date <= end_date) {

        // Get the next chunk of data, if necessary.
        if (this->current_date > chunk_end) {
            curr = max_lookback;
            chunk = ClientSingleBacktester::get_data_chunk(this->current_date);
            chunk_end = time_shift(this->current_date, data_reload_interval);
        }

        // No need to partition or rebalance the universe, don't need that yet

        // Slice the chunked data for the alpha.
        for (auto pair : chunk) {
            alpha_data[pair.first] = slice(pair.second, curr - alpha_lookback, curr + 1);
            this->close_prices.push_back(chunk["close"][curr]);
        }

        // Get the alpha's holdings and append it to the holdings matrix.
        alpha_holdings = alpha->generate_trades(alpha_data);
        holdings.push_back(alpha_holdings);

        // Increment the current timestamps.
        this->current_date = time_shift(this->current_date, 1);
        curr++;
    }

    // After the backtesting, compute the returns.
    std::vector<std::vector<ld>> returns = pct_change(this->close_prices);
    element_wise_multiplication(returns, holdings);
    
    // Return the trader's returns and historical holdings.
    return {returns, holdings};
}
