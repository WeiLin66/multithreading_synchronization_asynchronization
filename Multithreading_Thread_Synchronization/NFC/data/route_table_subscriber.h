#ifndef __ROUTE_TABLE_SUBSCRIBER__
#define __ROUTE_TABLE_SUBSCRIBER__

#include <stddef.h>  /* for size_t */
#include "utils.h"
#include "gluethread/glthread.h"

#define MAX_NOTIF_KEY_SIZE	64

typedef enum{
	NFC_UNKNOWN,
	NFC_SUB,
	NFC_ADD,
	NFC_MOD,
	NFC_DEL,
} nfc_op_t;

static inline char *nfc_get_str_op_code(nfc_op_t nfc_op_code) {
	static char op_code_str[16];

	switch(nfc_op_code) {

		case NFC_UNKNOWN:
			return "NFC_UNKNOWN";
		case NFC_SUB:
			return "NFC_SUB";
		case NFC_ADD:
			return "NFC_ADD";
		case NFC_MOD:
			return "NFC_MOD";
		case NFC_DEL:
			return "NFC_DEL";
		default:
			return NULL;
	}
}

typedef void (*nfc_app_cb)(void *, size_t, nfc_op_t, uint32_t);

typedef struct route_table_notif_elem {
    char key[MAX_NOTIF_KEY_SIZE]; // Destination address
    size_t key_size;
    uint32_t subs_id; // Subscriber ID
    bool is_key_set;
    void (*app_cb)(void *, size_t, uint32_t); // Callback function
    glthread_t glue; // Glue thread for linked list
} route_table_notif_elem_t;


#endif /* __ROUTE_TABLE_SUBSCRIBER__ */