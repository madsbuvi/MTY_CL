/***********************************************************************
 *
 *	file: expr_scan.lex
 *
 *	Simple scanner
 *
 */

%option 8bit

%x EXPR_SET

CHAR	[./0-9A-Za-z]
NCHAR	[^./0-9A-Za-z]
CHAE	[.26AEIMQUYcgkosw]
CHU		[./A-Z]
CHUE	[.AEIMQUY]
CHL		[./a-z]
CHLE	[.cgkosw]
CHC		({CHU}|{CHL})
CHCE	({CHUE}|{CHLE})

ACY		((\.+\/+|\/+\.+)|(A+a+|a+A+)|(C+c+|c+C+)|(E+e+|e+E+)|(G+g+|g+G+)|(I+i+|i+I+)|(K+k+|k+K+)|(M+m+|m+M+)|(O+o+|o+O+)|(Q+q+|q+Q+)|(S+s+|s+S+)|(U+u+|u+U+)|(W+w+|w+W+)|(Y+y+|y+Y+))
ACZ		((B+b+|b+B+)|(D+d+|d+D+)|(F+f+|f+F+)|(H+h+|h+H+)|(J+j+|j+J+)|(L+l+|l+L+)|(N+n+|n+N+)|(P+p+|p+P+)|(R+r+|r+R+)|(T+t+|t+T+)|(V+v+|v+V+)|(X+x+|x+X+)|(Z+z+|z+Z+)|{ACY})
CHX		\[{ACZ}\]
CHXE	\[{ACY}\]

BLANK	[\x20\r\t]

PRINT	[\x20-\x7E\xA1-\xDF]
SJIS1	[\x80-\x9F\xE0-\xFC]
SJIS2	[\x40-\x7E\x80-\xFC]
%%

"#"[^\r\n]*[\r]*

({CHAR}|{CHX}|{CHXE}){11,}		expr_error("Ignored: <%s:%d>\nReason: Too long! Tripcodes are max 10 characters.", yytext, yyleng); return JUNK;

{CHU}{0,9}{CHUE}				return mkdict(CA_U, 1);
{CHU}{1,9}						return mkdict(CA_U, 0);

{CHC}{0,9}{CHCE}				return mkdict(CA_C, 1);
{CHC}{1,9}						return mkdict(CA_C, 0);

{CHAR}{0,9}{CHAE}				return mkdict(CA_A, 1);
{CHAR}{1,9}						return mkdict(CA_A, 0);

{CHAR}+							expr_error("Ignored: <%s:%d>\nReason: Too long! Tripcodes are max 10 characters.", yytext, yyleng); return JUNK;

{CHXE}							return mkchx(yytext[1], 1);
{CHX}							return mkchx(yytext[1], 0);

<EXPR_SET>"]"					BEGIN(0);
<EXPR_SET>{CHAR}-{CHAR}			yylval.bmp = mkrng(cv64(yytext[0]), cv64(yytext[2])); return RNGS;
<EXPR_SET>{CHAR}				yylval.bmp = mkrng(cv64(yytext[0]), (unsigned)-1); return RNGS;
<EXPR_SET>\n					BEGIN(0); expr_error("範囲指定の中で改行するなんて…"); g_lineno++;
<EXPR_SET>{SJIS1}{SJIS2}		BEGIN(0); expr_error("文字<%c%c(0x%02X%02X)>は範囲指定に含めるワケにはいかないので無視します。", yytext[0], yytext[1], yytext[0] & 0xFF, yytext[1] & 0xFF);
<EXPR_SET>{PRINT}				BEGIN(0); expr_error("文字<%c>は範囲指定に含められないので無視します。", yytext[0]);
<EXPR_SET>.						BEGIN(0); expr_error("なんですかこの文字は範囲指定なのに? <0x%02X> 無視っすよ?", yytext[0] & 0xFF);

[\^$?\(\)]						return yytext[0];
"[^"							BEGIN(EXPR_SET); return CLS_CMPL;
"["								BEGIN(EXPR_SET); return CLS;

({BLANK}|"|")*\n({BLANK}|"|")*	g_lineno++; return OR;
({BLANK}|"|")+					return OR;

{SJIS1}{SJIS2}					expr_error("文字<%c%c(0x%02X%02X)>はお行儀が悪いので無視します。", yytext[0], yytext[1], yytext[0] & 0xFF, yytext[1] & 0xFF); return JUNK;
{PRINT}							expr_error("文字<%c>はよくわかってないので無視します。", yytext[0]); return JUNK;
.								expr_error("なんですかこの文字は? <0x%02X> 無視っすよ?", yytext[0] & 0xFF); return JUNK;

%%

/*
 *	Local Variables:
 *		tab-width:	4
 *	End:
 *
 * EOF */
