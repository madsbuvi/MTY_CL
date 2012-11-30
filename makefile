CC = gcc
CFLAGS = -O3 -fomit-frame-pointer -Wall -mms-bitfields -I"C:\Program Files (x86)\AMD APP\include" -L"C:\Program Files (x86)\AMD APP\lib\x86"
DEFS = -DDEBUG=0
BISON = bison
BFLAGS = --debug
FLEX = flex
LFLAGS = -lpthread -lOpenCL
LGTK = -LD:/usr/dev/lib -lgtk-win32-2.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lgtksourceview-2.0
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
TARG = mty_cl.exe
OBJS = mty.o crypt64.o cp932.o desconst.o dt4.o scoreboard.o synth.o translate.o expr_parse.o tr64_mmx.o wdict.o simplify.o key.o log.o hit.o mty_cl.o cl_util.o crypt3.o des_std.o key_cl.o
PARSE = expr_parse
SCAN = expr_scan

all	: $(TARG) $(GUI_TARG)

core : $(TARG)

gui : $(GUI_TARG)

.c.o	:
	$(CC) $(CFLAGS) $(GTKFLAGS) $(DEFS) -c $< 

.S.o	:
	$(CC) $(CFLAGS) $(DEFS) -c $<

$(PARSE).c	: $(PARSE).y $(SCAN).c
	$(BISON) $(BFLAGS) -o$(PARSE).c $(PARSE).y

$(SCAN).c	: $(SCAN).lex
	$(FLEX) -o$(SCAN).c $(SCAN).lex

$(GUI_TARG)	: $(GUI_OBJS)
	$(CC) $(CFLAGS) $(GTKFLAGS) $(DEFS) -o $(GUI_TARG) $(GUI_OBJS) $(LGTK)
	strip $(GUI_TARG)
	
$(TARG)	: $(OBJS)
	$(CC) $(CFLAGS) $(DEFS) -o $(TARG) $(OBJS) $(LFLAGS)
	strip $(TARG)
	editbin /STACK:33554432 $(TARG)

clean	:
	rm -f $(OBJS) $(TARG) $(GUI_OBJS) $(GUI_TARG) *.bak $(SCAN).c $(PARSE).c
