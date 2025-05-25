#define _GNU_SOURCE  // For strdup
#include "route_table_subscriber.h"

// app_cb function
// This function is a demonstration of a callback function
// that will be called when a notification is received
void app_callback(void *arg, size_t arg_size, nfc_op_t nfc_op_code, uint32_t client_id) {
    (void)arg_size; // Suppress unused parameter warning
    route_table_node_t *node = (route_table_node_t *)arg;
    
    printf("Notification received for subscriber %u with operation %s\n", 
           client_id, nfc_get_str_op_code(nfc_op_code));
    
    // Check if node is valid
    if (!node) {
        printf("ERROR: Node pointer is NULL in callback!\n");
        return;
    }
    
    printf("Route Table Entry: Destination: %s, Mask: %s, OIF: %s, Gateway: %s\n",
           node->dest_addr ? node->dest_addr : "(null)", 
           node->mask ? node->mask : "(null)", 
           node->oif ? node->oif : "(null)", 
           node->gateway ? node->gateway : "(null)");
}

// function for inserting a new subscriber to the route table
// this functio will first check if the route table entry exists
// if it does, it will add the subscriber to the list of subscribers
// if it does not, it will create a new route table entry and add the subscriber to it
void add_subscriber_to_route_table(char *dest_addr, char *mask, nfc_app_cb app_cb, uint32_t subs_id) {
    // Check if route entry with exact prefix already exists
    route_table_node_t *node = get_route_table_node_by_dest_and_mask(dest_addr, mask);
    bool new_entry_created = false;
    
    if (!node) {
        // Create placeholder route entry for this subscription
        printf("INFO: Creating placeholder route entry for subscriber %u: %s/%s\n", 
               subs_id, dest_addr, mask);
        node = create_route_table_node(
            strdup(dest_addr), 
            strdup(mask), 
            strdup("(placeholder)"),    // Placeholder OIF
            strdup("(placeholder)")     // Placeholder Gateway
        );
        add_route_table_node(node);
        new_entry_created = true;
    }
    
    // Create new subscriber element
    route_table_notif_elem_t *new_nfce = calloc(1, sizeof(route_table_notif_elem_t));
    if (!new_nfce) {
        printf("ERROR: Failed to allocate memory for subscriber\n");
        return;
    }
    new_nfce->app_cb = app_cb;
    new_nfce->subs_id = subs_id;
    init_glthread(&new_nfce->glue);
    
    // Add subscriber to the route's subscriber list
    glthread_add_next(&node->subscriber_list, &new_nfce->glue);
    
    printf("INFO: Subscriber %u registered for route %s/%s\n", subs_id, dest_addr, mask);
    
    // If this is an existing route (not placeholder), notify subscriber immediately
    if (!new_entry_created) {
        app_cb(node, sizeof(route_table_node_t), NFC_SUB, subs_id);
    }
}

// Function to be executed by the subscriber thread
// This function will subscribe to four different routes
void *subscriber_thread_fn(void *arg) {
    (void)arg; // Suppress unused parameter warning
    add_subscriber_to_route_table("192.168.1.1", "255.255.255.0", app_callback, 1);
    add_subscriber_to_route_table("192.168.1.2", "255.255.255.0", app_callback, 2);
    add_subscriber_to_route_table("192.168.1.10", "255.255.255.0", app_callback, 3);
    add_subscriber_to_route_table("192.168.1.11", "255.255.255.0", app_callback, 4);
    
    printf("Subscriber thread completed - 4 subscribers added\n");
    return NULL;
}

// Function to create a subscriber thread
void create_subscriber_thread() {
    pthread_t sub_thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&sub_thread, &attr, subscriber_thread_fn, NULL);
    printf("Subscriber thread created.\n");
    pthread_attr_destroy(&attr);
}