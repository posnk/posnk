#include <stdint.h>
#include <sys/types.h>
#include <clara/cmsg.h>
#include "osession.h"

#ifndef __OSWIN_OMSG_H__
#define __OSWIN_OMSG_H__

ssize_t oswin_recv_cmd(osession_node_t *session, long int target, void *msg, size_t size);

int oswin_send_event_reliable(osession_node_t *session, long int target, uint32_t type, void *msg, size_t size);

int oswin_send_event(osession_node_t *session, long int target, uint32_t type, void *msg, size_t size);

int oswin_send_sync_ack(osession_node_t *session, uint32_t seq, int result);

int oswin_send_sync_ack_pl(osession_node_t *session, uint32_t seq, void *payload, size_t size);
#endif

