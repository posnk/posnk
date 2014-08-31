#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h> //For error reporting
#include <string.h> //For memory functions and error reporting
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <unistd.h>
#include <clara/cllist.h>
#include <clara/cmsg.h>
#include <clara/csession.h>
#include <clara/cwindow.h>

clara_session_t clara_client_session;
uint32_t	clara_client_handle_ctr = CLARA_HANDLE_MIN;

int clara_init_client(const char *disp_path)
{
	uint32_t cmdq;
	int cmdq_id, evq_id, fifo_fd;
	ssize_t nw;
	clara_connect_msg_t conn_msg;
	clara_conn_acc_msg_t reply_msg;

	fifo_fd = open(disp_path, O_WRONLY);
	if (fifo_fd == -1) {
		fprintf(stderr, "libclara: could not open display %s : %s\n", disp_path, strerror(errno));
		return -1;
	}

	cmdq_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	if (cmdq_id == -1) {
		fprintf(stderr, "libclara: could not create command message queue: %s\n", strerror(errno));
		goto bailout_cmdq;
	}

	evq_id = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	if (evq_id == -1) {
		fprintf(stderr, "libclara: could not create event message queue: %s\n", strerror(errno));
		goto bailout_evq;
	}
	
	clara_client_session.event_queue = evq_id;
	clara_client_session.cmd_queue = cmdq_id;
	clara_client_session.client_pid = getpid();	

	conn_msg.client_pid = clara_client_session.client_pid;
	conn_msg.event_queue = clara_client_session.event_queue;

	nw = clara_send_cmd_async(CLARA_MSG_TARGET_SESSION, CLARA_MSG_CONNECT, &conn_msg, CLARA_MSG_SIZE(clara_connect_msg_t));
	if (nw == -1) {
		fprintf(stderr, "libclara: could not send connect message: %s\n", strerror(errno));
		goto bailout_send;
	}

	cmdq = cmdq_id;

	nw = write(fifo_fd, &cmdq, sizeof(uint32_t));
	if (nw == -1) {
		fprintf(stderr, "libclara: could write connect request: %s\n", strerror(errno));
		goto bailout_conn;
	} else if (nw != sizeof(uint32_t)) {
		fprintf(stderr, "libclara: could write full connect request\n");
		goto bailout_conn;
	}
	
	close(fifo_fd);
	fifo_fd = -1;

	nw = clara_recv_ev_block(CLARA_MSG_TARGET_SESSION, &reply_msg, CLARA_MSG_SIZE(clara_conn_acc_msg_t));
	if (nw == -1) {
		fprintf(stderr, "libclara: could receive connection acknowledgement: %s\n", strerror(errno));
		goto bailout_recv;
	} else if (nw != CLARA_MSG_SIZE(clara_conn_acc_msg_t)) {
		fprintf(stderr, "libclara: could receive full connection acknowledgement\n");
		goto bailout_recv;
	} else if (reply_msg.msg.type != CLARA_MSG_CONN_ACC) {
		fprintf(stderr, "libclara: received invalid packet type while waiting for conn acc: %i\n", reply_msg.msg.type);
		goto bailout_recv;
	} else if (reply_msg.protocol != CLARA_PROTOCOL_VER) {
		fprintf(stderr, "libclara: display %s uses incompatible protocol version %i\n", disp_path, reply_msg.protocol);
		goto bailout_recv;
	}
	
	clara_client_session.server_pid = reply_msg.server_pid;
	cllist_create(&(clara_client_session.window_list));

	return 0;

bailout_recv:
bailout_conn:
bailout_send:
	msgctl(evq_id, IPC_RMID, NULL);
	
bailout_evq:
	msgctl(cmdq_id, IPC_RMID, NULL);	

bailout_cmdq:
	if (fifo_fd != -1)
		close(fifo_fd);
	return -1;
	
}

int clara_create_window()
{
	clara_createwin_msg_t cw_msg;
	int st;
	uint32_t handle = clara_client_handle_ctr++;
	
	cw_msg.handle = handle;

	st = clara_send_cmd_sync(CLARA_MSG_TARGET_SESSION, CLARA_MSG_CREATE_WIN, &cw_msg, CLARA_MSG_SIZE(clara_createwin_msg_t));

	if (st < 0)
		return st;

	clara_window_add(&clara_client_session, handle);

	return (int) handle;	
}

clara_rect_t clara_get_screen_dims()
{
	clara_rect_t d;
	clara_poll_dims_msg_t cw_msg;
	int st;

	st = clara_send_cmd_sync(CLARA_MSG_TARGET_SESSION, CLARA_MSG_POLL_DIMS, &cw_msg, CLARA_MSG_SIZE(clara_poll_dims_msg_t));

	d.w = -1;
	d.h = -1;

	if (st < 0)
		return d;

	d.x = 0;
	d.y = 0;
	d.w = st & 0xFFFF;
	d.h = (st >> 16) & 0xFFFF;

	return d;	
}

clara_wndlist_t *clara_list_windows()
{
	clara_wndlist_msg_t cw_msg;
	clara_wndlist_t *list;

	list = (clara_wndlist_t *) clara_send_cmd_sync_pl(CLARA_MSG_TARGET_SESSION, CLARA_MSG_WNDLIST, &cw_msg, CLARA_MSG_SIZE(clara_wndlist_msg_t));

	return list;	
}



void clara_exit_client()
{
	int s;
	clara_disconnect_msg_t msg;
	s = clara_send_cmd_async(CLARA_MSG_TARGET_SESSION, CLARA_MSG_DISCONNECT, &msg, CLARA_MSG_SIZE(clara_disconnect_msg_t));
	if (s == -1) {
		msgctl(clara_client_session.cmd_queue, IPC_RMID, NULL);
		msgctl(clara_client_session.event_queue, IPC_RMID, NULL);
	}
}

