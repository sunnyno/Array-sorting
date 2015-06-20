#define BOOST_TEST_DYN_LINK        // this is optional
#define BOOST_TEST_MODULE Boost   // specify the name of your test module
#include <boost/test/included/unit_test.hpp>  // include this to get main()
#include "../functions/funcs.h"

using namespace std;

bool testSubstr(string str, string sub)
{
    return str.find(sub) < str.length() - 1;
}

bool lowToUp(boost::array<int, ARRAY_SIZE> arg)
{
    for(int i = 0; i < ARRAY_SIZE - 1; i++)
        if(arg[i] > arg[i + 1])
            return false;
    return true;
}

BOOST_AUTO_TEST_CASE(_test_serialize)
{
        boost::array<int, ARRAY_SIZE> c;
        for(int i = 0; i < ARRAY_SIZE; i++)
            c[i] = ARRAY_SIZE - i;

        BOOST_CHECK_MESSAGE(testSubstr(serialize(c), "4 3 2 1"), "1st subtest failed");
        BOOST_CHECK_MESSAGE(testSubstr(serialize(c), "8 7 6"), "2nd subtest failed");
}

/* create second test case template
   checking our function for finding our parameters in serialize data */
BOOST_AUTO_TEST_CASE(_test_sort)
{
        boost::array<int, ARRAY_SIZE> c;
        for(int i = 0; i < ARRAY_SIZE; i++)
            c[i] = ARRAY_SIZE - i;
        sort(c);
        BOOST_CHECK_EQUAL(lowToUp(c), true);
}