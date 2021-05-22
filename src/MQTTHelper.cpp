
#include "MQTTHelper.hpp"

#include <iostream>
#include <string>

using namespace std;

namespace mqtt
{


void mosquittoOnConnect (struct mosquitto *mosq, void *obj, int rc)
{
    std::cout << string("ID: ") + to_string(* (int *) obj) << endl;

	if(rc)
    {
		std::cout << string("Error with result code: ") +  to_string(rc) << endl;
	}

    // Subscribe to our "endpoint" topics.
    mosquitto_subscribe(mosq, NULL, "test", 0);
}


void mosquittoOnMessage (struct mosquitto *mosq,
                         void *obj, const struct mosquitto_message *msg)
{
	std::cout << string("New message with topic ")
               + string(msg->topic) + ". Message : "
               + string((char *) msg->payload) << endl; // We need to endl or flush the buffer.
}


}