///
/// @file SmartPotEndpoint.hpp
///
/// @brief Class which represents the HTTP and MQTT endpoints for
/// the @b SmartPot class.
///
#include "SmartPotEndpoint.hpp"
#include "MQTTHelper.hpp"

// Our JSON Parser.
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <string>
#include <omp.h>

using namespace rapidjson;

namespace pot
{
    SmartPotEndpoint::SmartPotEndpoint(Address address)
    {
        // Create the HTTP Endpoint.
        httpEndpoint = std::make_shared<Http::Endpoint>(address);

        // Create the MQTT Subscriber.
        mosquitto_lib_init();
        //                            HostName   CleanSession SessionID
        mosquittoSub = mosquitto_new("SmartPot", true, NULL);
    }

    SmartPotEndpoint::~SmartPotEndpoint(void)
    {
        // Stop the HTTP server.
        httpEndpoint->shutdown();

        mosquitto_destroy(mosquittoSub);
        mosquitto_lib_cleanup();
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
                    //publish('test', smartPot.status())
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
        mosquitto_loop_stop(mosquittoSub, true);
        mosquitto_disconnect(mosquittoSub);
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

        Routes::Put(router, "/settings",
                    Routes::bind(&SmartPotEndpoint::postSettingUpdate, this));

        Routes::Put(router, "/plantType",
                    Routes::bind(&SmartPotEndpoint::postPlantType, this));

        Routes::Get(router, "/status",
                    Routes::bind(&SmartPotEndpoint::getStatus, this));

        Routes::Get(router, "/loosenSoil",
                    Routes::bind(&SmartPotEndpoint::loosenSoil, this));
    }

    ///
    /// @brief GET request function which returns the value of the
    /// desired setting found in the :settingName parameter.
    ///
    /// @returns A response with the setting name and value if the setting exists
    /// or a "Setting was not found" message otherwise.
    ///
    void SmartPotEndpoint::getSetting(const Rest::Request &request,
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

    void SmartPotEndpoint::postSettingUpdate(const Rest::Request &request,
                                             Http::ResponseWriter response)
    {
        // Lock the pot settings.
        // TODO: REVISE THIS
        Guard guard(potLock);

        // Setup some headers for the response.
        using namespace Http;
        using namespace rapidjson;

        response.headers()
            .add<Header::Server>("pistache/0.2")
            .add<Header::ContentType>(MIME(Text, Plain));

        Document document;
        if (document.Parse(request.body().c_str()).HasParseError() || document.IsObject() == false)
        {
            response.send(Http::Code::Unprocessable_Entity,
                          "The schema is not a valid JSON. Impossible to parse.");
        }

        string message = "";

        double sensorTypeID = document["sensorType"].GetDouble();

        /*
        1 - Ground sensor.
        2 - Temperature sensor
        3 - Luminosity sensor.
        4 - Humidity sensor.
        5 - Plant species.
        */

        double sensorMin = document["min"].GetDouble();
        double sensorMax = document["max"].GetDouble();
        // Valoarea o updatam in MQTT.
        message += to_string(sensorTypeID) + " " + to_string(sensorMin) + " " + to_string(sensorMax) + " ";

        if (document["nutrientType"].IsNull())
        {
            message += "NULL";
            // smartPot.set(sensorTypeID, "min", NULL, sensorMin);
            // smartPot.set(sensorTypeID, "max", NULL, sensorMin);
        }
        else
        {
            message += document["nutrientType"].GetString();
            // smartPot.set(sensorTypeID, "min", document["nutrientType"].GetString(), sensorMin);
            // smartPot.set(sensorTypeID, "max", document["nutrientType"].GetString(), sensorMin);
        }
    }

    void SmartPotEndpoint::postPlantType(const Rest::Request &request,
                                         Http::ResponseWriter response)
    {
        // Lock the pot settings.
        // TODO: REVISE THIS
        Guard guard(potLock);

        // Setup some headers for the response.
        using namespace Http;
        using namespace rapidjson;

        response.headers()
            .add<Header::Server>("pistache/0.2")
            .add<Header::ContentType>(MIME(Text, Plain));

        Document document;
        if (document.Parse(request.body().c_str()).HasParseError() || document.IsObject() == false)
        {
            response.send(Http::Code::Unprocessable_Entity,
                          "The schema is not a valid JSON. Impossible to parse.");
        }

        string message = "";

        double sensorTypeID = document["sensorType"].GetDouble();
        string species = document["species"].GetString();
        string color = document["color"].GetString();
        string type = document["type"].GetString();
        double height = document["height"].GetDouble();
        bool edible = document["edible"].GetBool();

        message += species + " " + color + " " + " ";

        // smartPot.set(sensorTypeID, "species", NULL, species); // String set.
        // smartPot.set(sensorTypeID, "color",   NULL, color);   // String set.
        // smartPot.set(sensorTypeID, "type",    NULL, type);    // String set.
        // smartPot.set(sensorTypeID, "height",  NULL, height);  // Double set.
        // smartPot.set(sensorTypeID, "edible",  NULL, edible);  // Bool   set.

        response.send(Http::Code::Ok, message);
    }

    void SmartPotEndpoint::getStatus(const Rest::Request &request,
                                     Http::ResponseWriter response)
    {
        string status = "";
        if (smartPot.getStatus(status))
        {
            response.send(Http::Code::Unprocessable_Entity, status);
        }
        else
        {
            response.send(Http::Code::Ok, status);
        }
    }

    void SmartPotEndpoint::loosenSoil(const Rest::Request &request,
                                      Http::ResponseWriter response)
    {
        string soilLooseningStatus = "";
        if (smartPot.loosenSoil(soilLooseningStatus))
        {
            response.send(Http::Code::Unprocessable_Entity, soilLooseningStatus);
        }
        else
        {
            response.send(Http::Code::Ok, soilLooseningStatus);
        }
    }

}