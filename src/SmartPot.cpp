#include "SmartPot.hpp"


// Included in the header already, but safe than sorry :D
using namespace std;

namespace pot
{

SmartPot::SmartPot(void)
{
    settings["soilType"] = "Negru";
    // ...
}


SmartPot::~SmartPot(void)
{

}


///
/// @brief Gets a setting value.
///
/// @returns 1 for error, 0 for success besides the correct
/// @b returnedValue.
///
int SmartPot::get(const string& name, string& returnedValue)
{   
    // If the setting does not exist.
    if(settings.find(name) == settings.end())
    {
        returnedValue = "";
        return 1;
    } 
    else
    { 
        returnedValue = settings[name];
    }

    return 0;
}


///
/// @brief Sets a setting value.
///
/// @returns 1 for error, 0 for success.
///
int SmartPot::set(const string& name, const string& value)
{
    // If the setting does not exist.
    if(settings.find(name) == settings.end())
    {
        return 1;
    } 
    else
    { 
        settings[name] = value;
    }

    return 0;
}


// When the endpoint calls this, the function shall diagnose
// the pot based on its current settings and return a relevant
// string (we should discuss this though, may not be ideal).
// 
// I want to call this after I finish a json of settings updates.
// The small amount of settings we have allows us to iterate over
// all of them and see what status we have. We may do some mapping
// shennanigans like changing "soilType" -> {soilAlert, soilChange} and
// we'll only iterate only over these possible returns, but I think it's
// not needed.
string SmartPot::status (void) const
{
    // Whatever.
    return "";
}

}