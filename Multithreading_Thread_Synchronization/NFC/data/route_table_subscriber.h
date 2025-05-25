#ifndef __ROUTE_TABLE_SUBSCRIBER__
#define __ROUTE_TABLE_SUBSCRIBER__

#include <stddef.h>  /* for size_t */
//#include <utils.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include "route_table.h"
#include "../gtheard/glthread.h"

#define MAX_NOTIF_KEY_SIZE	64

static inline char *nfc_get_str_op_code(nfc_op_t nfc_op_code) {
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

void create_subscriber_thread(void);

#endif /* __ROUTE_TABLE_SUBSCRIBER__ */