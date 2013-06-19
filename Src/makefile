CC = gcc
CFLAGS = -O3 -fomit-frame-pointer -Wall -mms-bitfields -Wno-unused-function
DEFS = -DDEBUG=0
BFLAGS = --debug
CLFLAGS = -I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v5.0\include" -L"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v5.0\lib\Win32"
LFLAGS = -lpthread -lOpenCL
LGTK = -LD:/usr/dev/lib -lgtk-win32-2.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lgtksourceview-2.0 -lintl
GTKFLAGS =  -I"D:\usr\dev\include\gtksourceview-2.0" -ID:/usr/dev/include/gtk-2.0 -ID:/usr/dev/lib/gtk-2.0/include -ID:/usr/dev/include/atk-1.0 -ID:/usr/dev/include/cairo -ID:/usr/dev/include/gdk-pixbuf-2.0 -ID:/usr/dev/include/pango-1.0 -ID:/usr/dev/include/glib-2.0 -ID:/usr/dev/lib/glib-2.0/include -ID:/usr/dev/include

# SSE2 と MMX の選択 (どちらか片方だけを有効にする)
DEFS += -msse2
#DEFS += -DUSE_MMX

# FreeBSD 6.1R だとこれが必要
#DEFS += -DUSE_REGPARM

# W32
DEFS += -finput-charset=cp932 -fexec-charset=cp932

GUI_TARG = mty_gui.exe
GUI_OBJS = mty_gui.o util.o
TARG_AMD = mty_cl_amd.exe
TARG_NVI = mty_cl_nvidia.exe
OBJS = mty.o crypt64.o cp932.o desconst.o dt4.o scoreboard.o synth.o translate.o expr_parse.o tr64_mmx.o wdict.o simplify.o key.o log.o hit.o mty_cl.o cl_util.o crypt3.o des_std.o key_cl.o

all	: $(GUI_TARG)
	make core

core : 
	make amd
	make nvidia

amd: 
	rm -f mty_cl.o
	make amd_t CFLAGS="$(CFLAGS) -DNVIDIA=0"

nvidia: 
	rm -f mty_cl.o
	make nvidia_t CFLAGS="$(CFLAGS) -DNVIDIA=1"
	
amd_t: $(TARG_AMD)

nvidia_t: $(TARG_NVI)

gui : $(GUI_TARG)

%.c: %.y
%.c: %.l

.c.o	:
	$(CC) $(CFLAGS) $(CLFLAGS) $(GTKFLAGS) $(DEFS) -c $< 

.S.o	:
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -c $<

$(GUI_TARG)	: $(GUI_OBJS)
	$(CC) $(CFLAGS) $(GTKFLAGS) $(DEFS) -o $(GUI_TARG) $(GUI_OBJS) $(LGTK)
	strip $(GUI_TARG)
	
$(TARG_NVI): $(OBJS)
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -o $(TARG_NVI) $(OBJS) $(LFLAGS)
	strip $(TARG_NVI)
	
$(TARG_AMD): $(OBJS)
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -o $(TARG_AMD) $(OBJS) $(LFLAGS)
	strip $(TARG_AMD)

clean	:
	rm -f $(OBJS) $(TARG) $(GUI_OBJS) $(GUI_TARG) *.bak $(SCAN).c $(PARSE).c
