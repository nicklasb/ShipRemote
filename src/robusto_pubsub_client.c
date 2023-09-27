#include "robusto_pubsub_client.h"
#include <robusto_message.h>
#include <robusto_peer.h>
#include <robusto_network_service.h>
#include <robusto_pubsub.h>
#include <robusto_queue.h>
#include <string.h>

static uint16_t pubsub_conversation_id = 0;

static char *pubsub_client_log_prefix;

static void incoming_callback(robusto_message_t *message);
static void shutdown_callback();

typedef struct subscribed_topic subscribed_topic_t;

struct subscribed_topic
{
    uint32_t topic_hash;
    char *topic_name;
    uint16_t conversation_id;
    subscribed_topic_t *next;
};

static subscribed_topic_t *first_subscribed_topic;
static subscribed_topic_t *last_subscribed_topic;

network_service_t pubsub_client_service = {
    .incoming_callback = &incoming_callback,
    .service_name = "Pub-sub service",
    .service_id = PUBSUB_CLIENT_ID,
    .shutdown_callback = &shutdown_callback,
};

subscribed_topic_t *find_subscribed_topic_by_conversation_id(int16_t conversation_id)
{
    subscribed_topic_t *curr_topic = first_subscribed_topic;
    while (curr_topic)
    {
        if (curr_topic->conversation_id == conversation_id)
        {
            return curr_topic;
        }
        curr_topic = curr_topic->next;
    }
    return NULL;
}

void incoming_callback(robusto_message_t *message)
{
    if (message->binary_data[0] == PUBSUB_SUBSCRIBE_RESPONSE)
    {
        subscribed_topic_t *curr_topic = find_subscribed_topic_by_conversation_id(message->conversation_id);
        if (curr_topic)
        {
            memcpy(&curr_topic->topic_hash, message->binary_data + 1, 4);
        }
    }
}

void shutdown_callback()
{
}

uint32_t robusto_pubsub_subscribe(robusto_peer_t *peer, char *topic_name)
{
    
    uint16_t conversation_id = pubsub_conversation_id++;
    subscribed_topic_t *new_topic = robusto_malloc(sizeof(subscribed_topic_t));
    new_topic->topic_name = topic_name;
    new_topic->conversation_id = conversation_id;
    new_topic->next = NULL;
    new_topic->topic_hash = 0;
    if (!first_subscribed_topic)
    {
        first_subscribed_topic = new_topic;
        last_subscribed_topic = new_topic;
    }
    char *msg;
    uint16_t data_length = asprintf(&msg, " %s", topic_name);
    msg[0] = PUBSUB_SUBSCRIBE;
    send_message_binary(peer, PUBSUB_SERVER_ID, conversation_id, (uint8_t *)msg, data_length, NULL);
    rob_ret_val_t q_retval;
    if (robusto_waitfor_uint32_t_change(&new_topic->topic_hash, 4000) != ROB_OK)
    {
        ROB_LOGE(pubsub_client_log_prefix, "Pub sub client: Subscription failed, no response with topic hash.");
        return ROB_FAIL;
    }
    else
    {
        return ROB_OK;
    }
}

rob_ret_val_t robusto_pubsub_client_start()
{
    // Start queue
    robusto_register_network_service(&pubsub_client_service);
    return ROB_OK;
};

rob_ret_val_t robusto_pubsub_client_init(char *_log_prefix)
{
    pubsub_client_log_prefix = _log_prefix;
    return ROB_OK;
};
