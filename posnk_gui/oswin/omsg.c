#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>

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

int oswin_send_sync_ack(osession_node_t *session, uint32_t seq, int result)
{
	clara_sync_ack_msg_t msg;
	assert(session != NULL);

	msg.msg.target = seq | CLARA_MSG_TARGET_SYNC_BIT;
	msg.msg.type = CLARA_MSG_SYNC_ACK;
	msg.msg.magic = CLARA_MSG_MAGIC;
	msg.msg.seq = seq;
	msg.result = result;

	return msgsnd(session->session.event_queue, &msg, CLARA_MSG_SIZE(clara_sync_ack_msg_t), 0);	
}

int oswin_send_sync_ack_pl(osession_node_t *session, uint32_t seq, void *payload, size_t size)
{
	void *payload_shm;
	clara_sync_ack_msg_t msg;
	assert(session != NULL);

	msg.msg.target = seq | CLARA_MSG_TARGET_SYNC_BIT;
	msg.msg.type = CLARA_MSG_SYNC_ACK;
	msg.msg.magic = CLARA_MSG_MAGIC;
	msg.msg.seq = seq;
	msg.payload_size = size;

	msg.payload_handle = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT);
	assert (msg.payload_handle != -1);

	payload_shm = shmat(msg.payload_handle, NULL, 0);	
	assert (payload_shm != NULL);

	memcpy(payload_shm, payload, size);

	shmdt(payload_shm);

	return msgsnd(session->session.event_queue, &msg, CLARA_MSG_SIZE(clara_sync_ack_msg_t), 0);	
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
