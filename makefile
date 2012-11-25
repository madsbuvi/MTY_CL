CC = gcc
CFLAGS = -O3 -fomit-frame-pointer -Wall -I"C:\Program Files (x86)\AMD APP\include" -L"C:\Program Files (x86)\AMD APP\lib\x86"
DEFS = -DDEBUG=0
BISON = bison
BFLAGS = --debug
FLEX = flex
LFLAGS = -lpthread -lOpenCL

# SSE2 と MMX の選択 (どちらか片方だけを有効にする)
DEFS += -msse2
#DEFS += -DUSE_MMX

# FreeBSD 6.1R だとこれが必要
#DEFS += -DUSE_REGPARM

# W32
DEFS += -finput-charset=cp932 -fexec-charset=cp932

TARG = mty_cl.exe
OBJS = mty.o crypt64.o cp932.o desconst.o dt4.o scoreboard.o synth.o translate.o expr_parse.o tr64_mmx.o wdict.o simplify.o key.o log.o hit.o mty_cl.o cl_util.o crypt3.o des_std.o
PARSE = expr_parse
SCAN = expr_scan

all	: $(TARG)

.c.o	:
	$(CC) $(CFLAGS) $(DEFS) -c $< 

.S.o	:
	$(CC) $(CFLAGS) $(DEFS) -c $<

$(PARSE).c	: $(PARSE).y $(SCAN).c
	$(BISON) $(BFLAGS) -o$(PARSE).c $(PARSE).y

$(SCAN).c	: $(SCAN).lex
	$(FLEX) -o$(SCAN).c $(SCAN).lex

$(TARG)	: $(OBJS)
	$(CC) $(CFLAGS) $(DEFS) -o $(TARG) $(OBJS) $(LFLAGS)
	strip $(TARG)

clean	:
	rm -f $(OBJS) $(TARG) *.bak $(SCAN).c $(PARSE).c
