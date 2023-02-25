
#include "main.hpp"
#include "mqtt.hpp"
#include "debug_indicator.hpp"

static context_t *pContext;

void mqtt_setContext(context_t *pCtx){
  pContext = pCtx;
}

void mqtt_sub_request_cb(void* arg, err_t result) {
  /* Just print the result code here for simplicity,
     normal behaviour would be to take some action if subscribe fails like
     notifying user, retry subscribe or disconnect from server */
  printf("Subscribe result: %d\n", result);
  indicatorWhite();
}

void mqtt_connect(mqtt_client_t* client) {
  struct mqtt_connect_client_info_t ci;
  err_t err;

  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));

  /* Minimal amount of information required is client identifier, so set it here */
  ci.client_id = "lwip_test";

  /* Initiate client and connect to server, if this fails immediately an error code is returned
     otherwise mqtt_connection_cb will be called with connection result after attempting
     to establish a connection with the server.
     For now MQTT version 3.1.1 is always used */
     // 192.168.1.150:1885"
  ip_addr_t mqtt_ip;
  ip4addr_aton("192.168.1.150", &mqtt_ip);

  cyw43_arch_lwip_begin();
  err = mqtt_client_connect(client, &mqtt_ip, 1885, mqtt_connection_cb, 0, &ci);
  cyw43_arch_lwip_end();

  /* For now just print the result code if something goes wrong*/
  if (err != ERR_OK) {
    printf("mqtt_connect return %d\n", err);
  }
}



void mqtt_incoming_data_cb(void* arg, const u8_t* data, u16_t len, u8_t flags) {
  printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

  if (flags & MQTT_DATA_FLAG_LAST) {
    /* Last fragment of payload received (or whole part if payload fits receive buffer
       See MQTT_VAR_HEADER_BUFFER_LEN)  */

    queue_entry_t entry = {};
    char request[50];
    strncpy(request, (const char*)data, len);
    request[len] = '\0';

    sscanf(request, "%[^:]:%s", &(entry.cmd), &(entry.cmddata));
    DEBUG_PRINT("[mqtt] %s %s\n", entry.cmd, entry.cmddata);  
    DEBUG_PRINT("[mqtt] %d\n", pContext->pQueue);  


    queue_add_blocking(pContext->pQueue, &entry);
    DEBUG_PRINT("[mqtt] added to queue\n");  
  } else {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
  }
}

void mqtt_connection_cb(mqtt_client_t* client, void* arg, mqtt_connection_status_t status) {
  err_t err;
  if (status == MQTT_CONNECT_ACCEPTED) {
    printf("mqtt_connection_cb: Successfully connected\n");

    // /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

    // /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
    err = mqtt_subscribe(client, "/display/study", 1, mqtt_sub_request_cb, arg);

    if (err != ERR_OK) {
      printf("mqtt_subscribe return: %d\n", err);
    }
  } else {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);

    /* Its more nice to be connected, so try to reconnect */
    mqtt_connect(client);
  }
}

    

void mqtt_incoming_publish_cb(void* arg, const char* topic, u32_t tot_len) {
  DEBUG_PRINT("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);
  // ironically we don't need to really do anything here
}
