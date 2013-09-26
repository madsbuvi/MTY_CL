CC = gcc
CFLAGS = -fomit-frame-pointer -Wall -mms-bitfields -Wno-unused-function -DUSE_64_XMM
DEFS = -DDEBUG=0
BFLAGS = --debug
LFLAGS = -lpthread -lOpenCL


##WINDOWS
##Edit paths to match your own system. Running pgk-config in the gtk directory might help.
#CLFLAGS = -I"C:\Program Files (x86)\AMD APP\include" -L"C:\Program Files (x86)\AMD APP\lib\x86"
#LGTK = -LD:/usr/dev/lib -lgtk-win32-2.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lgtksourceview-2.0 -lintl
#GTKFLAGS =  -I"D:\usr\dev\include\gtksourceview-2.0" -ID:/usr/dev/include/gtk-2.0 -ID:/usr/dev/lib/gtk-2.0/include -ID:/usr/dev/include/atk-1.0 -ID:/usr/dev/include/cairo -ID:/usr/dev/include/gdk-pixbuf-2.0 -ID:/usr/dev/include/pango-1.0 -ID:/usr/dev/include/glib-2.0 -ID:/usr/dev/lib/glib-2.0/include -ID:/usr/dev/include
#GUI_TARG = mty_gui.exe
#TARG_AMD = mty_cl_amd.exe
#TARG_NVI = mty_cl_nvidia.exe
#OBJS = tr64_mmx.o

##LINUX
#Unfortunately it doesn't seem like nvidia/amd wants to give me a way to include opencl without using absolute paths...
CLFLAGS = -I/usr/local/cuda-5.5/include/
#export LD_LIBRARY_PATH=/usr/local/cuda-5.5/lib64:$LD_LIBRARY_PATH
GTKFLAGS =  `pkg-config --cflags gtk+-3.0` `pkg-config --cflags glib-2.0` `pkg-config --cflags gtksourceview-3.0` 
LGTK = `pkg-config --libs gtk+-3.0` `pkg-config --libs glib-2.0` `pkg-config --libs gtksourceview-3.0`
GUI_TARG = mty_gui
TARG_AMD = mty_cl_amd
TARG_NVI = mty_cl_nvidia
OBJS = tr64_amd64.o


DEFS += -msse2
DEFS += -finput-charset=cp932 -fexec-charset=cp932

GUI_OBJS = mty_gui.o util.o
OBJS += mty.o crypt64.o cp932.o desconst.o dt4.o scoreboard.o synth.o translate.o expr_parse.o wdict.o simplify.o key.o log.o hit.o mty_cl.o cl_util.o crypt3.o DES_std.o key_cl.o


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
	#strip $(GUI_TARG)
	
$(TARG_NVI): $(OBJS)
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -o $(TARG_NVI) $(OBJS) $(LFLAGS)
	#strip $(TARG_NVI)
	
$(TARG_AMD): $(OBJS)
	$(CC) $(CFLAGS) $(CLFLAGS) $(DEFS) -o $(TARG_AMD) $(OBJS) $(LFLAGS)
	#strip $(TARG_AMD)

clean	:
	rm -f $(OBJS) $(TARG) $(GUI_OBJS) $(GUI_TARG) *.bak $(SCAN).c $(PARSE).c
