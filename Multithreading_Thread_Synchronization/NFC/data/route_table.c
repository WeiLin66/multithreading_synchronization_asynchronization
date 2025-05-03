#include "route_table.h"

// Global variable to hold the route table head
static struct route_table *route_table_head = NULL;

// Function to initialize the route table
route_table_instance_t* init_route_table(char *description) {
    route_table_instance_t *new_route_table = (route_table_instance_t *)malloc(sizeof(route_table_instance_t));
    if (!new_route_table) {
        perror("Failed to allocate memory for route table");
        exit(EXIT_FAILURE);
    }
    new_route_table->description = description;
    new_route_table->head = NULL;
    new_route_table->count = 0;
    return new_route_table;
}

// Function to create a new route table node
route_table_node_t *create_route_table_node(char *dest_addr, char *mask, char *oif, char *gateway) {
    route_table_node_t *new_node = (route_table_node_t *)malloc(sizeof(route_table_node_t));
    if (!new_node) {
        perror("Failed to allocate memory for route table node");
        exit(EXIT_FAILURE);
    }
    new_node->dest_addr = dest_addr;
    new_node->mask = mask;
    new_node->oif = oif;
    new_node->gateway = gateway;
    init_glthread(&new_node->list);
    new_node->list.right = NULL; // Ensure proper initialization
    new_node->list.left = NULL;  // Ensure proper initialization
    return new_node;
}

// Function to add a new route table node to the route_table_head
void add_route_table_node(route_table_node_t *new_node) {
    if (!route_table_head) {
        route_table_head = init_route_table("Default Route Table");
    }
    if (!route_table_head->head) {
        route_table_head->head = new_node;
    } else {
        glthread_add_last(&route_table_head->head->list, &new_node->list);
    }
    route_table_head->count++;
}

// Function to remove a route table node from the route_table_head
void remove_route_table_node(route_table_node_t *node) {
    if (!route_table_head || !node) {
        return;
    }
    glthread_remove(&node->list);
    free(node);
    route_table_head->count--;
}

// Functio to get the route table node by destination address
route_table_node_t *get_route_table_node(char *dest_addr) {
    if (!route_table_head) {
        return NULL;
    }
    route_table_node_t *curr = (route_table_node_t *)route_table_head->head;
    while (curr) {
        if (strcmp(curr->dest_addr, dest_addr) == 0) {
            return curr;
        }
        curr = (curr->list.right) ? (route_table_node_t *)curr->list.right : NULL;
    }
    return NULL;
}

// Functio to get the route table node by gateway address
route_table_node_t *get_route_table_node_by_gateway(char *gateway) {
    if (!route_table_head) {
        return NULL;
    }
    route_table_node_t *curr = (route_table_node_t *)route_table_head->head;
    while (curr) {
        if (strcmp(curr->gateway, gateway) == 0) {
            return curr;
        }
        curr = (route_table_node_t *)curr->list.right;
    }
    return NULL;
}

// Function to get the route table by both destination address and mask
route_table_node_t *get_route_table_node_by_dest_and_mask(char *dest_addr, char *mask) {
    if (!route_table_head) {
        return NULL;
    }
    route_table_node_t *curr = (route_table_node_t *)route_table_head->head;
    while (curr) {
        if (strcmp(curr->dest_addr, dest_addr) == 0 && strcmp(curr->mask, mask) == 0) {
            return curr;
        }
        curr = (route_table_node_t *)curr->list.right;
    }
    return NULL;
}

// Function to print the route table
void print_route_table(void) {
    if (!route_table_head) {
        printf("Route table is empty.\n");
        return;
    }
    route_table_node_t *curr = (route_table_node_t *)route_table_head->head;
    printf("Route Table: %s\n", route_table_head->description);
    while (curr) {
        printf("Destination: %s, Mask: %s, OIF: %s, Gateway: %s\n",
               curr->dest_addr, curr->mask, curr->oif, curr->gateway);
        curr = (route_table_node_t *)curr->list.right;
    }
}

// Function to free the route table
void free_route_table(void) {
    if (!route_table_head) {
        return;
    }
    route_table_node_t *curr = (route_table_node_t *)route_table_head->head;
    while (curr) {
        // Free dynamically allocated string fields (dest_addr, mask, oif, gateway)
        free(temp->dest_addr);
        free(temp->mask);
        free(temp->oif);
        free(temp->gateway);
        free(temp->oif);
        free(temp->gateway);
        free(temp);
    }
    if (route_table_head) {
        free(route_table_head);
        route_table_head = NULL;
    }
}

// Function to count the number of route table nodes
int count_route_table_nodes(void) {
    if (!route_table_head) {
        return 0;
    }
    return route_table_head->count;
}

// Function to get the route table head
route_table_instance_t* get_route_table_head(void) {
    return route_table_head;
}