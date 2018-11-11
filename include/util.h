#ifndef __UTIL_H__
#define __UTIL_H__
#include <string>
#include <sstream>

using namespace std;
inline static int string_to_int(string val) 
{
        int x;
        stringstream ss;
        ss << hex << val;
        ss >> x;
        return x;
}

#endif
