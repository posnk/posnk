#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

LIBNAME = libwtk

# define the C compiler to use
CC = @echo " [  CC  ]	" $< ; i386-pc-posnk-gcc
AR = @echo " [  AR  ]	" $@ ; i386-pc-posnk-ar
RL = @echo " [RANLIB]	" $@ ; i386-pc-posnk-ranlib

NASM = @echo " [ NASM ]	" $< ; nasm

# define any compile-time flags
CFLAGS = -Wall -g `pkg-config --cflags --libs cairo` -Iinclude

NASMFLAGS = -w+orphan-labels -felf -g 

INSTALL_DIR = ../../gcc_posnk/i386-pc-posnk

# define any directories containing header files other than /usr/include
#
INCLUDES = 

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
ARFLAGS = rcs

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS =  -lbmp -lclara `pkg-config --cflags --libs cairo` -lpixman-1 -lm -lfontconfig -lfreetype -lexpat -lpng -lz

# define the C source files
SRCS = window.c \
	widget.c \
	button.c \
	resources.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)
all:	$(LIBNAME).a

$(LIBNAME).a: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBNAME).a $(OBJS)
	$(RL) $(LIBNAME).a

install: $(LIBNAME).a
	@cp $(LIBNAME).a $(INSTALL_DIR)/lib/$(LIBNAME).a
	@cp -R include/* $(INSTALL_DIR)/include/

.PHONY: depend clean

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

.s.o:
	$(NASM) $(NASMFLAGS) $< -o $@ 

clean:
	$(RM) *.o *~ $(LIBNAME).a

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

