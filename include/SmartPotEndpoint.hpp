///
/// @file SmartPotEndpoint.hpp
///
/// @brief Class which represents the HTTP and MQTT endpoints for
/// the @b SmartPot class.
///
#ifndef SMART_POT_ENDPOINT_HPP
#define SMART_POT_ENDPOINT_HPP

#include "SmartPot.hpp"

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <pistache/common.h>
#include <signal.h>


using namespace std;
using namespace Pistache;
using Lock  = mutex;
using Guard = lock_guard<Lock>;

namespace pot 
{
    
class SmartPotEndpoint
{
    public:
        SmartPotEndpoint    (Address address);
       ~SmartPotEndpoint    (void); 

        // Server initialization.
        void init           (size_t threadCount = 2);

        // Server start.
        void start          (void);

        // Server stop.
        void stop           (void);


    private:
        void createRoutes   (void);

        void testerFunction (const Rest::Request& request,
                             Http::ResponseWriter response);

        void getSetting     (const Rest::Request& request,
                             Http::ResponseWriter response);

        void setSetting     (const Rest::Request& request,
                             Http::ResponseWriter response);

        // Our Endpoint for the http server thread.
        std::shared_ptr <Http::Endpoint> httpEndpoint;

        // The router for our routes.
        Rest::Router router;

        // The actual smart pot.
        SmartPot smartPot;

        // Lock variable which prohibits the threads to concurrently edit
        // the same variable.
        Lock potLock;
};

}

#endif