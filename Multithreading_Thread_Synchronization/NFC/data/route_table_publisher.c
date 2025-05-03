// This is a publisher program using route_table.c and route_table.h
#include <stdio.h>
#include <stdlib.h>
#include "route_table.h"

// Global route table instance
route_table_instance_t *route_table_head = NULL;

// publisher thread handler
// This thread will insert three entries to route table
void *publisher_thread_fn(void *arg) {
    // Create three route table entries
    route_table_node_t *node1 = create_route_table_node("192.168.1.2", "255.255.255.0", "eth0", "192.168.0.1");
    route_table_node_t *node2 = create_route_table_node("192.168.1.3", "255.255.255.0", "eth0", "192.168.0.2");
    route_table_node_t *node3 = create_route_table_node("192.168.1.4", "255.255.255.0", "eth0", "192.168.0.3");

    // Add the entries to the route table
    add_route_table_node(node1);
    add_route_table_node(node2);
    add_route_table_node(node3);
    printf("Publisher thread created and entries added to the route table.\n");
    printf("Route table entries:\n");
    print_route_table();

    // enter infinite loop and allow user to 1. add new entries, 2. delete entries, 3. print route table 4. exit
    while (1) {
        int choice;
        printf("Enter 1 to add a new entry, 2 to delete an entry, 3 to print the route table, 4 to exit: ");
        scanf("%d", &choice);
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
                route_table_node_t *new_node = create_route_table_node(dest_addr, mask, oif, gateway);
                add_route_table_node(new_node);
                break;
            }
            case 2: {
                char dest_addr[16];
                printf("Enter destination address to delete: ");
                scanf("%s", dest_addr);
                route_table_node_t *node = get_route_table_node(dest_addr);
                if (node) {
                    remove_route_table_node(node);
                    printf("Entry with destination address %s deleted.\n", dest_addr);
                } else {
                    printf("No entry found with destination address %s.\n", dest_addr);
                }
                break;
            }
            case 3:
                print_route_table();
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return NULL
}

// Function to create a publisher thread
void create_publisher_thread(void) {
    ptherad_t pub_thread;
    ptherad_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_DETACHED);

    pthread_create(&pub_thread, &attr, publisher_thread_fn, NULL);
    printf("Publisher thread created.\n");
    pthread_attr_destroy(&attr);
}
    
int main (int arc, char *argv[]) {
    // initialize route table
    route_table_head = init_route_table("Route Table");

    // create a publisher thread
    create_publisher_thread();

    // delay for a while to allow the publisher thread to run
    sleep(5);

    // create a subscriber thread

    pthread_exit(NULL);

    return 0;
}