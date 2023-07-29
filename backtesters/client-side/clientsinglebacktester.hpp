#include "abstractclientbacktester.hpp"

class ClientSingleBacktester : public AbstractClientBacktester {
public:
    std::pair<std::vector<std::vector<ld>>, std::vector<std::vector<ld>>>
    backtest(AbstractAlpha* alpha);

private:
    std::vector<std::vector<ld>> close_prices;
    std::vector<std::vector<ld>> holdings;
    std::string current_date = AbstractBacktester::get_start_date();
};