#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/msg.h>

#include <clara/cmsg.h>
#include <clara/csession.h>
#include "omsg.h"

ssize_t oswin_recv_cmd(osession_node_t *session, long int target, void *msg, size_t size)
{
	ssize_t sz;
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);

	sz = msgrcv(session->session.cmd_queue, msg, size, target, IPC_NOWAIT);
	if (sz == -1) {
		if ((errno == ENOMSG) || (errno == EAGAIN))
			return 0;
		else
			return -1;
	}

	if (sz < CLARA_MSG_SIZE(clara_message_t)) {
		errno = EINVAL;
		return -1;
	}

	if (m->magic != CLARA_MSG_MAGIC) {
		errno = EINVAL;
		return -1;
	}
	
	return sz;

}

int oswin_send_event_reliable(osession_node_t *session, long int target, uint32_t type, void *msg, size_t size)
{
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);

	m->target = target;
	m->type = type;
	m->magic = CLARA_MSG_MAGIC;

	return msgsnd(session->session.event_queue, msg, size, 0);	
}

int oswin_send_event(osession_node_t *session, long int target, uint32_t type, void *msg, size_t size)
{
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);

	m->target = target;
	m->type = type;
	m->magic = CLARA_MSG_MAGIC;

	return msgsnd(session->session.event_queue, msg, size, IPC_NOWAIT);	
}
