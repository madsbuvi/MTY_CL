/***********************************************************************
 *
 *	file: sboxdef.cl
 *
 *	Selection boxes of the kernel side of the OpenCL implementation of mty cl
 *
 *  These Boolean expressions corresponding to DES S-boxes have been generated
 *  by Roman Rusakov <roman_rus at openwall.com> for use in Openwall's
 *  John the Ripper password cracker: http://www.openwall.com/john/
 *  Being mathematical formulas, they are not copyrighted and are free for reuse
 *  by anyone.
 *
 *  This file is a modification of one sboxes-s.c, Copyright (c) 2011 by Solar
 *  Designer <solar at openwall.com>.
 *  Modified by Mads Buvik Sandvei (https://github.com/madsbuvi/, madsbuvi@stud.ntnu.no)
 */
#define S1(out1,out2,out3,out4)\
{\
 type x55005500, x5A0F5A0F, x3333FFFF, x66666666, x22226666, x2D2D6969,\
     x25202160;\
 type x00FFFF00, x33CCCC33, x4803120C, x2222FFFF, x6A21EDF3, x4A01CC93;\
 type x5555FFFF, x7F75FFFF, x00D20096, x7FA7FF69;\
 type x0A0A0000, x0AD80096, x00999900, x0AD99996;\
 type x22332233, x257AA5F0, x054885C0, xFAB77A3F, x2221EDF3, xD89697CC;\
 type x05B77AC0, x05F77AD6, x36C48529, x6391D07C, xBB0747B0;\
 type x4C460000, x4EDF9996, x2D4E49EA, xBBFFFFB0, x96B1B65A;\
 type x5AFF5AFF, x52B11215, x4201C010, x10B0D205;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
\
 x55005500 = a1 & ~a5;\
 x5A0F5A0F = a4 ^ x55005500;\
 x3333FFFF = a3 | a6;\
 x66666666 = a1 ^ a3;\
 x22226666 = x3333FFFF & x66666666;\
 x2D2D6969 = a4 ^ x22226666;\
 x25202160 = x2D2D6969 & ~x5A0F5A0F;\
\
 x00FFFF00 = a5 ^ a6;\
 x33CCCC33 = a3 ^ x00FFFF00;\
 x4803120C = x5A0F5A0F & ~x33CCCC33;\
 x2222FFFF = a6 | x22226666;\
 x6A21EDF3 = x4803120C ^ x2222FFFF;\
 x4A01CC93 = x6A21EDF3 & ~x25202160;\
\
 x5555FFFF = a1 | a6;\
 x7F75FFFF = x6A21EDF3 | x5555FFFF;\
 x00D20096 = a5 & ~x2D2D6969;\
 x7FA7FF69 = x7F75FFFF ^ x00D20096;\
\
 x0A0A0000 = a4 & ~x5555FFFF;\
 x0AD80096 = x00D20096 ^ x0A0A0000;\
 x00999900 = x00FFFF00 & ~x66666666;\
 x0AD99996 = x0AD80096 | x00999900;\
\
 x22332233 = a3 & ~x55005500;\
 x257AA5F0 = x5A0F5A0F ^ x7F75FFFF;\
 x054885C0 = x257AA5F0 & ~x22332233;\
 xFAB77A3F = ~x054885C0;\
 x2221EDF3 = x3333FFFF & x6A21EDF3;\
 xD89697CC = xFAB77A3F ^ x2221EDF3;\
 x20 = x7FA7FF69 & ~a2;\
 x21 = x20 ^ xD89697CC;\
 out3 = out3 ^ x21;\
\
 x05B77AC0 = x00FFFF00 ^ x054885C0;\
 x05F77AD6 = x00D20096 | x05B77AC0;\
 x36C48529 = x3333FFFF ^ x05F77AD6;\
 x6391D07C = a1 ^ x36C48529;\
 xBB0747B0 = xD89697CC ^ x6391D07C;\
 x00 = x25202160 | a2;\
 x01 = x00 ^ xBB0747B0;\
 out1 = out1 ^ x01;\
\
 x4C460000 = x3333FFFF ^ x7F75FFFF;\
 x4EDF9996 = x0AD99996 | x4C460000;\
 x2D4E49EA = x6391D07C ^ x4EDF9996;\
 xBBFFFFB0 = x00FFFF00 | xBB0747B0;\
 x96B1B65A = x2D4E49EA ^ xBBFFFFB0;\
 x10 = x4A01CC93 | a2;\
 x11 = x10 ^ x96B1B65A;\
 out2 = out2 ^ x11;\
\
 x5AFF5AFF = a5 | x5A0F5A0F;\
 x52B11215 = x5AFF5AFF & ~x2D4E49EA;\
 x4201C010 = x4A01CC93 & x6391D07C;\
 x10B0D205 = x52B11215 ^ x4201C010;\
 x30 = x10B0D205 | a2;\
 x31 = x30 ^ x0AD99996;\
 out4 = out4 ^ x31;\
}

#define S2(out1,out2,out3,out4)\
{\
 type x33CC33CC;\
 type x55550000, x00AA00FF, x33BB33FF;\
 type x33CC0000, x11441144, x11BB11BB, x003311BB;\
 type x00000F0F, x336600FF, x332200FF, x332200F0;\
 type x0302000F, xAAAAAAAA, xA9A8AAA5, x33CCCC33, x33CCC030, x9A646A95;\
 type x00333303, x118822B8, xA8208805, x3CC3C33C, x94E34B39;\
 type x0331330C, x3FF3F33C, xA9DF596A, xA9DF5F6F, x962CAC53;\
 type xA9466A6A, x3DA52153, x29850143, x33C0330C, x1A45324F;\
 type x0A451047, xBBDFDD7B, xB19ACD3C;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
\
 x33CC33CC = a2 ^ a5;\
\
 x55550000 = a1 & ~a6;\
 x00AA00FF = a5 & ~x55550000;\
 x33BB33FF = a2 | x00AA00FF;\
\
 x33CC0000 = x33CC33CC & ~a6;\
 x11441144 = a1 & x33CC33CC;\
 x11BB11BB = a5 ^ x11441144;\
 x003311BB = x11BB11BB & ~x33CC0000;\
\
 x00000F0F = a3 & a6;\
 x336600FF = x00AA00FF ^ x33CC0000;\
 x332200FF = x33BB33FF & x336600FF;\
 x332200F0 = x332200FF & ~x00000F0F;\
\
 x0302000F = a3 & x332200FF;\
 xAAAAAAAA = ~a1;\
 xA9A8AAA5 = x0302000F ^ xAAAAAAAA;\
 x33CCCC33 = a6 ^ x33CC33CC;\
 x33CCC030 = x33CCCC33 & ~x00000F0F;\
 x9A646A95 = xA9A8AAA5 ^ x33CCC030;\
 x10 = a4 & ~x332200F0;\
 x11 = x10 ^ x9A646A95;\
 out2 = out2 ^ x11;\
\
 x00333303 = a2 & ~x33CCC030;\
 x118822B8 = x11BB11BB ^ x00333303;\
 xA8208805 = xA9A8AAA5 & ~x118822B8;\
 x3CC3C33C = a3 ^ x33CCCC33;\
 x94E34B39 = xA8208805 ^ x3CC3C33C;\
 x00 = x33BB33FF & ~a4;\
 x01 = x00 ^ x94E34B39;\
 out1 = out1 ^ x01;\
\
 x0331330C = x0302000F ^ x00333303;\
 x3FF3F33C = x3CC3C33C | x0331330C;\
 xA9DF596A = x33BB33FF ^ x9A646A95;\
 xA9DF5F6F = x00000F0F | xA9DF596A;\
 x962CAC53 = x3FF3F33C ^ xA9DF5F6F;\
\
 xA9466A6A = x332200FF ^ x9A646A95;\
 x3DA52153 = x94E34B39 ^ xA9466A6A;\
 x29850143 = xA9DF5F6F & x3DA52153;\
 x33C0330C = x33CC33CC & x3FF3F33C;\
 x1A45324F = x29850143 ^ x33C0330C;\
 x20 = x1A45324F | a4;\
 x21 = x20 ^ x962CAC53;\
 out3 = out3 ^ x21;\
\
 x0A451047 = x1A45324F & ~x118822B8;\
 xBBDFDD7B = x33CCCC33 | xA9DF596A;\
 xB19ACD3C = x0A451047 ^ xBBDFDD7B;\
 x30 = x003311BB | a4;\
 x31 = x30 ^ xB19ACD3C;\
 out4 = out4 ^ x31;\
}

#define S3(out1,out2,out3,out4)\
{\
 type x44444444, x0F0FF0F0, x4F4FF4F4, x00FFFF00, x00AAAA00, x4FE55EF4;\
 type x3C3CC3C3, x3C3C0000, x7373F4F4, x0C840A00;\
 type x00005EF4, x00FF5EFF, x00555455, x3C699796;\
 type x000FF000, x55AA55AA, x26D9A15E, x2FDFAF5F, x2FD00F5F;\
 type x55AAFFAA, x28410014, x000000FF, x000000CC, x284100D8;\
 type x204100D0, x3C3CC3FF, x1C3CC32F, x4969967A;\
 type x3F3F3F3F, xB01AA10B, xBF3FBF3F, x83037CFC, xD6A98356;\
 type x001A000B, x3C73979D, xBF73FFFD, x0D2DD23E, xB25E2DC3;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
 x44444444 = a1 & ~a2;\
 x0F0FF0F0 = a3 ^ a6;\
 x4F4FF4F4 = x44444444 | x0F0FF0F0;\
 x00FFFF00 = a4 ^ a6;\
 x00AAAA00 = x00FFFF00 & ~a1;\
 x4FE55EF4 = x4F4FF4F4 ^ x00AAAA00;\
 x3C3CC3C3 = a2 ^ x0F0FF0F0;\
 x3C3C0000 = x3C3CC3C3 & ~a6;\
 x7373F4F4 = x4F4FF4F4 ^ x3C3C0000;\
 x0C840A00 = x4FE55EF4 & ~x7373F4F4;\
 x00005EF4 = a6 & x4FE55EF4;\
 x00FF5EFF = a4 | x00005EF4;\
 x00555455 = a1 & x00FF5EFF;\
 x3C699796 = x3C3CC3C3 ^ x00555455;\
 x30 = x4FE55EF4 & ~a5;\
 x31 = x30 ^ x3C699796;\
 out4 = out4 ^ x31;\
 x000FF000 = x0F0FF0F0 & x00FFFF00;\
 x55AA55AA = a1 ^ a4;\
 x26D9A15E = x7373F4F4 ^ x55AA55AA;\
 x2FDFAF5F = a3 | x26D9A15E;\
 x2FD00F5F = x2FDFAF5F & ~x000FF000;\
 x55AAFFAA = x00AAAA00 | x55AA55AA;\
 x28410014 = x3C699796 & ~x55AAFFAA;\
 x000000FF = a4 & a6;\
 x000000CC = x000000FF & ~a2;\
 x284100D8 = x28410014 ^ x000000CC;\
 x204100D0 = x7373F4F4 & x284100D8;\
 x3C3CC3FF = x3C3CC3C3 | x000000FF;\
 x1C3CC32F = x3C3CC3FF & ~x204100D0;\
 x4969967A = a1 ^ x1C3CC32F;\
 x10 = x2FD00F5F & a5;\
 x11 = x10 ^ x4969967A;\
 out2 = out2 ^ x11;\
 x3F3F3F3F = a2 | a3;\
 xB01AA10B = ~x4FE55EF4;\
 xBF3FBF3F = x3F3F3F3F | xB01AA10B;\
 x83037CFC = x3C3CC3C3 ^ xBF3FBF3F;\
 xD6A98356 = x55AAFFAA ^ x83037CFC;\
 x00 = a5 & ~x0C840A00;\
 x01 = x00 ^ xD6A98356;\
 out1 = out1 ^ x01;\
 x001A000B = a4 & xB01AA10B;\
 x3C73979D = x3C699796 ^ x001A000B;\
 xBF73FFFD = x83037CFC | x3C73979D;\
 x0D2DD23E = x44444444 ^ x4969967A;\
 xB25E2DC3 = xBF73FFFD ^ x0D2DD23E;\
 x20 = x284100D8 | a5;\
 x21 = x20 ^ xB25E2DC3;\
 out3 = out3 ^ x21;\
}

#define S4(out1,out2,out3,out4)\
{\
 type x5A5A5A5A, x0F0FF0F0;\
 type x33FF33FF, x33FFCC00, x0C0030F0, x0C0CC0C0, x0CF3C03F, x5EFBDA7F,\
     x52FBCA0F, x61C8F93C;\
 type x00C0C03C, x0F0F30C0, x3B92A366, x30908326, x3C90B3D6;\
 type x33CC33CC, x0C0CFFFF, x379E5C99, x04124C11, x56E9861E, xA91679E1;\
 type x9586CA37, x8402C833, x84C2C83F, xB35C94A6;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
 x5A5A5A5A = a1 ^ a3;\
 x0F0FF0F0 = a3 ^ a5;\
 x33FF33FF = a2 | a4;\
 x33FFCC00 = a5 ^ x33FF33FF;\
 x0C0030F0 = x0F0FF0F0 & ~x33FFCC00;\
 x0C0CC0C0 = x0F0FF0F0 & ~a2;\
 x0CF3C03F = a4 ^ x0C0CC0C0;\
 x5EFBDA7F = x5A5A5A5A | x0CF3C03F;\
 x52FBCA0F = x5EFBDA7F & ~x0C0030F0;\
 x61C8F93C = a2 ^ x52FBCA0F;\
 x00C0C03C = x0CF3C03F & x61C8F93C;\
 x0F0F30C0 = x0F0FF0F0 & ~x00C0C03C;\
 x3B92A366 = x5A5A5A5A ^ x61C8F93C;\
 x30908326 = x3B92A366 & ~x0F0F30C0;\
 x3C90B3D6 = x0C0030F0 ^ x30908326;\
 x33CC33CC = a2 ^ a4;\
 x0C0CFFFF = a5 | x0C0CC0C0;\
 x379E5C99 = x3B92A366 ^ x0C0CFFFF;\
 x04124C11 = x379E5C99 & ~x33CC33CC;\
 x56E9861E = x52FBCA0F ^ x04124C11;\
 x00 = a6 & ~x3C90B3D6;\
 x01 = x00 ^ x56E9861E;\
 out1 = out1 ^ x01;\
 xA91679E1 = ~x56E9861E;\
 x10 = x3C90B3D6 & ~a6;\
 x11 = x10 ^ xA91679E1;\
 out2 = out2 ^ x11;\
 x9586CA37 = x3C90B3D6 ^ xA91679E1;\
 x8402C833 = x9586CA37 & ~x33CC33CC;\
 x84C2C83F = x00C0C03C | x8402C833;\
 xB35C94A6 = x379E5C99 ^ x84C2C83F;\
 x20 = x61C8F93C | a6;\
 x21 = x20 ^ xB35C94A6;\
 out3 = out3 ^ x21;\
 x30 = a6 & x61C8F93C;\
 x31 = x30 ^ xB35C94A6;\
 out4 = out4 ^ x31;\
}
#define S5(out1,out2,out3,out4)\
{\
 type x77777777, x77770000, x22225555, x11116666, x1F1F6F6F;\
 type x70700000, x43433333, x00430033, x55557777, x55167744, x5A19784B;\
 type x5A1987B4, x7A3BD7F5, x003B00F5, x221955A0, x05050707, x271C52A7;\
 type x2A2A82A0, x6969B193, x1FE06F90, x16804E00, xE97FB1FF;\
 type x43403302, x35CAED30, x37DEFFB7, x349ECCB5, x0B01234A;\
 type x101884B4, x0FF8EB24, x41413333, x4FF9FB37, x4FC2FBC2;\
 type x43E9BBC2, x16BCEE97, x0F080B04, x19B4E593;\
 type x5C5C5C5C, x4448184C, x2DDABE71, x6992A63D;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
 x77777777 = a1 | a3;\
 x77770000 = x77777777 & ~a6;\
 x22225555 = a1 ^ x77770000;\
 x11116666 = a3 ^ x22225555;\
 x1F1F6F6F = a4 | x11116666;\
 x70700000 = x77770000 & ~a4;\
 x43433333 = a3 ^ x70700000;\
 x00430033 = a5 & x43433333;\
 x55557777 = a1 | x11116666;\
 x55167744 = x00430033 ^ x55557777;\
 x5A19784B = a4 ^ x55167744;\
 x5A1987B4 = a6 ^ x5A19784B;\
 x7A3BD7F5 = x22225555 | x5A1987B4;\
 x003B00F5 = a5 & x7A3BD7F5;\
 x221955A0 = x22225555 ^ x003B00F5;\
 x05050707 = a4 & x55557777;\
 x271C52A7 = x221955A0 ^ x05050707;\
 x2A2A82A0 = x7A3BD7F5 & ~a1;\
 x6969B193 = x43433333 ^ x2A2A82A0;\
 x1FE06F90 = a5 ^ x1F1F6F6F;\
 x16804E00 = x1FE06F90 & ~x6969B193;\
 xE97FB1FF = ~x16804E00;\
 x20 = xE97FB1FF & ~a2;\
 x21 = x20 ^ x5A19784B;\
 out3 = out3 ^ x21;\
 x43403302 = x43433333 & ~x003B00F5;\
 x35CAED30 = x2A2A82A0 ^ x1FE06F90;\
 x37DEFFB7 = x271C52A7 | x35CAED30;\
 x349ECCB5 = x37DEFFB7 & ~x43403302;\
 x0B01234A = x1F1F6F6F & ~x349ECCB5;\
 x101884B4 = x5A1987B4 & x349ECCB5;\
 x0FF8EB24 = x1FE06F90 ^ x101884B4;\
 x41413333 = x43433333 & x55557777;\
 x4FF9FB37 = x0FF8EB24 | x41413333;\
 x4FC2FBC2 = x003B00F5 ^ x4FF9FB37;\
 x30 = x4FC2FBC2 & a2;\
 x31 = x30 ^ x271C52A7;\
 out4 = out4 ^ x31;\
 x43E9BBC2 = x77777777 ^ x349ECCB5;\
 x16BCEE97 = a1 ^ x43E9BBC2;\
 x0F080B04 = a4 & x0FF8EB24;\
 x19B4E593 = x16BCEE97 ^ x0F080B04;\
 x00 = x0B01234A | a2;\
 x01 = x00 ^ x19B4E593;\
 out1 = out1 ^ x01;\
 x5C5C5C5C = x1F1F6F6F ^ x43433333;\
 x4448184C = x5C5C5C5C & ~x19B4E593;\
 x2DDABE71 = x22225555 ^ x0FF8EB24;\
 x6992A63D = x4448184C ^ x2DDABE71;\
 x10 = x1F1F6F6F & a2;\
 x11 = x10 ^ x6992A63D;\
 out2 = out2 ^ x11;\
}
#define S6(out1,out2,out3,out4)\
{\
 type x33CC33CC;\
 type x3333FFFF, x11115555, x22DD6699, x22DD9966, x00220099;\
 type x00551144, x33662277, x5A5A5A5A, x7B7E7A7F, x59A31CE6;\
 type x09030C06, x09030000, x336622FF, x3A6522FF;\
 type x484D494C, x0000B6B3, x0F0FB9BC, x00FC00F9, x0FFFB9FD;\
 type x7B7B7B7B, x411E5984, x1FFFFDFD, x5EE1A479;\
 type x5DF75DF7, x116600F7, x1E69B94B, x1668B94B;\
 type x3CB4DFD2, x004B002D, x33363237, xCCC9CDC8, xCC82CDE5;\
 type x0055EEBB, x5A5AECE9, x0050ECA9, xC5CAC1CE, xC59A2D67;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
 x33CC33CC = a2 ^ a5;\
 x3333FFFF = a2 | a6;\
 x11115555 = a1 & x3333FFFF;\
 x22DD6699 = x33CC33CC ^ x11115555;\
 x22DD9966 = a6 ^ x22DD6699;\
 x00220099 = a5 & ~x22DD9966;\
 x00551144 = a1 & x22DD9966;\
 x33662277 = a2 ^ x00551144;\
 x5A5A5A5A = a1 ^ a3;\
 x7B7E7A7F = x33662277 | x5A5A5A5A;\
 x59A31CE6 = x22DD6699 ^ x7B7E7A7F;\
 x09030C06 = a3 & x59A31CE6;\
 x09030000 = x09030C06 & ~a6;\
 x336622FF = x00220099 | x33662277;\
 x3A6522FF = x09030000 ^ x336622FF;\
 x30 = x3A6522FF & a4;\
 x31 = x30 ^ x59A31CE6;\
 out4 = out4 ^ x31;\
 x484D494C = a2 ^ x7B7E7A7F;\
 x0000B6B3 = a6 & ~x484D494C;\
 x0F0FB9BC = a3 ^ x0000B6B3;\
 x00FC00F9 = a5 & ~x09030C06;\
 x0FFFB9FD = x0F0FB9BC | x00FC00F9;\
 x7B7B7B7B = a2 | x5A5A5A5A;\
 x411E5984 = x3A6522FF ^ x7B7B7B7B;\
 x1FFFFDFD = x11115555 | x0FFFB9FD;\
 x5EE1A479 = x411E5984 ^ x1FFFFDFD;\
 x5DF75DF7 = a1 | x59A31CE6;\
 x116600F7 = x336622FF & x5DF75DF7;\
 x1E69B94B = x0F0FB9BC ^ x116600F7;\
 x1668B94B = x1E69B94B & ~x09030000;\
 x20 = x00220099 | a4;\
 x21 = x20 ^ x1668B94B;\
 out3 = out3 ^ x21;\
 x3CB4DFD2 = x22DD6699 ^ x1E69B94B;\
 x004B002D = a5 & ~x3CB4DFD2;\
 x33363237 = x484D494C ^ x7B7B7B7B;\
 xCCC9CDC8 = ~x33363237;\
 xCC82CDE5 = x004B002D ^ xCCC9CDC8;\
 x10 = xCC82CDE5 & ~a4;\
 x11 = x10 ^ x5EE1A479;\
 out2 = out2 ^ x11;\
 x0055EEBB = a6 ^ x00551144;\
 x5A5AECE9 = a1 ^ x0F0FB9BC;\
 x0050ECA9 = x0055EEBB & x5A5AECE9;\
 xC5CAC1CE = x09030C06 ^ xCCC9CDC8;\
 xC59A2D67 = x0050ECA9 ^ xC5CAC1CE;\
 x00 = x0FFFB9FD & ~a4;\
 x01 = x00 ^ xC59A2D67;\
 out1 = out1 ^ x01;\
}

#define S7(out1,out2,out3,out4)\
{\
 type x0FF00FF0, x3CC33CC3, x00003CC3, x0F000F00, x5A555A55, x00001841;\
 type x00000F00, x33333C33, x7B777E77, x0FF0F00F, x74878E78;\
 type x003C003C, x5A7D5A7D, x333300F0, x694E5A8D;\
 type x0FF0CCCC, x000F0303, x5A505854, x33CC000F, x699C585B;\
 type x0FF0C0CC, x0FFFC3CF, x2E222B22, x28000802, x27FFCBCD;\
 type x48444844, x4FF4C8CC, x6F9C5F5B, x4F944848, x686B8385;\
 type x6FFFDBCF, x6FC3DBF3, x0000DBF3, x4F9493BB;\
 type x00005151, x96B1A572, x96B1F423, xD9256798;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
 x0FF00FF0 = a4 ^ a5;\
 x3CC33CC3 = a3 ^ x0FF00FF0;\
 x00003CC3 = a6 & x3CC33CC3;\
 x0F000F00 = a4 & x0FF00FF0;\
 x5A555A55 = a2 ^ x0F000F00;\
 x00001841 = x00003CC3 & x5A555A55;\
 x00000F00 = a6 & x0F000F00;\
 x33333C33 = a3 ^ x00000F00;\
 x7B777E77 = x5A555A55 | x33333C33;\
 x0FF0F00F = a6 ^ x0FF00FF0;\
 x74878E78 = x7B777E77 ^ x0FF0F00F;\
 x30 = a1 & ~x00001841;\
 x31 = x30 ^ x74878E78;\
 out4 = out4 ^ x31;\
 x003C003C = a5 & ~x3CC33CC3;\
 x5A7D5A7D = x5A555A55 | x003C003C;\
 x333300F0 = x00003CC3 ^ x33333C33;\
 x694E5A8D = x5A7D5A7D ^ x333300F0;\
 x0FF0CCCC = x00003CC3 ^ x0FF0F00F;\
 x000F0303 = a4 & ~x0FF0CCCC;\
 x5A505854 = x5A555A55 & ~x000F0303;\
 x33CC000F = a5 ^ x333300F0;\
 x699C585B = x5A505854 ^ x33CC000F;\
 x0FF0C0CC = x0FF0CCCC & ~x00000F00;\
 x0FFFC3CF = x000F0303 | x0FF0C0CC;\
 x2E222B22 = a2 ^ x7B777E77;\
 x28000802 = x699C585B & x2E222B22;\
 x27FFCBCD = x0FFFC3CF ^ x28000802;\
 x20 = x27FFCBCD & a1;\
 x21 = x20 ^ x699C585B;\
 out3 = out3 ^ x21;\
 x48444844 = x5A555A55 & ~a3;\
 x4FF4C8CC = x0FF0C0CC | x48444844;\
 x6F9C5F5B = x0F000F00 | x699C585B;\
 x4F944848 = x4FF4C8CC & x6F9C5F5B;\
 x686B8385 = x27FFCBCD ^ x4F944848;\
 x6FFFDBCF = x694E5A8D | x0FFFC3CF;\
 x6FC3DBF3 = x003C003C ^ x6FFFDBCF;\
 x0000DBF3 = a6 & x6FC3DBF3;\
 x4F9493BB = x4F944848 ^ x0000DBF3;\
 x00 = x4F9493BB & ~a1;\
 x01 = x00 ^ x694E5A8D;\
 out1 = out1 ^ x01;\
 x00005151 = a2 & x0000DBF3;\
 x96B1A572 = ~x694E5A8D;\
 x96B1F423 = x00005151 ^ x96B1A572;\
 xD9256798 = x4F9493BB ^ x96B1F423;\
 x10 = x686B8385 | a1;\
 x11 = x10 ^ xD9256798;\
 out2 = out2 ^ x11;\
}

#define S8(out1,out2,out3,out4)\
{\
 type x0C0C0C0C, x0000F0F0, x00FFF00F, x00555005, x00515001;\
 type x33000330, x77555775, x30303030, x3030CFCF, x30104745, x30555745;\
 type xFF000FF0, xCF1048B5, x080A080A, xC71A40BF, xCB164CB3;\
 type xF429738C, xC72970BC, x9E4319E6, x000019E6, xC729695A;\
 type xF77C3E1F, xF77F3F3F, x9E43E619, x693CD926;\
 type xF719A695, xF4FF73FF, x03E6D56A, x56B3803F;\
 type xF700A600, x61008000, x03B7856B, x62B7056B;\
 type x00, x01, x10, x11, x20, x21, x30, x31;\
 x0C0C0C0C = a3 & ~a2;\
 x0000F0F0 = a5 & ~a3;\
 x00FFF00F = a4 ^ x0000F0F0;\
 x00555005 = a1 & x00FFF00F;\
 x00515001 = x00555005 & ~x0C0C0C0C;\
 x33000330 = a2 & ~x00FFF00F;\
 x77555775 = a1 | x33000330;\
 x30303030 = a2 & ~a3;\
 x3030CFCF = a5 ^ x30303030;\
 x30104745 = x77555775 & x3030CFCF;\
 x30555745 = x00555005 | x30104745;\
 xFF000FF0 = ~x00FFF00F;\
 xCF1048B5 = x30104745 ^ xFF000FF0;\
 x080A080A = a3 & ~x77555775;\
 xC71A40BF = xCF1048B5 ^ x080A080A;\
 xCB164CB3 = x0C0C0C0C ^ xC71A40BF;\
 x10 = x00515001 | a6;\
 x11 = x10 ^ xCB164CB3;\
 out2 = out2 ^ x11;\
 xF429738C = a2 ^ xC71A40BF;\
 xC72970BC = x33000330 ^ xF429738C;\
 x9E4319E6 = a1 ^ xCB164CB3;\
 x000019E6 = a5 & x9E4319E6;\
 xC729695A = xC72970BC ^ x000019E6;\
 xF77C3E1F = x30555745 ^ xC729695A;\
 xF77F3F3F = a2 | xF77C3E1F;\
 x9E43E619 = a5 ^ x9E4319E6;\
 x693CD926 = xF77F3F3F ^ x9E43E619;\
 x20 = x30555745 & a6;\
 x21 = x20 ^ x693CD926;\
 out3 = out3 ^ x21;\
 xF719A695 = x3030CFCF ^ xC729695A;\
 xF4FF73FF = a4 | xF429738C;\
 x03E6D56A = xF719A695 ^ xF4FF73FF;\
 x56B3803F = a1 ^ x03E6D56A;\
 x30 = x56B3803F & a6;\
 x31 = x30 ^ xC729695A;\
 out4 = out4 ^ x31;\
 xF700A600 = xF719A695 & ~a4;\
 x61008000 = x693CD926 & xF700A600;\
 x03B7856B = x00515001 ^ x03E6D56A;\
 x62B7056B = x61008000 ^ x03B7856B;\
 x00 = x62B7056B | a6;\
 x01 = x00 ^ xC729695A;\
 out1 = out1 ^ x01;\
}
