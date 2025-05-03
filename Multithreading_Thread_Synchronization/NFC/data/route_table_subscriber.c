#include "route_table_subscriber.h"

// app_cb function
// This function is a demonstration of a callback function
// that will be called when a notification is received
void app_callback(void *arg, size_t arg_size, nfc_op_t nfc_op_code, uint32_t client_id) {
    route_table_node_t *node = (route_table_node_t *)arg;
    printf("Notification received for subscriber %u with operation %s\n", client_id, nfc_get_str_op_code(nfc_op_code));
    printf("Route Table Entry: Destination: %s, Mask: %s, OIF: %s, Gateway: %s\n",
           node->dest_addr, node->mask, node->oif, node->gateway);
}

// function for inserting a new subscriber to the route table
// this functio will first check if the route table entry exists
// if it does, it will add the subscriber to the list of subscribers
// if it does not, it will create a new route table entry and add the subscriber to it
void add_subscriber_to_route_table(route_table_node_t *node, char *dest_addr, char *mask, nfc_app_cb app_cb, uint32_t subs_id) {
    if (!node) {
        printf("Route table entry does not exist. Creating a new entry.\n");
    }
    /* iterate through the route table and match the destination address and mask using glthread macros
     * rt_entry was not existing before, but we are
     * creating it because subscriber is interested in notif
     * for this entry. Create such an entry without data. Later
     * When publisher would actually cate this entry, all registered
     * subscriber should be notified
     * */
    bool new_entry_created = false;
    route_table_node_t *new_node = create_route_table_node(dest_addr, mask, NULL, NULL);
    if (!new_node) {
        new_entry_created = true;
        // create a new route table entry
        if((new_node = create_route_table_node(dest_addr, mask, NULL, NULL)) == NULL) {
            printf("Failed to create new route table entry.\n");
            return;
        }
    }
    // insert notification chain element
    route_table_notif_elem_t *new_nfce = calloc(1, sizeof(route_table_notif_elem_t));
    if (!new_nfce) {
        printf("Failed to allocate memory for new notification chain element.\n");
        return;
    }
    // assign cb and subs_id
    new_nfce->app_cb = app_cb; // assign the callback function
    new_nfce->subs_id = subs_id; // assign the subscriber ID

    // add the new notification chain element to the subscriber list
    init_glthread(&new_nfce->glue);
    glthread_add_next(&node->subscriber_list, &new_nfce->glue);

    // send notification to the subscriber if new notification chain element is insrted
    if (!new_entry_created) {
        new_nfce->app_cb(new_node, sizeof(route_table_node_t), NFC_ADD, subs_id);
    }
}

// Function to be executed by the publisher thread
// This function will subscribe to four different routes
void *publisher_thread_fn(void *arg) {
    add_subscriber_to_route_table(get_route_table_head()->head, "192.168.1.1", "255.255.255.0", app_callback, 1);
    add_subscriber_to_route_table(get_route_table_head()->head, "192.168.1.2", "255.255.255.0", app_callback, 2);
    add_subscriber_to_route_table(get_route_table_head()->head, "192.168.1.10", "255.255.255.0", app_callback, 3);
    add_subscriber_to_route_table(get_route_table_head()->head, "192.168.1.11", "255.255.255.0", app_callback, 4);
}

// Function to create a publisher thread
void create_publisher_thread() {
    pthread_t pub_thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&pub_thread, &attr, publisher_thread_fn, NULL);
    printf("Publisher thread created.\n");
    pthread_attr_destroy(&attr);
}