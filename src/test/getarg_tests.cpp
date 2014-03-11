#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BTH");
    BOOST_CHECK(GetBoolArg("-BTH"));
    BOOST_CHECK(GetBoolArg("-BTH", false));
    BOOST_CHECK(GetBoolArg("-BTH", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BTHo"));
    BOOST_CHECK(!GetBoolArg("-BTHo", false));
    BOOST_CHECK(GetBoolArg("-BTHo", true));

    ResetArgs("-BTH=0");
    BOOST_CHECK(!GetBoolArg("-BTH"));
    BOOST_CHECK(!GetBoolArg("-BTH", false));
    BOOST_CHECK(!GetBoolArg("-BTH", true));

    ResetArgs("-BTH=1");
    BOOST_CHECK(GetBoolArg("-BTH"));
    BOOST_CHECK(GetBoolArg("-BTH", false));
    BOOST_CHECK(GetBoolArg("-BTH", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBTH");
    BOOST_CHECK(!GetBoolArg("-BTH"));
    BOOST_CHECK(!GetBoolArg("-BTH", false));
    BOOST_CHECK(!GetBoolArg("-BTH", true));

    ResetArgs("-noBTH=1");
    BOOST_CHECK(!GetBoolArg("-BTH"));
    BOOST_CHECK(!GetBoolArg("-BTH", false));
    BOOST_CHECK(!GetBoolArg("-BTH", true));

    ResetArgs("-BTH -noBTH");  // -BTH should win
    BOOST_CHECK(GetBoolArg("-BTH"));
    BOOST_CHECK(GetBoolArg("-BTH", false));
    BOOST_CHECK(GetBoolArg("-BTH", true));

    ResetArgs("-BTH=1 -noBTH=1");  // -BTH should win
    BOOST_CHECK(GetBoolArg("-BTH"));
    BOOST_CHECK(GetBoolArg("-BTH", false));
    BOOST_CHECK(GetBoolArg("-BTH", true));

    ResetArgs("-BTH=0 -noBTH=0");  // -BTH should win
    BOOST_CHECK(!GetBoolArg("-BTH"));
    BOOST_CHECK(!GetBoolArg("-BTH", false));
    BOOST_CHECK(!GetBoolArg("-BTH", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BTH=1");
    BOOST_CHECK(GetBoolArg("-BTH"));
    BOOST_CHECK(GetBoolArg("-BTH", false));
    BOOST_CHECK(GetBoolArg("-BTH", true));

    ResetArgs("--noBTH=1");
    BOOST_CHECK(!GetBoolArg("-BTH"));
    BOOST_CHECK(!GetBoolArg("-BTH", false));
    BOOST_CHECK(!GetBoolArg("-BTH", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BTH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BTH", "eleven"), "eleven");

    ResetArgs("-BTH -bar");
    BOOST_CHECK_EQUAL(GetArg("-BTH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BTH", "eleven"), "");

    ResetArgs("-BTH=");
    BOOST_CHECK_EQUAL(GetArg("-BTH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BTH", "eleven"), "");

    ResetArgs("-BTH=11");
    BOOST_CHECK_EQUAL(GetArg("-BTH", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BTH", "eleven"), "11");

    ResetArgs("-BTH=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BTH", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BTH", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BTH", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BTH", 0), 0);

    ResetArgs("-BTH -bar");
    BOOST_CHECK_EQUAL(GetArg("-BTH", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BTH=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BTH", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BTH=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BTH", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BTH");
    BOOST_CHECK_EQUAL(GetBoolArg("-BTH"), true);

    ResetArgs("--BTH=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BTH", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBTH");
    BOOST_CHECK(!GetBoolArg("-BTH"));
    BOOST_CHECK(!GetBoolArg("-BTH", true));
    BOOST_CHECK(!GetBoolArg("-BTH", false));

    ResetArgs("-noBTH=1");
    BOOST_CHECK(!GetBoolArg("-BTH"));
    BOOST_CHECK(!GetBoolArg("-BTH", true));
    BOOST_CHECK(!GetBoolArg("-BTH", false));

    ResetArgs("-noBTH=0");
    BOOST_CHECK(GetBoolArg("-BTH"));
    BOOST_CHECK(GetBoolArg("-BTH", true));
    BOOST_CHECK(GetBoolArg("-BTH", false));

    ResetArgs("-BTH --noBTH");
    BOOST_CHECK(GetBoolArg("-BTH"));

    ResetArgs("-noBTH -BTH"); // BTH always wins:
    BOOST_CHECK(GetBoolArg("-BTH"));
}

BOOST_AUTO_TEST_SUITE_END()
