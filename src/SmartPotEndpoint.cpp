///
/// @file SmartPotEndpoint.hpp
///
/// @brief Class which represents the HTTP and MQTT endpoints for
/// the @b SmartPot class.
///
#include "SmartPotEndpoint.hpp"
#include "MQTTHelper.hpp"

#include <string>
#include <omp.h>


namespace pot
{
SmartPotEndpoint::SmartPotEndpoint(Address address)
{
    // Create the HTTP Endpoint.
    httpEndpoint = std::make_shared<Http::Endpoint>(address);

    // Create the MQTT Subscriber.
    mosquitto_lib_init();
    //                            HostName   CleanSession SessionID 
    mosquittoSub = mosquitto_new("SmartPot", true,        NULL);
}


SmartPotEndpoint::~SmartPotEndpoint(void)
{
    mosquitto_destroy     (mosquittoSub);
	mosquitto_lib_cleanup ();
}


///
/// @brief Server initialization.
///
void SmartPotEndpoint::init(void)
{
    // Get the optimal settings for our HTTP endpoint.
    auto settings = Http::Endpoint::options();
    httpEndpoint->init(settings);
    // Create the http routes we'll use.
    createHttpRoutes();

    // Setup MQTT function calls for connection and received messages.
    mosquitto_connect_callback_set(mosquittoSub, mqtt::mosquittoOnConnect);
	mosquitto_message_callback_set(mosquittoSub, mqtt::mosquittoOnMessage);
}


///
/// @brief Servers start.
///
void SmartPotEndpoint::start(void)
{
    // Start the parallel region for the HTTP and MQTT servers.
    #pragma omp parallel sections
    {
        // The HTTP server.
        #pragma omp section
        {   
            // cout << "HTTP " << omp_get_thread_num() << endl;
            httpEndpoint->setHandler(router.handler());
            httpEndpoint->serveThreaded();
        }

        // The MQTT server.
        #pragma omp section
        {
            // cout << "MQTT " << omp_get_thread_num() << endl;
            if (mosquitto_connect(mosquittoSub, "localhost", 1883, 60))
            {
                std::cout << "Could not connect to MQTT broker." << endl;
            }
            else
            {
                mosquitto_loop_start(mosquittoSub);
            }
        }
    }
}


///
/// @brief Server stop.
///
void SmartPotEndpoint::stop(void)
{
    // Stop the HTTP server.
    httpEndpoint->shutdown();

    // Stop the MQTT server and disconnect from the broker.
    mosquitto_loop_stop  (mosquittoSub, true);
    mosquitto_disconnect (mosquittoSub);
}


///
/// @brief Function which creates our http routes, called at 
/// server initialization.
///
void SmartPotEndpoint::createHttpRoutes(void)
{
    using namespace Rest;

    Routes::Get(router, "/settings/:settingName/",
                Routes::bind(&SmartPotEndpoint::getSetting, this));

    Routes::Put(router, "/settings/:settingName/:value",
                Routes::bind(&SmartPotEndpoint::setSetting, this)); 
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
    // TODO: REVISE THIS.
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
        std::cout << settingName + " is " + settingValue << endl;
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
    // TODO: REVISE THIS.
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
        std::cout << settingName + " is " + settingValue << endl;
    }
}


}