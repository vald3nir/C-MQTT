/*
 ============================================================================
 Name        : main.c
 Author      : Vald3nir
 Version     : 1.0.0
 Copyright   : Vald3nir 2021
 Description : Client MQTT
 Library used: sudo apt-get install libmosquitto-dev
 ============================================================================
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <mosquitto.h>
#include <string.h>

// ==========================================================================
// MQTT Config
// ==========================================================================

#define BROKER_ADDRESS "localhost"
#define BROKER_PORT 1883
#define KEEP_ALIVE 60
#define QOS 0
#define RETAIN_MESSAGE 1
#define TOPIC_MQTT "/v1/topic_test"
const char *MESSAGE_MQTT = "this is a test!";

struct mosquitto *receiver_mqtt, *publisher_mqtt;

// ==========================================================================
// Prototypes
// ==========================================================================

void *publish_messages(void *args);

void on_connect_mqtt(struct mosquitto *mosq, void *obj, int result);

void on_message_mqtt(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

// ==========================================================================
// Main Function
// ==========================================================================

int main(int argc, char *argv[])
{

    mosquitto_lib_init();

    publisher_mqtt = mosquitto_new(NULL, true, NULL);
    receiver_mqtt = mosquitto_new(NULL, true, publisher_mqtt);

    mosquitto_connect_callback_set(receiver_mqtt, on_connect_mqtt);
    mosquitto_message_callback_set(receiver_mqtt, on_message_mqtt);

    mosquitto_connect(publisher_mqtt, BROKER_ADDRESS, BROKER_PORT, KEEP_ALIVE);
    mosquitto_connect(receiver_mqtt, BROKER_ADDRESS, BROKER_PORT, KEEP_ALIVE);

    mosquitto_loop_start(publisher_mqtt);

    pthread_t threads[2];
    pthread_create(&(threads[0]), NULL, &publish_messages, NULL);

    mosquitto_loop_forever(receiver_mqtt, -1, 1);

    mosquitto_destroy(receiver_mqtt);
    mosquitto_destroy(publisher_mqtt);
    mosquitto_lib_cleanup();
}

// ==========================================================================
// Implementations
// ==========================================================================

void *publish_messages(void *args)
{
    while (1)
    {
        puts("publish!");
        mosquitto_publish(publisher_mqtt, NULL, TOPIC_MQTT, strlen(MESSAGE_MQTT), MESSAGE_MQTT, QOS, RETAIN_MESSAGE);
        sleep(2);
    }
    return NULL;
}

void on_connect_mqtt(struct mosquitto *mosq, void *obj, int result)
{
    if (!result)
    {
        mosquitto_subscribe(mosq, NULL, TOPIC_MQTT, QOS);
    }
    else
    {
        fprintf(stderr, "%s\n", mosquitto_connack_string(result));
    }
}

void on_message_mqtt(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    printf("receive: %s\n", (char *)message->payload);
}
