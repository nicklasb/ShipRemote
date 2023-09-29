#include "robusto_pubsub_client.h"
#include <robusto_message.h>
#include <robusto_peer.h>
#include <robusto_network_service.h>
#include <robusto_pubsub.h>
#include <robusto_queue.h>
#include <string.h>

static uint16_t pubsub_conversation_id = 1;

static char *pubsub_client_log_prefix;

static void incoming_callback(robusto_message_t *message);
static void shutdown_callback();


network_service_t pubsub_client_service = {
    .incoming_callback = &incoming_callback,
    .service_name = "Pub-Sub service",
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

subscribed_topic_t *find_subscribed_topic_by_topic_hash(int32_t topic_hash)
{
    subscribed_topic_t *curr_topic = first_subscribed_topic;
    while (curr_topic)
    {
        if (curr_topic->topic_hash == topic_hash)
        {
            return curr_topic;
        }
        curr_topic = curr_topic->next;
    }
    return NULL;
}

void incoming_callback(robusto_message_t *message)
{
    ROB_LOGI(pubsub_client_log_prefix, "Got pubsub data from %s peer", message->peer->name);
    rob_log_bit_mesh(ROB_LOG_INFO, pubsub_client_log_prefix, message->binary_data, message->binary_data_length);
    if (message->binary_data[0] == PUBSUB_SUBSCRIBE_RESPONSE)
    {
        subscribed_topic_t *curr_topic = find_subscribed_topic_by_conversation_id(message->conversation_id);
        if (curr_topic)
        {
            memcpy(&curr_topic->topic_hash, message->binary_data + 1, 4);
            ROB_LOGI(pubsub_client_log_prefix, "Topic hash set to %lu", curr_topic->topic_hash); 
        } else {
            ROB_LOGE(pubsub_client_log_prefix, "Could not find matching topic for conversation_id %u!", message->conversation_id);
        }
    } else {
        ROB_LOGE(pubsub_client_log_prefix, "Unhandled pub sub byte %hhu!", message->binary_data[0]);
    }
}


void shutdown_callback()
{
}

rob_ret_val_t robusto_pubsub_client_publish(subscribed_topic_t * topic, uint8_t * data, int16_t data_length) {

    if ((topic->peer != NULL) && (topic->peer->state != PEER_UNKNOWN)) {
        uint8_t * request = robusto_malloc(data_length +5);
        request[0] = PUBSUB_PUBLISH;
        memcpy(request + 1, &topic->topic_hash, sizeof(topic->topic_hash));
        memcpy(request + 5, data, data_length);
        send_message_binary(topic->peer, PUBSUB_SERVER_ID, 0, request, data_length + 1, NULL);
        return ROB_OK;
    } else {
        ROB_LOGW(pubsub_client_log_prefix, "Could not send to NMEA gateway, not initiated or peer still unknown.");
        return ROB_FAIL;
    }
}

subscribed_topic_t * robusto_pubsub_client_subscribe(robusto_peer_t * peer, char * topic_name, subscription_cb * callback)
{
    uint16_t conversation_id = pubsub_conversation_id++;
    subscribed_topic_t *new_topic = robusto_malloc(sizeof(subscribed_topic_t));
    new_topic->topic_name = topic_name;
    new_topic->conversation_id = conversation_id;
    new_topic->next = NULL;
    new_topic->peer = peer;
    new_topic->topic_hash = 0;
    new_topic->callback = callback;
    if (!first_subscribed_topic)
    {
        first_subscribed_topic = new_topic;
        last_subscribed_topic = new_topic;
    }
    char *msg;
    uint16_t data_length = asprintf(&msg, " %s", topic_name);
    msg[0] = PUBSUB_SUBSCRIBE;
    ROB_LOGE(pubsub_client_log_prefix, "Sending subscription for conversation_id %u!", conversation_id);
    send_message_binary(peer, PUBSUB_SERVER_ID, conversation_id, (uint8_t *)msg, data_length, NULL);
    if (!robusto_waitfor_uint32_t_change(&new_topic->topic_hash, 4000))
    {
        ROB_LOGE(pubsub_client_log_prefix, "Pub sub client: Subscription failed, no response with topic hash.");
        return NULL;
    }
    else
    {
        return new_topic;
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
