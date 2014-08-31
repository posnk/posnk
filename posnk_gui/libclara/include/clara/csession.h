#include <stdint.h>
#include <sys/types.h>

#include <clara/cmsg.h>
#include <clara/cllist.h>
#include <clara/ctypes.h>

#ifndef __CLARA_CSESSION_H__
#define __CLARA_CSESSION_H__

#define CLARA_PROTOCOL_VER	(1)

#define CLARA_HANDLE_MAX		(CLARA_MSG_TARGET_SYNC_BIT - 1)
#define CLARA_HANDLE_MIN		(CLARA_MSG_TARGET_SESSION + 1)

#define CLARA_MSG_CONNECT	(1)
#define CLARA_MSG_CONN_ACC	(2)
#define CLARA_MSG_DISCONNECT	(3)
#define CLARA_MSG_CREATE_WIN	(4)
#define CLARA_MSG_POLL_DIMS	(9)
#define CLARA_MSG_WNDLIST	(10)

typedef struct {
	clara_message_t msg;
	uint32_t	handle;
} clara_createwin_msg_t;

typedef struct {
	clara_message_t msg;
} clara_disconnect_msg_t;

typedef struct {
	clara_message_t msg;
} clara_poll_dims_msg_t;

typedef struct {
	clara_message_t msg;
} clara_wndlist_msg_t;

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

typedef struct {
	uint32_t	flags;
	char		title[128];
	clara_rect_t	dimensions;
	int		focused;
} clara_wndlist_e_t;

typedef struct {
	int			entry_count;
	clara_wndlist_e_t	entries[1];
} clara_wndlist_t;

extern clara_session_t	clara_client_session;

int clara_init_client(const char *disp_path);

int clara_create_window();

void clara_exit_client();

clara_rect_t clara_get_screen_dims();

clara_wndlist_t *clara_list_windows();

#endif

