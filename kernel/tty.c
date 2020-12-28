/**
 * kernel/tty.c
 *
 * Part of P-OS kernel.
 *
 * Written by Peter Bosch <peterbosc@gmail.com>
 *
 * @{
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
#include "config.h"

tty_info_t *tty_list [256];
int         tty_count[256];
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


int tty_open( dev_t device, stream_ptr_t *fd, int options )			//device, fd, options
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
	fdp->proc = current_process;
	llist_add_end( &tty->fds, ( llist_t *) fdp );
	if (!(options & O_NOCTTY)) {
		if (( current_process->pid == current_process->sid) && !current_process->ctty) {
			current_process->ctty = device;
			tty->ct_pid = current_process->pid;
			tty->fg_pgid = current_process->pgid;
		}
	}

	if ( tty->ops->open )
        tty->ops->open( tty );

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
	    //TODO: Fix CTTY handling
	    if ( tty->ops->close )
	        tty->ops->close( tty );
		//TODO: process_strip_ctty(device);
		if (device == current_process->ctty)
			current_process->ctty = 0;
	}
	return 0;
}

static void tty_notify_poll( tty_info_t *tty )
{
    tty_fd_t *fdp;
    llist_t *_fdp;
    for ( _fdp = tty->fds.next; _fdp !=& tty->fds; _fdp = _fdp->next ){
                assert(_fdp != NULL );
        fdp = ( tty_fd_t *) _fdp;
        stream_notify_poll( fdp->ptr->info );
    }
}

/**
 * Add string to input pipe
 */
void tty_queue_input_str( tty_info_t *tty, const char *c)
{
	aoff_t w;
	pipe_write(tty->pipe_in, c, strlen(c), &w, 1);
	tty_notify_poll( tty );
}

/**
 * Add character to input pipe
 */
void tty_queue_input_char( tty_info_t *tty, char c)
{
	aoff_t w;
	pipe_write(tty->pipe_in, &c, 1, &w, 1);
    tty_notify_poll( tty );
}

/**
 * Flushes the input line buffer into the input pipe
 * @param tty
 */
void tty_flush_line_buffer(tty_info_t *tty){
	aoff_t w;
	pipe_write(tty->pipe_in, tty->line_buffer, tty->line_buffer_pos, &w, 1);
	tty->line_buffer_pos = 0;
}

void tty_buf_line_char(tty_info_t *tty, char c)
{
	if ( tty->line_buffer_pos == CONFIG_TTY_BUFFER )
		tty_flush_line_buffer(tty);
	tty->line_buffer[tty->line_buffer_pos++] = c;
}

void tty_output_char( dev_t device, char c )
{
	tty_info_t *tty = tty_get(device);
	assert(tty);
	if ( tty->termios.c_oflag & OPOST ) { //TODO: Flow control
		if ((tty->termios.c_oflag & ONLCR) && (c == '\n')) {
			tty->ops->write_out(device, '\r');

		} else if ((tty->termios.c_oflag & OCRNL) && (c == '\r'))
			c = '\n';
		tty->ops->write_out(device, c);
	} else
		tty->ops->write_out(device, c);
}

void tty_input_str( dev_t device, const char *c )
{
	tty_info_t *tty = tty_get(device);
	assert(tty);
    if (tty->termios.c_lflag & ICANON) {
        while (*c)
            tty_input_char( device, *c++ );
    } else {
        tty_queue_input_str( tty, c );
    }
}

static void echo_char ( tty_info_t *tty, char c )
{
    aoff_t a;
    if (c == tty->termios.c_cc[VERASE] && tty->termios.c_lflag & ECHOE) {
        if (tty->line_buffer_pos) {
            tty->ops->write_out( tty->device, '\b' );
            tty->ops->write_out( tty->device, ' ' );
            tty->ops->write_out( tty->device, '\b' );
        }
    } else if (c == tty->termios.c_cc[VKILL] &&
               tty->termios.c_lflag & ECHOK) {
        for (a = 0; a < tty->line_buffer_pos; a++) {
            tty->ops->write_out( tty->device, '\b' );
            tty->ops->write_out( tty->device, ' ' );
            tty->ops->write_out( tty->device, '\b' );
        }
    } else if ( tty->termios.c_lflag & ECHO ||
               (c == '\n' && (tty->termios.c_lflag & ECHONL)))
        tty_output_char( tty->device, c );
}

static void canon_input_char( tty_info_t *tty, char c )
{
    struct siginfo info;
    memset( &info, 0, sizeof( struct siginfo ) );

    /* Strip high bit if ISTRIP set */
    if ( tty->termios.c_iflag & ISTRIP )
        c &= 0x7F;
    else if ( tty->termios.c_iflag & PARMRK &&
              ((unsigned char)c) == 0377u ) {
        /* If parity marking is enabled, escape 0377 */
        //XXX: This means 0377 cannot be a control char in this case
        tty_queue_input_char( tty, 0377 );
        tty_queue_input_char( tty, 0377 );
    }

    /* Translate newlines */
    if ( (tty->termios.c_iflag & INLCR) && (c == '\n') )
        c = '\r';
    else if ( (tty->termios.c_iflag & ICRNL) && (c == '\r') )
        c = '\n';

    /* Drop CR if IGNCR is set */
    if ( (tty->termios.c_iflag & IGNCR) && (c == '\r') )
        return;

    /* Echo characters */
    echo_char( tty, c );

    /* Handle control characters */
    if ( c == '\n' || c == tty->termios.c_cc[VEOL] ) {
        tty_buf_line_char( tty, c );
        tty_flush_line_buffer( tty );
    } else if ( c == tty->termios.c_cc[VERASE] ) {
        if ( tty->line_buffer_pos )
            tty->line_buffer_pos--;
    } else if ( c == tty->termios.c_cc[VKILL] ) {
        tty->line_buffer_pos = 0;
    } else if ( tty->termios.c_lflag & ISIG ) {
        if ( c == tty->termios.c_cc[VQUIT] ) {
            process_signal_pgroup( tty->fg_pgid, SIGQUIT, info );
        } else if ( c == tty->termios.c_cc[VINTR] ) {
            process_signal_pgroup( tty->fg_pgid, SIGINT, info );
        } else if ( c == tty->termios.c_cc[VSUSP] ) {
            process_signal_pgroup( tty->fg_pgid, SIGTSTP, info );
        } else
            tty_buf_line_char(tty, c);
    } else {
        tty_buf_line_char(tty, c);
    }
}

void tty_input_char(dev_t device, char c)
{
	tty_info_t *tty = tty_get(device);
	assert(tty);
	if (tty->termios.c_lflag & ICANON) { //TODO: Flow control
        canon_input_char( tty, c );

	} else {
	    if (tty->termios.c_lflag & ECHO) {
	        tty->ops->write_out(device, c);
	    }
        tty_queue_input_char( tty, c );
	}
}

int tty_input_queue_full( dev_t device )
{
    tty_info_t *tty = tty_get( device );
    return pipe_is_full( tty->pipe_in );
}

void tty_input_break( dev_t device, char c )
{
    tty_info_t *tty = tty_get(device);
            assert(tty);
    if ( tty->termios.c_iflag & IGNBRK )
        return;
    if ( tty->termios.c_iflag & BRKINT ) {
        //TODO: Handle BREAK interrupt
    } else if ( tty->termios.c_iflag & PARMRK ) {
        tty_queue_input_char( tty, 0377 );
        tty_queue_input_char( tty, 0000 );
        tty_queue_input_char( tty, 0000 );
    } else {
        tty_queue_input_char( tty, 0000 );
    }
}

void tty_input_error( dev_t device, char c )
{
    tty_info_t *tty = tty_get(device);
    assert(tty);
    if ( tty->termios.c_iflag & IGNPAR ) {

        /* Input the character as normal
         * (check if this should also apply to framing errors) */
        tty_input_char( device, c );

    } else if ( tty->termios.c_iflag & PARMRK ) {
        tty_queue_input_char( tty, 0377 );
        tty_queue_input_char( tty, 0000 );
    } else {
        tty_queue_input_char( tty, 0000 );
    }
}


int tty_write(dev_t device, const void *buf, aoff_t count, aoff_t *write_size, __attribute__((__unused__)) int non_block) //device, buf, count, wr_size, non_block
{
	aoff_t c;
	const char *b = buf;
	for (c = 0; c < count; c++)
		tty_output_char(device, b[c]);
	(*write_size) = count;
	return 0;
}

/**
 * @brief Read data from TTY device
 *
 * @param device The device to read from
 * @param buffer The buffer to read the data to
 * @oaram count The number of bytes to read
 * @param read_size Output parameter for the number of bytes actually read
 * @param non_block If true, the call will not block when no data is available
 * @return An error code on failure, 0 on success
 * @exception EINVAL The device is not suitable for reading
 */
int tty_read(
        dev_t device,
        void *buf,
        aoff_t count,
        aoff_t *read_size,
        int non_block)	//device, buf, count, rd_size, non_block
{
	tty_info_t *tty = tty_get(device);
	assert( tty != NULL );

	/* Read directly for the input pipe. */
	return pipe_read( tty->pipe_in, buf, count, read_size, non_block );
}

short int tty_poll(dev_t device, short int events ){
	tty_info_t *tty = tty_get(device);
	assert( tty != NULL );
	//TODO: Implement POLLHUP
	return pipe_poll( tty->pipe_in, events ) & POLLIN;
}

int tty_ioctl(
        dev_t device,
        __attribute__((__unused__)) int fd,
        int func,
        int arg )
{
	struct siginfo info;
	tty_info_t *tty;

	/* Get TTY to operate on */
	tty = tty_get(device);
	if ( tty == NULL ) {
	    syscall_errno = ENODEV;
	    return -1;
	}

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
            if ( tty->ops->termios_changed )
                tty->ops->termios_changed( tty );
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
			if ( (current_process->pid == current_process->sid) &&
			   ((current_process->ctty == device) || !current_process->ctty)) {
				current_process->ctty = device;
				tty->ct_pid = current_process->pid;
				tty->fg_pgid = current_process->pgid;
			}
			return 0;
		case TIOCNOTTY:
			if ( current_process->ctty != device )
				return 0;
			current_process->ctty = 0;
			if (current_process->pid == current_process->sid) {
				process_signal_pgroup(tty->fg_pgid, SIGCONT, info);
				process_signal_pgroup(tty->fg_pgid, SIGHUP, info);
				//TODO: Strip pgrp of ctty
			}
			return 0;

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

static char_ops_t tty_ops = {
        .open_new  = tty_open,
        .close_new = tty_close,
        .ioctl     = tty_ioctl,
        .read      = tty_read,
        .write     = tty_write,
        .poll      = tty_poll,
        .dup       = tty_dup
};

void tty_init(){
}

tty_info_t *tty_get(dev_t device)
{
    dev_t major = MAJOR(device);
    dev_t minor = MINOR(device);
    if ( tty_count[ major ] <= minor )
        return NULL;
    return &tty_list[major][minor];
}

void tty_register_driver( char *name, dev_t major, int minor_count, tty_ops_t *ops )
{
    dev_t c;
    tty_info_t *tty;
    char_dev_t *dev;

    /* Allocate minor devices */
    tty_count[ major ] = minor_count;
    tty_list [ major ] = heapmm_alloc( sizeof( tty_info_t ) * minor_count );
    assert( tty_list[major] != NULL );
    memset( tty_list[major], 0, sizeof( tty_info_t ) * minor_count );

    for (c = 0; c < (dev_t) minor_count; c++) {
        tty = &tty_list[ major ][ c ];

        /* Create the pipe providing the TTY input buffer */
        tty->pipe_in = pipe_create();
        pipe_open_read( tty->pipe_in );
        pipe_open_write( tty->pipe_in );

        /* Set the device number */
        tty->device = MAKEDEV( major, c );

        /* Start with an empty input buffer */
        tty->line_buffer_pos = 0;

        /* Keep track of open handles */
        tty->ref_count = 0;

        /* Initialize task management info */
        tty->fg_pgid = 0;
        tty->ct_pid = 0;

        /* Set character output callback */
        tty->ops = ops;

        /* Initialize TTY settings */
        tty_reset_termios(tty);

        llist_create( &tty->fds );
    }

    dev = heapmm_alloc(sizeof(char_dev_t));
    assert( dev != NULL );

    /* Setup character device */
    dev->major = major;
    dev->name = name;
    dev->ops = &tty_ops;
    device_char_register(dev);
}
/**
@}
 */