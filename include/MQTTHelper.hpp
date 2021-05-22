
#ifndef MQTT_HELPER_H
#define MQTT_HELPER_H

#include <mosquitto.h>

namespace mqtt
{
    void mosquittoOnConnect (struct mosquitto *mosq, void *obj, int rc);
    
    void mosquittoOnMessage (struct mosquitto *mosq,
                             void *obj, const struct mosquitto_message *msg);
}

#endif