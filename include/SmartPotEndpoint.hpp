///
/// @file SmartPotEndpoint.hpp
///
/// @brief Class which represents the HTTP and MQTT endpoints for
/// the @b SmartPot class.
///
#ifndef SMART_POT_ENDPOINT_HPP
#define SMART_POT_ENDPOINT_HPP

#include "SmartPot.hpp"

#include <iostream>
#include <signal.h>
// Our HTTP library.
#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <pistache/common.h>
// Our MQTT library.
#include <mosquitto.h>

using namespace std;
using namespace Pistache;
using Lock = mutex;
using Guard = lock_guard<Lock>;

namespace pot
{

    class SmartPotEndpoint
    {
    public:
        SmartPotEndpoint(Address address);
        ~SmartPotEndpoint(void);

        // Server initialization.
        void init(void);

        // Server start.
        void start(void);

        // Server stop.
        void stop(void);

    private:
        void getSetting(const Rest::Request &request,
                        Http::ResponseWriter response);

        void setSetting(const Rest::Request &request,
                        Http::ResponseWriter response);

        void postSettingUpdate(const Rest::Request &request,
                               Http::ResponseWriter response);

        void postPlantType(const Rest::Request &request,
                           Http::ResponseWriter response);

        void loosenSoil(const Rest::Request &request,
                        Http::ResponseWriter response);

        void changeSoil(const Rest::Request &request,
                        Http::ResponseWriter response);

        void irrigationSoil(const Rest::Request &request,
                            Http::ResponseWriter response);

        void activateSolarLamp(const Rest::Request &request,
                               Http::ResponseWriter response);

        void createHttpRoutes(void);

        // Our Endpoint for the http server thread.
        std::shared_ptr<Http::Endpoint> httpEndpoint;
        // The router for our HTTP routes.
        Rest::Router router;

        // Our MQTT Subscriber.
        //TODO: Change this to smart pointer.
        // int mosquittoID;
        struct mosquitto *mosquittoSub;

        // The actual smart pot.
        SmartPot smartPot;

        // Lock variable which prohibits the threads to concurrently edit
        // the same variable.
        Lock potLock;
    };

}

#endif