/**
 * kernel/tty.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * Changelog:
 * 08-05-2014 - Created
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include "kernel/tty.h"
#include "kernel/scheduler.h"
#include "kernel/process.h"
#include "kernel/pipe.h"
#include "kernel/signals.h"
#include "kernel/streams.h"
#include "kernel/syscall.h"
#include "kernel/heapmm.h"
#include "kernel/device.h"
#include "kernel/earlycon.h"
#include "config.h"

tty_info_t ***tty_list;
tty_ops_t *tty_operations;

/*

	if (S_ISCHR(inode->mode) && !(flags & O_NOCTTY)) {
		if ((scheduler_current_task->pid == scheduler_current_task->sid) && !scheduler_current_task->ctty) {
			scheduler_current_task->ctty = inode->if_def; 
			
		}
	}
*/

void tty_reset_termios(tty_info_t *tty)
{
	tty->termios.c_iflag = ICRNL | IXON | BRKINT;
	tty->termios.c_oflag = OPOST | ONLCR | NL0 | CR0 | TAB0 | BS0 | VT0 | FF0;
	tty->termios.c_lflag = ECHO | ECHOE | ECHOK | ISIG | ICANON;
	tty->termios.c_cflag = CS8 | CREAD;
	tty->termios.c_ispeed = B9600;
	tty->termios.c_ospeed = B9600;
	tty->termios.c_cc[VINTR] = 3;//EOI
	tty->termios.c_cc[VQUIT] = 28;//FS
	tty->termios.c_cc[VERASE] = 8;//BS
	tty->termios.c_cc[VEOF] = 4;//EOT
	tty->termios.c_cc[VSUSP] = 26;//BS
	tty->termios.c_cc[VEOL] = 0;//BS
	tty->termios.c_cc[VKILL] = 1;//BS
	tty->termios.c_cc[VEOL] = 2;//BS
	tty->win_size.ws_col = 80;
	tty->win_size.ws_row = 25;
}

void tty_register_driver(char *name, dev_t major, int minor_count, tty_write_out_t write_out)
{
	dev_t c;
	tty_info_t *tty;
	char_dev_t *dev;
	tty_list[major] = heapmm_alloc(sizeof(tty_info_t *) * 256); 	
	for (c = 0; c < (dev_t) minor_count; c++) {
		tty = heapmm_alloc(sizeof(tty_info_t));
		tty->pipe_in = pipe_create();
		pipe_open_read(tty->pipe_in);
		pipe_open_write(tty->pipe_in);
		tty->device = MAKEDEV(major,c);
		tty->line_buffer = heapmm_alloc(CONFIG_TTY_BUFFER);
		tty->line_buffer_pos = 0;
		tty->line_buffer_size = CONFIG_TTY_BUFFER;
		tty->ref_count = 0;
		tty->fg_pgid = 0;
		tty->ct_pid = 0;
		tty->write_out = write_out;
		tty_reset_termios(tty);
		tty_list[major][c] = tty;
		llist_create( &tty->fds );
	}
	dev = heapmm_alloc(sizeof(char_dev_t));
	dev->major = major;
	dev->name = name;
	dev->ops = tty_operations;
	device_char_register(dev);
}

tty_info_t *tty_get(dev_t device)
{
	dev_t major = MAJOR(device);	
	dev_t minor = MINOR(device);
	tty_info_t *ttyi = tty_list[major][minor];
	return ttyi;
}


int tty_open(dev_t device, stream_ptr_t *fd, int options)			//device, fd, options
{
	tty_fd_t *fdp;
	tty_info_t *tty = tty_get(device);
	if ( !tty )
		return ENODEV;
	fdp = heapmm_alloc(sizeof(tty_fd_t));
	if ( fdp == NULL )
		return ENOMEM;
	tty->ref_count++;
	fdp->ptr = fd;
	fdp->proc = scheduler_current_task;
	llist_add_end( &tty->fds, ( llist_t *) fdp );
	if (!(options & O_NOCTTY)) {
		if ((scheduler_current_task->pid == scheduler_current_task->sid) && !scheduler_current_task->ctty) {
			scheduler_current_task->ctty = device; 
			tty->ct_pid = scheduler_current_task->pid;
			tty->fg_pgid = scheduler_current_task->pgid;
		}
	}
	return 0;
	
}

int tty_dup(dev_t device, stream_ptr_t *fd)			//device, fd, options
{
	return tty_open( device, fd, O_NOCTTY );
	
}

int tty_close(dev_t device, stream_ptr_t *fd)
{
	tty_fd_t *fdp;
	llist_t *_fdp, *_nfp;
	tty_info_t *tty = tty_get(device);
	assert(tty);
	tty->ref_count--;
	for ( _fdp = tty->fds.next; _fdp !=& tty->fds; _fdp = _nfp ){
		assert(_fdp != NULL );
		_nfp = _fdp->next;
		fdp = ( tty_fd_t *) _fdp;
		if ( fdp->ptr != fd )
			continue;
		llist_unlink(_fdp);
		heapmm_free(fdp,sizeof(tty_fd_t));
	}
	if (!tty->ref_count) {
		//TODO: process_strip_ctty(device);
		if (device == scheduler_current_task->ctty)
			scheduler_current_task->ctty = 0;
	}	
	return 0;
}

void tty_buf_out_char(tty_info_t *tty, char c)
{
	aoff_t w;
	tty_fd_t *fdp;
	llist_t *_fdp;
	pipe_write(tty->pipe_in, &c, 1, &w, 1);
	for ( _fdp = tty->fds.next; _fdp !=& tty->fds; _fdp = _fdp->next ){
		assert(_fdp != NULL );
		fdp = ( tty_fd_t *) _fdp;
		stream_notify_poll( fdp->ptr->info );
	}
}

void tty_flush_line_buffer(tty_info_t *tty){
	aoff_t w;
	pipe_write(tty->pipe_in, tty->line_buffer, tty->line_buffer_pos, &w, 1);
	tty->line_buffer_pos = 0;	
}

void tty_buf_line_char(tty_info_t *tty, char c)
{
	if (tty->line_buffer_pos == tty->line_buffer_size)
		tty_flush_line_buffer(tty);
	tty->line_buffer[tty->line_buffer_pos++] = c;
}

void tty_output_char(dev_t device, char c)
{
	tty_info_t *tty = tty_get(device);
	assert(tty);
	if (tty->termios.c_oflag & OPOST) { //TODO: Flow control
		if ((tty->termios.c_oflag & ONLCR) && (c == '\n')) {
			tty->write_out(device, '\r');
			
		} else if ((tty->termios.c_oflag & OCRNL) && (c == '\r'))
			c = '\n';
		tty->write_out(device, c);
	} else
		tty->write_out(device, c);
}

void tty_input_char(dev_t device, char c)
{
	struct siginfo info;
	aoff_t a;
	tty_info_t *tty = tty_get(device);
	assert(tty);
	memset( &info, 0, sizeof( struct siginfo ) );
	if (tty->termios.c_lflag & ICANON) { //TODO: Flow control
		if ((tty->termios.c_iflag & INLCR) && (c == '\n'))
			c = '\r';
		else if ((tty->termios.c_iflag & ICRNL) && (c == '\r'))
			c = '\n';
		if ((tty->termios.c_iflag & IGNCR) && (c == '\r'))
			return;
		if ((tty->termios.c_iflag & ISTRIP))
			c &= 0x7F;
		if ((c == tty->termios.c_cc[VERASE]) && (tty->termios.c_lflag & ECHOE)) {
			if ((tty->line_buffer_pos)) {
				tty->write_out(device, '\b');
				tty->write_out(device, ' ');
				tty->write_out(device, '\b');
			}
		} else if ((c == tty->termios.c_cc[VKILL]) && (tty->termios.c_lflag & ECHOK)) {
			for (a = 0; a < tty->line_buffer_pos; a++) {
				tty->write_out(device, '\b');
				tty->write_out(device, ' ');
				tty->write_out(device, '\b');
			}				
		} else if ((tty->termios.c_lflag & ECHO) || 
		    ((c == '\n') && (tty->termios.c_lflag & ECHONL)))
			tty->write_out(device, c);		
	
		if ((c == '\n') || (c == tty->termios.c_cc[VEOL])) {
			tty_buf_line_char(tty, c);
			tty_flush_line_buffer(tty);
		} else if (c == tty->termios.c_cc[VERASE]) {
			if (tty->line_buffer_pos)
				tty->line_buffer_pos--;			
		} else if (c == tty->termios.c_cc[VKILL]) {
			tty->line_buffer_pos = 0;			
		} else if (tty->termios.c_lflag & ISIG) {
			if (c == tty->termios.c_cc[VQUIT]) {
				process_signal_pgroup( tty->fg_pgid, SIGQUIT, info );
			} else if (c == tty->termios.c_cc[VINTR]) {
				process_signal_pgroup( tty->fg_pgid, SIGINT, info );
			} else if (c == tty->termios.c_cc[VSUSP]) {
				process_signal_pgroup( tty->fg_pgid, SIGTSTP, info );
			} else 
				tty_buf_line_char(tty, c);
		} else {
			tty_buf_line_char(tty, c);
		}			
	} else if (tty->termios.c_lflag & ECHO) {
		tty->write_out(device, c);
		tty_buf_out_char(tty, c);
	} else {
		tty_buf_out_char(tty, c);
	}
}

int tty_write(dev_t device, void *buf, aoff_t count, aoff_t *write_size, __attribute__((__unused__)) int non_block) //device, buf, count, wr_size, non_block
{
	aoff_t c;
	char *b = buf;
	for (c = 0; c < count; c++)
		tty_output_char(device, b[c]);
	(*write_size) = count;
	return 0;
}

int tty_read(dev_t device, void *buf, aoff_t count, aoff_t *read_size, int non_block)	//device, buf, count, rd_size, non_block
{
	tty_info_t *tty = tty_get(device);
	assert(tty);
	return pipe_read(tty->pipe_in, buf, count, read_size, non_block);
}

short int tty_poll(dev_t device, short int events ){
	tty_info_t *tty = tty_get(device);
	assert(tty);
	//TODO: Implement POLLHUP
	return pipe_poll(tty->pipe_in,events) & POLLIN;
}

int tty_ioctl(dev_t device, __attribute__((__unused__)) int fd, int func, int arg)			//device, fd, func, arg
{
	struct siginfo info;
	tty_info_t *tty = tty_get(device);
	assert(tty);
	memset( &info, 0, sizeof( struct siginfo ) );
	switch(func) {
		case TCGETS:
			if (!copy_kern_to_user(&(tty->termios), (void *) arg, sizeof(termios_t))){
				syscall_errno = EFAULT;
				return -1;
			}
			return 0;
		case TCSETSW://For now, we don't have an output buffer...
		case TCSETSF://Or an input buffer for that matter
		case TCSETS:
			if (!copy_user_to_kern((void *) arg, &(tty->termios), sizeof(termios_t))){
				syscall_errno = EFAULT;
				return -1;
			}
			//TODO: Call driver
			return 0;

		case TIOCGPGRP:
			if (!copy_kern_to_user(&(tty->fg_pgid), (void *) arg, sizeof(pid_t))){
				syscall_errno = EFAULT;
				return -1;
			}		
			return 0;
		case TIOCSPGRP:
			if (!copy_user_to_kern((void *) arg, &(tty->fg_pgid), sizeof(pid_t))){
				syscall_errno = EFAULT;
				return -1;
			}		
			return 0;

		case TIOCSCTTY:
			if ((scheduler_current_task->pid == scheduler_current_task->sid) && ((scheduler_current_task->ctty == device) || !scheduler_current_task->ctty)) {
				scheduler_current_task->ctty = device; 
				tty->ct_pid = scheduler_current_task->pid;
				tty->fg_pgid = scheduler_current_task->pgid;
			}
			return 0;
		case TIOCNOTTY:
			if (scheduler_current_task->ctty != device) 
				return 0;
			scheduler_current_task->ctty = 0;
			if (scheduler_current_task->pid == scheduler_current_task->sid) {
				process_signal_pgroup(tty->fg_pgid, SIGCONT, info);
				process_signal_pgroup(tty->fg_pgid, SIGHUP, info);
				//TODO: Strip pgrp of ctty
			}

		case TIOCGWINSZ:
			if (!copy_kern_to_user(&(tty->win_size), (void *) arg, sizeof(winsize_t))){
				syscall_errno = EFAULT;
				return -1;
			}		
			return 0;

		case TIOCSWINSZ:
			if (!copy_user_to_kern((void *) arg, &(tty->win_size), sizeof(winsize_t))){
				syscall_errno = EFAULT;
				return -1;
			}		
			//TODO: process_signal_pgroup(tty->fg_pgid, SIGWINCH);
			return 0;
						
			
		default:
			return 0;				
	}
}

void tty_init(){
	tty_list = heapmm_alloc(sizeof(tty_info_t **) * 256);
	tty_operations = heapmm_alloc(sizeof(tty_ops_t));
	tty_operations->open = NULL;
	tty_operations->open_new = &tty_open;
	tty_operations->close_new = &tty_close;
	tty_operations->ioctl = &tty_ioctl;
	tty_operations->read = &tty_read;
	tty_operations->write = &tty_write;
	tty_operations->poll = &tty_poll;
	tty_operations->dup = &tty_dup;
}
