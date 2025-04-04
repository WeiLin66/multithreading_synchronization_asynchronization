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
#include "glthread.h/glthread.h"

// Route table node structure
struct route_table_node {
    char *dest_addr; // Destination address
    char *mask;      // Mask
    char *oif;       // Outgoing interface
    char *gateway;   // Gateway address
    struct glthread subscriber_list; // Linked list of subscribers
    struct glthread list; // Double Linked list pointer to the next route table node
};route_table_node_t;

// Route table structure
struct route_table {
    char* description; // Description of the route table
    struct route_table_node *head;
    int count;
} route_table_instance;

// APIs declaration
route_table_node_t *create_route_table_node(char *dest_addr, char *mask, char *oif, char *gateway);
void add_route_table_node(route_table_node_t *node);
void remove_route_table_node(route_table_node_t *node);
route_table_node_t *get_route_table_node(char *dest_addr);
route_table_node_t *get_route_table_node_by_gateway(char *gateway);
void print_route_table(void);
void free_route_table(void);
int count_route_table_nodes(void);

#endif // ROUTE_TABLE_H