///
/// @file SmartPot.hpp
///
/// @brief Class which represents the SmartPot actual functionalities
/// and methods.
///
#ifndef SMART_POT_HPP
#define SMART_POT_HPP

#include <map>
#include <vector>
#include <string>


using namespace std;

namespace pot 
{

// This is just some basic implementation I came up with at 4 A.M.,
// we may change this at our next meeting.
class SmartPot 
{
    public:
        SmartPot (void);
       ~SmartPot (void);

        int get  (const string& name, string& returnedValue);
        int set  (const string& name, const string& value);

        string status (void) const;


    private:
        map <string, string> settings;
};

}

#endif