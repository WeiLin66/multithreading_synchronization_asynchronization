/* In order to learn Notification chain mechanism, I'd like to write a simple 
    route table program. 
    
    @ The route table will be a simple linked list of routes.
    @ Each route table linked list node will contain the information of destination address, 
    Maskm Oif, gateway address and linked list head for subscriber nodes, glthread pointer to
    the next route table node.
*/

#ifndef ROUTE_TABLE_H
#define ROUTE_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include "../gtheard/glthread.h"

// Forward declarations for notification system
typedef enum{
    NFC_UNKNOWN,
    NFC_SUB,
    NFC_ADD,
    NFC_MOD,
    NFC_DEL,
} nfc_op_t;

typedef void (*nfc_app_cb)(void *, size_t, nfc_op_t, uint32_t);

typedef struct route_table_notif_elem {
    uint32_t subs_id;
    nfc_app_cb app_cb;
    glthread_t glue;
} route_table_notif_elem_t;

// Macro to convert glthread to route_table_notif_elem_t
GLTHREAD_TO_STRUCT(glue_to_notif_elem, route_table_notif_elem_t, glue);

// Route table node structure
typedef struct route_table_node {
    char *dest_addr; // Destination address
    char *mask;      // Mask
    char *oif;       // Outgoing interface
    char *gateway;   // Gateway address
    glthread_t subscriber_list; // Linked list of subscribers
    glthread_t list; // Double Linked list pointer to the next route table node
} route_table_node_t;

// Macro to convert glthread to route_table_node_t
GLTHREAD_TO_STRUCT(glue_to_route_node, route_table_node_t, list);

// Route table structure
typedef struct route_table {
    char* description; // Description of the route table
    glthread_t list;  // Head of the route table linked list
    int count;
} route_table_instance_t;

// APIs declaration
route_table_instance_t* init_route_table(char *description);
route_table_node_t *create_route_table_node(char *dest_addr, char *mask, char *oif, char *gateway);
void add_route_table_node(route_table_node_t *node);
void remove_route_table_node(route_table_node_t *node);
route_table_node_t *get_route_table_node(char *dest_addr);
route_table_node_t *get_route_table_node_by_gateway(char *gateway);
void print_route_table(void);
void free_route_table(void);
int count_route_table_nodes(void);
route_table_instance_t* get_route_table_head(void);
route_table_node_t *get_route_table_node_by_dest_and_mask(char *dest_addr, char *mask);
void notify_subscribers(route_table_node_t *node, nfc_op_t op);
int modify_route_table_node(char *dest_addr, char *current_mask, char *new_mask, char *new_oif, char *new_gateway);
void set_route_table_head(route_table_instance_t* table);

#endif // ROUTE_TABLE_H