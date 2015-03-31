       #include <stdint.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <time.h>
       #include <stdio.h>
       #include <stdlib.h>

       int
       main(int argc, char *argv[])
       {
           struct stat sb;
	   char *linkname;
           ssize_t r;

           if (argc != 2) {
               fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           if (lstat(argv[1], &sb) == -1) {
               perror("stat");
               exit(EXIT_FAILURE);
           }

           printf("File type:                ");

           switch (sb.st_mode & S_IFMT) {
           case S_IFBLK:  printf("block device\n");            break;
           case S_IFCHR:  printf("character device\n");        break;
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFIFO:  printf("FIFO/pipe\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           case S_IFSOCK: printf("socket\n");                  break;
           default:       printf("unknown?\n");                break;
           }
           printf("I-node number:            %ld\n", (long) sb.st_ino);

           printf("Mode:                     %lo (octal)\n",
                   (unsigned long) sb.st_mode);

           printf("Link count:               %ld\n", (long) sb.st_nlink);
           printf("Ownership:                UID=%ld   GID=%ld\n",
                   (long) sb.st_uid, (long) sb.st_gid);

           printf("Preferred I/O block size: %ld bytes\n",
                   (long) sb.st_blksize);
           printf("File size:                %lld bytes\n",
                   (long long) sb.st_size);
           printf("Blocks allocated:         %lld\n",
                   (long long) sb.st_blocks);

           printf("Last status change:       %s", ctime(&sb.st_ctime));
           printf("Last file access:         %s", ctime(&sb.st_atime));
           printf("Last file modification:   %s", ctime(&sb.st_mtime));
 
	   linkname = malloc(sb.st_size + 1);
           if (linkname == NULL) {
               fprintf(stderr, "insufficient memory\n");
               exit(EXIT_FAILURE);
           }

           r = readlink(argv[1], linkname, sb.st_size + 1);

           if (r < 0) {
               perror("lstat");
               exit(EXIT_FAILURE);
           }

           if (r > sb.st_size) {
               fprintf(stderr, "symlink increased in size "
                               "between lstat() and readlink()\n");
               exit(EXIT_FAILURE);
           }

           linkname[sb.st_size] = '\0';

           printf("'%s' points to '%s'\n", argv[1], linkname);

           exit(EXIT_SUCCESS);
       }

