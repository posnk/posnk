INCLUDE_DIRECTORIES(include include/crt)
add_compile_definitions(ARCH_I386)
add_executable(fake
        fs/proc/proc.c
        fs/proc/dir.c
        fs/proc/files.c
        fs/proc/process.c
        fs/proc/snapfile.c
        fs/proc/task.c
        fs/proc/mem.c
        fs/proc/sig.c
        kernel/permissions.c
        kernel/exception.c
        kernel/pipe.c
        kernel/sc_perm.c
        kernel/sc_streams.c
        kernel/sc_time.c
        kernel/sc_vfs.c
        kernel/shutdown.c
        kernel/streams.c
        kernel/syscall.c
        kernel/tar.c
        kernel/time.c
        kernel/tty.c
        kernel/version.c
        kernel/vfs.c
        kernel/mm/physmm.c 
kernel/mm/paging.c 
kernel/earlycon.c 
kernel/exception.c 
kernel/dev/blkcache.c 
kernel/dev/interrupt.c 
kernel/dev/drivermgr.c 
kernel/dev/blkdev.c 
kernel/dev/chardev.c 
kernel/task/scheduler.c 
kernel/task/synch.c 
kernel/proc/process.c 
kernel/proc/signal.c 
kernel/proc/procvmm.c 
kernel/proc/exec.c 
kernel/proc/sc_process.c 
kernel/proc/sc_signals.c 
kernel/proc/signal_api.c 
kernel/vfs/icache.c 
kernel/vfs/dcache.c 
kernel/vfs/ifswrap.c 
kernel/vfs/pathres.c 
kernel/vfs/perm.c 
kernel/vfs/ifsmgr.c 
kernel/vfs/mount.c 
kernel/vfs.c 
        kernel/binfmt/elfloader.c 
kernel/svipc/ipc.c 
kernel/svipc/shm.c 
kernel/svipc/sem.c 
kernel/svipc/msg.c 
kernel/svipc/sc_ipc.c 
kernel/console/srcmap.c 
kernel/console/conmgr.c 
kernel/console/conput.c 
kernel/console/consink.c 
kernel/console/cmdline.c 
kernel/init/cmdline.c 
kernel/init/kinit.c 
kernel/init/uinit.c 
kernel/init/idle.c
        kernel/mm/dlmalloc.c
kernel/mm/dlheapmm.c driver/tty/uart/ns16x50.c include/driver/tty/uart/ns16x50.h driver/tty/uart/pc_uart.c)