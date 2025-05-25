// Main demo program to demonstrate publisher-subscriber notification mechanism
#define _GNU_SOURCE  // For strdup
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "route_table.h"
#include "route_table_subscriber.h"

// Publisher thread function
void *publisher_thread_fn(void *arg) {
    printf("\n=== Publisher initializing routes ===\n");
    
    // Create initial route table entries
    route_table_node_t *node1 = create_route_table_node(strdup("192.168.1.2"), strdup("255.255.255.0"), strdup("eth0"), strdup("192.168.0.1"));
    route_table_node_t *node2 = create_route_table_node(strdup("192.168.1.3"), strdup("255.255.255.0"), strdup("eth0"), strdup("192.168.0.2"));
    route_table_node_t *node3 = create_route_table_node(strdup("192.168.1.4"), strdup("255.255.255.0"), strdup("eth0"), strdup("192.168.0.3"));

    // Add the entries to the route table
    add_route_table_node(node1);
    add_route_table_node(node2);
    add_route_table_node(node3);
    
    printf("Publisher thread created and entries added to the route table.\n");
    printf("=====================================\n\n");
    
    printf("Current route table:\n");
    print_route_table();

    // Interactive loop for route management
    while (1) {
        int choice;
        printf("\n=== Route Management Menu ===\n");
        printf("1. Add a new entry\n");
        printf("2. Delete an entry\n");
        printf("3. Modify an entry\n");
        printf("4. Print route table\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please try again.\n");
            // Clear input buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        
        switch (choice) {
            case 1: {
                char dest_addr[16], mask[16], oif[16], gateway[16];
                printf("Enter destination address: ");
                scanf("%s", dest_addr);
                printf("Enter mask: ");
                scanf("%s", mask);
                printf("Enter outgoing interface: ");
                scanf("%s", oif);
                printf("Enter gateway address: ");
                scanf("%s", gateway);
                
                route_table_node_t *new_node = create_route_table_node(strdup(dest_addr), strdup(mask), strdup(oif), strdup(gateway));
                add_route_table_node(new_node);
                printf("Route entry added successfully!\n");
                break;
            }
            case 2: {
                char dest_addr[16], mask[16];
                printf("Enter destination address to delete: ");
                scanf("%s", dest_addr);
                printf("Enter mask: ");
                scanf("%s", mask);
                
                // Use composite key (dest_addr + mask) to find the route
                route_table_node_t *node = get_route_table_node_by_dest_and_mask(dest_addr, mask);
                if (node) {
                    remove_route_table_node(node);
                    printf("Entry with destination %s/%s deleted.\n", dest_addr, mask);
                } else {
                    printf("No entry found with destination %s/%s.\n", dest_addr, mask);
                }
                break;
            }
            case 3: {
                char dest_addr[16], current_mask[16];
                printf("Enter destination address to modify: ");
                scanf("%s", dest_addr);
                printf("Enter current mask: ");
                scanf("%s", current_mask);
                
                // Use composite key (dest_addr + mask) to find the route
                route_table_node_t *node = get_route_table_node_by_dest_and_mask(dest_addr, current_mask);
                if (!node) {
                    printf("No entry found with destination %s/%s.\n", dest_addr, current_mask);
                    break;
                }
                
                printf("\nCurrent entry:\n");
                printf("Destination: %s, Mask: %s, OIF: %s, Gateway: %s\n",
                       node->dest_addr, node->mask, 
                       node->oif ? node->oif : "(null)", 
                       node->gateway ? node->gateway : "(null)");
                
                printf("\nWhich fields would you like to modify?\n");
                printf("1. Mask\n");
                printf("2. Outgoing Interface (OIF)\n");
                printf("3. Gateway\n");
                printf("4. All fields\n");
                printf("Enter your choice: ");
                
                int modify_choice;
                if (scanf("%d", &modify_choice) != 1) {
                    printf("Invalid input.\n");
                    break;
                }
                
                char new_mask[16] = {0}, new_oif[16] = {0}, new_gateway[16] = {0};
                char *mask_ptr = NULL, *oif_ptr = NULL, *gateway_ptr = NULL;
                
                switch (modify_choice) {
                    case 1:
                        printf("Enter new mask: ");
                        scanf("%s", new_mask);
                        mask_ptr = new_mask;
                        break;
                    case 2:
                        printf("Enter new outgoing interface: ");
                        scanf("%s", new_oif);
                        oif_ptr = new_oif;
                        break;
                    case 3:
                        printf("Enter new gateway address: ");
                        scanf("%s", new_gateway);
                        gateway_ptr = new_gateway;
                        break;
                    case 4:
                        printf("Enter new mask: ");
                        scanf("%s", new_mask);
                        printf("Enter new outgoing interface: ");
                        scanf("%s", new_oif);
                        printf("Enter new gateway address: ");
                        scanf("%s", new_gateway);
                        mask_ptr = new_mask;
                        oif_ptr = new_oif;
                        gateway_ptr = new_gateway;
                        break;
                    default:
                        printf("Invalid choice.\n");
                        break;
                }
                
                if (modify_choice >= 1 && modify_choice <= 4) {
                    if (modify_route_table_node(dest_addr, current_mask, mask_ptr, oif_ptr, gateway_ptr)) {
                        printf("Route entry modified successfully!\n");
                    } else {
                        printf("Failed to modify route entry.\n");
                    }
                }
                break;
            }
            case 4:
                printf("\nCurrent route table:\n");
                print_route_table();
                break;
            case 5:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    printf("DEBUG: Starting main function\n");
    fflush(stdout);
    
    (void)argc; (void)argv; // Suppress unused parameter warnings
    pthread_t pub_thread;
    
    printf("=== Publisher-Subscriber Notification Demo ===\n");
    
    printf("DEBUG: About to initialize route table\n");
    fflush(stdout);
    
    // Initialize route table
    route_table_instance_t* table = init_route_table("Demo Route Table");
    printf("DEBUG: init_route_table returned\n");
    fflush(stdout);
    
    set_route_table_head(table);
    printf("Route table initialized.\n");

    printf("DEBUG: About to create subscriber thread\n");
    fflush(stdout);
    
    // Create subscriber thread first
    create_subscriber_thread();
    printf("Subscriber thread created and initialized.\n");
    
    // Small delay to ensure output clarity
    sleep(1);
    
    // Create publisher thread
    pthread_create(&pub_thread, NULL, publisher_thread_fn, NULL);
    
    // Wait for publisher thread to complete
    pthread_join(pub_thread, NULL);
    
    printf("\nDemo completed.\n");
    return 0;
}
