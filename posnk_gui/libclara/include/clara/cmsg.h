#include <stdint.h>
#include <sys/types.h>
#ifndef __CLARA_CMSG_H__
#define __CLARA_CMSG_H__

#define CLARA_MSG_MAGIC			(0xCAFEC007)
#define CLARA_MSG_BUF_SIZE		(256)

#define CLARA_MSG_SIZE(TyPe)		(sizeof(TyPe) - sizeof(long int))

#define CLARA_MSG_TARGET_SESSION	(1)
#define CLARA_MSG_TARGET_SYNC_BIT	(1 << 30)

#define CLARA_MSG_SYNC_ACK		(0)

typedef struct {
	long int	target;
	uint32_t	magic;
	uint32_t	type;
	uint32_t	seq;
} clara_message_t;

typedef union {
	clara_message_t msg;
	uint8_t		pad[CLARA_MSG_BUF_SIZE];
} clara_msg_buffer_t;

typedef struct {
	clara_message_t	msg;	
	int		result;
	int		payload_handle;
	size_t		payload_size;	
} clara_sync_ack_msg_t;

int clara_send_cmd_sync(long int target, uint32_t type, void *msg, size_t size);
void *clara_send_cmd_sync_pl(long int target, uint32_t type, void *msg, size_t size);
int clara_send_cmd_async(long int target, uint32_t type, void *msg, size_t size);
ssize_t clara_recv_ev_block(long int target, void *msg, size_t size);
ssize_t clara_recv_ev(long int target, void *msg, size_t size);

#endif

