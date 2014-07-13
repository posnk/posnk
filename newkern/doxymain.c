/** \mainpage posnk project site
 *
 * \section intro_sec Introduction
 *
 * This is the website for posnk, an simple operating system written by Peter Bosch
 *
 * \ref fs_device FS driver API documentation\n
 * \ref vfs VFS documentation\n
 *
 */

/** \page downloads Downloads
 * 
 * \li \subpage small 
 * \li \subpage large
 *
 */

//-----------------------------------------------------------

/** \page small Small CD image
 * <a href="downloads/pos_small_july10_14.iso"> Download here (26MB)</a>
 *
 * \section cont Contents:
 *
 * \li posnk kernel (obviously)
 * \li ncurses
 * \li klange's nyancat ( command: nyan [note: set TERM to cygwin for this to work] )
 * \li elvis editor ( command: elvis -f ram [issues with sessionfiles make the -f argument required] )
 * \li bash 4.2
 * \li busybox
 * \li custom getty, init, login, mount utilities
 * \li ext2 support (use mount /dev/hda1 <mountpoint> ext2 to mount)
 * \li signal test program
 * \li multiuser support (ctrl+F[n] switches to vt[n]) 
 */

//-----------------------------------------------------------
/** \page large Large CD image
 * <a href="https://www.sendspace.com/file/x5w9tb"> Download here (150MB)</a>
 * \section cont Contents: 
 * \li posnk kernel (obviously)
 * \li ncurses
 * \li klange's nyancat ( command: nyan [note: set TERM to cygwin for this to work] )
 * \li elvis editor ( command: elvis -f ram [issues with sessionfiles make the -f argument required] )
 * \li bash 4.2
 * \li busybox
 * \li custom getty, init, login, mount utilities
 * \li ext2 support (use mount /dev/hda1 <mountpoint> ext2 to mount)
 * \li signal test program
 * \li multiuser support (ctrl+F[n] switches to vt[n]) 
 * \li gcc
 * \li binutils
 * \li klanges nyancat sources and build script (run bash ./make.sh)
 * \li a copy of George Orwell's 1984 (/root/1984.txt)
 *
 */

//-----------------------------------------------------------
/** \page features Features
 *
 * \section now Current features:
 *
 * \li Partially UNIX-compatible file API
 * \li Initial ramdisk loaded from tar files passed as modules
 * \li Multitasking
 * \li Signals
 * \li Partial old signal API
 * \li Ext2 filesystem (read-only)
 * \li Partial UNIX timekeeping API implementation
 * \li Memory mapped files
 * \li Multiple virtual consoles
 * \li UNIX TTY api
 * \li User/group based permissions
 * \li Post-mortem debugger hook
 *
 * \section future Planned features:
 * \li mmap API
 * \li In-kernel gdb server to ease debugging multitasking
 * \li Ext2 write support
 * \li Shared memory
 * \li VGA Framebuffer driver  
 */
 */
