///
/// @file SmartPotEndpoint.hpp
///
/// @brief Class which represents the HTTP and MQTT endpoints for
/// the @b SmartPot class.
///
#include "SmartPotEndpoint.hpp"

#include <string>


namespace pot
{

SmartPotEndpoint::SmartPotEndpoint(Address address)
{
    httpEndpoint = std::make_shared<Http::Endpoint>(address);
}


SmartPotEndpoint::~SmartPotEndpoint(void)
{

}


///
/// @brief Server initialization.
///
void SmartPotEndpoint::init(size_t threadCount)
{
    // Get the optimal settings for our @b threadCount.
    auto settings = Http::Endpoint::options().threads(threadCount);
    httpEndpoint->init(settings);

    // Create the routes we'll use.
    createRoutes();
}


///
/// @brief Server start.
///
void SmartPotEndpoint::start(void)
{
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serveThreaded();
}


///
/// @brief Server stop.
///
void SmartPotEndpoint::stop(void)
{
    httpEndpoint->shutdown();
}


///
/// @brief Function which creates our http routes, called at 
/// server initialization.
///
void SmartPotEndpoint::createRoutes(void)
{
    using namespace Rest;

    Routes::Get(router, "/test", 
                Routes::bind(&SmartPotEndpoint::testerFunction, this));

    Routes::Get(router, "/settings/:settingName/",
                Routes::bind(&SmartPotEndpoint::getSetting, this));

    Routes::Put(router, "/settings/:settingName/:value",
                Routes::bind(&SmartPotEndpoint::setSetting, this));
}


///
/// @brief Just a function to test if everything is OK.
///
void SmartPotEndpoint::testerFunction(const Rest::Request& request,
                                      Http::ResponseWriter response)
{   
    using namespace Http;
    response.headers()
                .add<Header::Server>("pistache/0.2")
                .add<Header::ContentType>(MIME(Text, Plain));

    response.send(Http::Code::Ok, "MERGE!");
}


///
/// @brief GET request function which returns the value of the
/// desired setting found in the :settingName parameter.
///
/// @returns A response with the setting name and value if the setting exists
/// or a "Setting was not found" message otherwise.
///
void SmartPotEndpoint::getSetting(const Rest::Request& request,
                                  Http::ResponseWriter response)
{

    // Lock the pot settings.
    // TODO: REVISE THIS SHIT.
    Guard guard(potLock);

    // Setup some headers for the response.
    using namespace Http;
    response.headers()
                .add<Header::Server>("pistache/0.2")
                .add<Header::ContentType>(MIME(Text, Plain));

    // Retrieve the setting name.
    string settingName = request.param(":settingName").as<string>();
    
    // Retrieve the setting value.
    string settingValue = "";
    // If it does NOT exist.
    if (smartPot.get(settingName, settingValue))
    {
        response.send(Http::Code::Not_Found, settingName + " was not found");
    }
    else
    {
        response.send(Http::Code::Ok, settingName + " is " + settingValue);
    }
    
}


///
/// @brief PUT request function which sets the value of the
/// desired setting found in the :settingName parameter.
///
/// @returns A response with the setting name and the set value if the setting exists
/// or a "Setting was not found" message otherwise.
///
void SmartPotEndpoint::setSetting(const Rest::Request& request,
                                  Http::ResponseWriter response)
{
    
    // Lock the pot settings. 
    // TODO: REVISE THIS SHIT.
    Guard guard(potLock);
    
    // Setup some headers for the response.
    using namespace Http;
    response.headers()
                .add<Header::Server>("pistache/0.1")
                .add<Header::ContentType>(MIME(Text, Plain));
    
    // Retrieve the setting name.
    string settingName = request.param(":settingName").as<string>();
    
    // Retrieve the setting value to be set.
    string settingValue = "";
    if (request.hasParam(":value"))
    {
        settingValue = request.param(":value").as<string>();
    }
    else //It should have went on the GET route, not this PUT one.
    {
        response.send(Http::Code::Not_Found, settingName + "Inexistant value parameter" +  
                      + " for the PUT setting route. Should have went the GET route");
        return;
    }

    // Set the desired setting.
    // If it does NOT exist.
    if (smartPot.set(settingName, settingValue))
    {
        response.send(Http::Code::Not_Found, settingName + " was not found and or '" 
                      + settingValue + "' was not a valid value ");
    }
    else
    {
        response.send(Http::Code::Ok, settingName + " was set to " + settingValue);
    }

}

}