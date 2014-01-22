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
    ResetArgs("-ABC");
    BOOST_CHECK(GetBoolArg("-ABC"));
    BOOST_CHECK(GetBoolArg("-ABC", false));
    BOOST_CHECK(GetBoolArg("-ABC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ABCo"));
    BOOST_CHECK(!GetBoolArg("-ABCo", false));
    BOOST_CHECK(GetBoolArg("-ABCo", true));

    ResetArgs("-ABC=0");
    BOOST_CHECK(!GetBoolArg("-ABC"));
    BOOST_CHECK(!GetBoolArg("-ABC", false));
    BOOST_CHECK(!GetBoolArg("-ABC", true));

    ResetArgs("-ABC=1");
    BOOST_CHECK(GetBoolArg("-ABC"));
    BOOST_CHECK(GetBoolArg("-ABC", false));
    BOOST_CHECK(GetBoolArg("-ABC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noABC");
    BOOST_CHECK(!GetBoolArg("-ABC"));
    BOOST_CHECK(!GetBoolArg("-ABC", false));
    BOOST_CHECK(!GetBoolArg("-ABC", true));

    ResetArgs("-noABC=1");
    BOOST_CHECK(!GetBoolArg("-ABC"));
    BOOST_CHECK(!GetBoolArg("-ABC", false));
    BOOST_CHECK(!GetBoolArg("-ABC", true));

    ResetArgs("-ABC -noABC");  // -ABC should win
    BOOST_CHECK(GetBoolArg("-ABC"));
    BOOST_CHECK(GetBoolArg("-ABC", false));
    BOOST_CHECK(GetBoolArg("-ABC", true));

    ResetArgs("-ABC=1 -noABC=1");  // -ABC should win
    BOOST_CHECK(GetBoolArg("-ABC"));
    BOOST_CHECK(GetBoolArg("-ABC", false));
    BOOST_CHECK(GetBoolArg("-ABC", true));

    ResetArgs("-ABC=0 -noABC=0");  // -ABC should win
    BOOST_CHECK(!GetBoolArg("-ABC"));
    BOOST_CHECK(!GetBoolArg("-ABC", false));
    BOOST_CHECK(!GetBoolArg("-ABC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ABC=1");
    BOOST_CHECK(GetBoolArg("-ABC"));
    BOOST_CHECK(GetBoolArg("-ABC", false));
    BOOST_CHECK(GetBoolArg("-ABC", true));

    ResetArgs("--noABC=1");
    BOOST_CHECK(!GetBoolArg("-ABC"));
    BOOST_CHECK(!GetBoolArg("-ABC", false));
    BOOST_CHECK(!GetBoolArg("-ABC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ABC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ABC", "eleven"), "eleven");

    ResetArgs("-ABC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ABC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ABC", "eleven"), "");

    ResetArgs("-ABC=");
    BOOST_CHECK_EQUAL(GetArg("-ABC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ABC", "eleven"), "");

    ResetArgs("-ABC=11");
    BOOST_CHECK_EQUAL(GetArg("-ABC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ABC", "eleven"), "11");

    ResetArgs("-ABC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ABC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ABC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ABC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ABC", 0), 0);

    ResetArgs("-ABC -bar");
    BOOST_CHECK_EQUAL(GetArg("-ABC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ABC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ABC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ABC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ABC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ABC");
    BOOST_CHECK_EQUAL(GetBoolArg("-ABC"), true);

    ResetArgs("--ABC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ABC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noABC");
    BOOST_CHECK(!GetBoolArg("-ABC"));
    BOOST_CHECK(!GetBoolArg("-ABC", true));
    BOOST_CHECK(!GetBoolArg("-ABC", false));

    ResetArgs("-noABC=1");
    BOOST_CHECK(!GetBoolArg("-ABC"));
    BOOST_CHECK(!GetBoolArg("-ABC", true));
    BOOST_CHECK(!GetBoolArg("-ABC", false));

    ResetArgs("-noABC=0");
    BOOST_CHECK(GetBoolArg("-ABC"));
    BOOST_CHECK(GetBoolArg("-ABC", true));
    BOOST_CHECK(GetBoolArg("-ABC", false));

    ResetArgs("-ABC --noABC");
    BOOST_CHECK(GetBoolArg("-ABC"));

    ResetArgs("-noABC -ABC"); // ABC always wins:
    BOOST_CHECK(GetBoolArg("-ABC"));
}

BOOST_AUTO_TEST_SUITE_END()
