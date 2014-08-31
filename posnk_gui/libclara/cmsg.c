#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include <clara/cmsg.h>
#include <clara/csession.h>

uint16_t clara_sync_seq = 0;

int clara_send_cmd_sync(long int target, uint32_t type, void *msg, size_t size)
{
	int st;
	ssize_t nr;
	clara_sync_ack_msg_t reply_msg;
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);
	
	m->seq = clara_sync_seq++;

	st = clara_send_cmd_async(target, type, msg, size);
	if (st == -1)
		return st;	

	nr = clara_recv_ev_block(((long int)m->seq) | CLARA_MSG_TARGET_SYNC_BIT, &reply_msg, CLARA_MSG_SIZE(clara_sync_ack_msg_t));

	if (nr == -1)
		return (int) nr;
	
	if (nr != CLARA_MSG_SIZE(clara_sync_ack_msg_t)){
		errno = EINVAL;
		return -1;
	}
	
	if (reply_msg.msg.type != CLARA_MSG_SYNC_ACK){
		errno = EINVAL;
		return -1;
	}

	return reply_msg.result;
}

void *clara_send_cmd_sync_pl(long int target, uint32_t type, void *msg, size_t size)
{
	void *payload_copy, *payload;
	int st;
	ssize_t nr;
	clara_sync_ack_msg_t reply_msg;
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);
	
	m->seq = clara_sync_seq++;

	st = clara_send_cmd_async(target, type, msg, size);
	if (st == -1)
		return NULL;	

	nr = clara_recv_ev_block(((long int)m->seq) | CLARA_MSG_TARGET_SYNC_BIT, &reply_msg, CLARA_MSG_SIZE(clara_sync_ack_msg_t));

	if (nr == -1)
		return NULL;
	
	if (nr != CLARA_MSG_SIZE(clara_sync_ack_msg_t)){
		errno = EINVAL;
		return NULL;
	}
	
	if (reply_msg.msg.type != CLARA_MSG_SYNC_ACK){
		errno = EINVAL;
		return NULL;
	}

	payload_copy = malloc(reply_msg.payload_size);
	assert( payload_copy != NULL );

	payload = shmat(reply_msg.payload_handle, NULL, 0);
	assert( payload != -1 );

	memcpy(payload_copy, payload, reply_msg.payload_size);

	shmdt(payload);

	shmctl(reply_msg.payload_handle, IPC_RMID, NULL);

	return payload_copy;
}

int clara_send_cmd_async(long int target, uint32_t type, void *msg, size_t size)
{
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);

	m->target = target;
	m->type = type;
	m->magic = CLARA_MSG_MAGIC;

	return msgsnd(clara_client_session.cmd_queue, msg, size, 0);	
}

ssize_t clara_recv_ev_block(long int target, void *msg, size_t size)
{
	ssize_t sz;
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);

	sz = msgrcv(clara_client_session.event_queue, msg, size, target, 0);
	if (sz == -1)
		return -1;

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

ssize_t clara_recv_ev(long int target, void *msg, size_t size)
{
	ssize_t sz;
	clara_message_t *m = (clara_message_t *) msg;
	assert(m != NULL);

	sz = msgrcv(clara_client_session.event_queue, msg, size, target, IPC_NOWAIT);
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
