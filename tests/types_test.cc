#include <at/types.hpp>
#include <gtest/gtest.h>

TEST(CurrencyPair, ShouldPrintCorrectly) {
    auto p = at::currency_pair_t("USD", "BTC");
    ASSERT_EQ("USD_BTC", p.str());
}

