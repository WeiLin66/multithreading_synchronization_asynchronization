#define _GNU_SOURCE  // For strdup
#include "route_table.h"
#include <stddef.h>

// Global variable to hold the route table head
static route_table_instance_t *route_table_head = NULL;

// Function to set the route table head
void set_route_table_head(route_table_instance_t* table) {
    route_table_head = table;
}

// Function to print the route table
void print_route_table(void) {
    if (!route_table_head) {
        printf("Route table is empty.\n");
        return;
    }
    printf("Route Table: %s\n", route_table_head->description);
    
    if (IS_GLTHREAD_LIST_EMPTY(&route_table_head->list)) {
        printf("No routes in table.\n");
        return;
    }
    
    glthread_t *curr;
    route_table_node_t *node;
    
    ITERATE_GLTHREAD_BEGIN(&route_table_head->list, curr) {
        node = glue_to_route_node(curr);
        printf("Destination: %s, Mask: %s, OIF: %s, Gateway: %s\n",
               node->dest_addr ? node->dest_addr : "(null)", 
               node->mask ? node->mask : "(null)", 
               node->oif ? node->oif : "(null)", 
               node->gateway ? node->gateway : "(null)");
    } ITERATE_GLTHREAD_END(&route_table_head->list, curr);
}

// Function to initialize the route table
route_table_instance_t* init_route_table(char *description) {
    route_table_instance_t *new_route_table = (route_table_instance_t *)malloc(sizeof(route_table_instance_t));
    if (!new_route_table) {
        perror("Failed to allocate memory for route table");
        exit(EXIT_FAILURE);
    }
    new_route_table->description = description;
    init_glthread(&new_route_table->list);
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
    init_glthread(&new_node->subscriber_list);

    return new_node;
}

// Function to add a new route table node to the route_table_head
// Implements prefix-based uniqueness (dest_addr + mask as composite key)
void add_route_table_node(route_table_node_t *new_node) {
    if (!route_table_head) {
        printf("Route table is not initialized. Initializing now...\n");
        route_table_head = init_route_table("Default Route Table");
    }
    
    // Check if route with same prefix already exists
    route_table_node_t *existing = get_route_table_node_by_dest_and_mask(new_node->dest_addr, new_node->mask);
    if (existing) {
        printf("INFO: Route entry %s/%s already exists. Updating existing entry.\n", 
               new_node->dest_addr, new_node->mask);
        
        // Update existing entry instead of creating duplicate
        if (new_node->oif) {
            if (existing->oif) free(existing->oif);
            existing->oif = strdup(new_node->oif);
        }
        if (new_node->gateway) {
            if (existing->gateway) free(existing->gateway);
            existing->gateway = strdup(new_node->gateway);
        }
        
        // Notify subscribers about the modification
        notify_subscribers(existing, NFC_MOD);
        
        // Free the new node since we're not using it
        free(new_node->dest_addr);
        free(new_node->mask);
        if (new_node->oif) free(new_node->oif);
        if (new_node->gateway) free(new_node->gateway);
        free(new_node);
        return;
    }
    
    // Add new unique route entry
    glthread_add_next(&route_table_head->list, &new_node->list);
    route_table_head->count++;
    
    printf("INFO: Added new route entry: %s/%s via %s (OIF: %s)\n",
           new_node->dest_addr ? new_node->dest_addr : "(null)",
           new_node->mask ? new_node->mask : "(null)",
           new_node->gateway ? new_node->gateway : "(null)",
           new_node->oif ? new_node->oif : "(null)");
    
    // Notify subscribers about the new route
    notify_subscribers(new_node, NFC_ADD);
}

// Function to remove a route table node from the route_table_head
void remove_route_table_node(route_table_node_t *node) {
    if (!route_table_head || !node) {
        return;
    }
    
    // Notify subscribers before removal
    notify_subscribers(node, NFC_DEL);
    
    // Remove from glthread list
    remove_glthread(&node->list);
    
    free(node);
    route_table_head->count--;
}

// Function to get the route table node by destination address
route_table_node_t *get_route_table_node(char *dest_addr) {
    if (!route_table_head || !dest_addr) {
        return NULL;
    }
    
    glthread_t *curr;
    route_table_node_t *node;
    
    ITERATE_GLTHREAD_BEGIN(&route_table_head->list, curr) {
        node = glue_to_route_node(curr);
        if (node->dest_addr && strcmp(node->dest_addr, dest_addr) == 0) {
            return node;
        }
    } ITERATE_GLTHREAD_END(&route_table_head->list, curr);
    
    return NULL;
}

// Function to get the route table node by gateway address
route_table_node_t *get_route_table_node_by_gateway(char *gateway) {
    if (!route_table_head || !gateway) {
        return NULL;
    }
    
    glthread_t *curr;
    route_table_node_t *node;
    
    ITERATE_GLTHREAD_BEGIN(&route_table_head->list, curr) {
        node = glue_to_route_node(curr);
        if (node->gateway && strcmp(node->gateway, gateway) == 0) {
            return node;
        }
    } ITERATE_GLTHREAD_END(&route_table_head->list, curr);
    
    return NULL;
}

// Function to get the route table by both destination address and mask
route_table_node_t *get_route_table_node_by_dest_and_mask(char *dest_addr, char *mask) {
    if (!route_table_head || !dest_addr || !mask) {
        return NULL;
    }
    
    glthread_t *curr;
    route_table_node_t *node;
    
    ITERATE_GLTHREAD_BEGIN(&route_table_head->list, curr) {
        node = glue_to_route_node(curr);
        if (node->dest_addr && node->mask && 
            strcmp(node->dest_addr, dest_addr) == 0 && 
            strcmp(node->mask, mask) == 0) {
            return node;
        }
    } ITERATE_GLTHREAD_END(&route_table_head->list, curr);
    
    return NULL;
}

// Function to free the route table
void free_route_table(void) {
    if (!route_table_head) {
        return;
    }
    
    glthread_t *curr, *next;
    route_table_node_t *node;
    
    // Use delete safe iteration
    for(curr = BASE(&route_table_head->list); curr != NULL; curr = next) {
        next = curr->right;
        node = glue_to_route_node(curr);
        
        // Free dynamically allocated string fields (dest_addr, mask, oif, gateway)
        free(node->dest_addr);
        free(node->mask);
        free(node->oif);
        free(node->gateway);
        free(node);
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

// Function to notify all subscribers
void notify_subscribers(route_table_node_t *node, nfc_op_t op) {
    if (!node) return;
    
    glthread_t *curr;
    route_table_notif_elem_t *subscriber;
    
    ITERATE_GLTHREAD_BEGIN(&node->subscriber_list, curr) {
        subscriber = glue_to_notif_elem(curr);
        if (subscriber->app_cb) {
            subscriber->app_cb(node, sizeof(route_table_node_t), op, subscriber->subs_id);
        }
    } ITERATE_GLTHREAD_END(&node->subscriber_list, curr);
}

// Function to modify a route table node
// Uses dest_addr + current_mask as composite key for lookup
int modify_route_table_node(char *dest_addr, char *current_mask, char *new_mask, char *new_oif, char *new_gateway) {
    if (!route_table_head || !dest_addr || !current_mask) {
        printf("ERROR: Invalid parameters for modify operation\n");
        return 0; // Failed - invalid parameters
    }
    
    // Find the node to modify using composite key (dest_addr + current_mask)
    route_table_node_t *node = get_route_table_node_by_dest_and_mask(dest_addr, current_mask);
    if (!node) {
        printf("ERROR: Route entry %s/%s not found\n", dest_addr, current_mask);
        return 0; // Failed - node not found
    }
    
    // Update mask if provided
    if (new_mask) {
        free(node->mask);
        node->mask = strdup(new_mask);
    }
    
    // Update OIF if provided
    if (new_oif) {
        free(node->oif);
        node->oif = strdup(new_oif);
    }
    
    // Update gateway if provided
    if (new_gateway) {
        free(node->gateway);
        node->gateway = strdup(new_gateway);
    }
    
    printf("INFO: Modified route entry: %s/%s via %s (OIF: %s)\n",
           node->dest_addr ? node->dest_addr : "(null)",
           node->mask ? node->mask : "(null)",
           node->gateway ? node->gateway : "(null)",
           node->oif ? node->oif : "(null)");
    
    // Notify subscribers about the modification
    notify_subscribers(node, NFC_MOD);
    
    return 1; // Success
}