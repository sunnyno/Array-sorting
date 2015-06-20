#include <algorithm>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <sstream>

using namespace boost::archive;
using namespace std;

int const ARRAY_SIZE = 1000;

void sort(boost::array<int, ARRAY_SIZE> &A)
{
    sort(begin(A), end(A));
}

string serialize(boost::array<int, ARRAY_SIZE> &a)
{
    stringstream ss;
    text_oarchive oa{ss};
    oa << a;
    return ss.str() + '\n';
};

boost::array<int, ARRAY_SIZE> const deserialize(const string &message){
    stringstream ss;

    ss.str(message);
    boost::archive::text_iarchive ia(ss);
    boost::array<int, ARRAY_SIZE> a;
    ia >> a;
    return a;
};

string const convert_array(const boost::array<char, 4 * ARRAY_SIZE> &buf)
{
    string message = "";

    for(auto chr: buf){
        if(chr == '\n') break;
        message += chr;
    }

    return message;
};