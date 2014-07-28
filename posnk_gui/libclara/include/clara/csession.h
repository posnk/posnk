#include <stdint.h>
#include <sys/types.h>

#include <clara/cmsg.h>
#include <clara/cllist.h>

#ifndef __CLARA_CSESSION_H__
#define __CLARA_CSESSION_H__

#define CLARA_PROTOCOL_VER	(0)

#define CLARA_MSG_CONNECT	(1)
#define CLARA_MSG_CONN_ACC	(2)

typedef struct {
	clara_message_t	msg;
	pid_t		client_pid;
	int		event_queue;
} clara_connect_msg_t;

typedef struct {
	clara_message_t	msg;	
	int		protocol;
	pid_t		server_pid;	
} clara_conn_acc_msg_t;

typedef struct {
	pid_t		server_pid;
	pid_t		client_pid;
	int		cmd_queue;
	int		event_queue;
	cllist_t	window_list;	
} clara_session_t;

extern clara_session_t	clara_client_session;

int clara_init_client(const char *disp_path);

#endif

