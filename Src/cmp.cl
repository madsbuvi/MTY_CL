//LR assembly
#define COMPARISON \
uint32_t work0, work1, work2, work3, work4, work5, work6, work7, work8, work9, work10, work11, work12, work13, work14, work15, work16, work17, work18, work19, work20, work21, work22, work23, work24, work25, work26, work27, work28, work29, work30, work31, work32, work33, work34, work35, work36, work37, work38, work39, work40, work41, work42, work43, work44, work45, work46, work47, work48, work49, work50, work51, work52, work53, work54, work55, work56, work57, work58, work59, work60, work61, work62, work63, work64, work65, work66, work67, work68, work69, work70, work71, work72, work73, work74, work75, work76, work77, work78, work79, work80, work81, work82, work83, work84, work85, work86, work87, work88, work89, work90, work91, work92, work93, work94, work95, work96, work97, work98, work99, work100, work101, work102, work103, work104, work105, work106, work107, work108, work109, work110, work111, work112, work113, work114, work115, work116, work117, work118, work119, work120, work121, work122, work123, work124, work125, work126, work127; \
x6 = b14; \
x7 = b46; \
x8 = b6; \
x9 = b38; \
x10 = b31; \
x11 = b63; \
x13 = x8; \
x13 = (x13)|(x11); \
x13 = (~x13)&(x6); \
x13 = (x13)&(x7); \
x13 = (x13)&(x9); \
x13 = (x13)&(x10); \
work23 = x13; \
x13 = x6; \
x13 = (x13)|(x7); \
x13 = (x13)|(x10); \
x13 = (x13)|(x11); \
x13 = (~x13)&(x8); \
x13 = (x13)&(x9); \
x12 = x6; \
x12 = (x12)|(x9); \
x12 = (x12)|(x10); \
x12 = (~x12)&(x7); \
x12 = (x12)&(x8); \
x12 = (x12)&(x11); \
x13 = (x13)|(x12); \
work24 = x13; \
x13 = x7; \
x13 = (x13)|(x10); \
x13 = (x13)|(x11); \
x13 = (~x13)&(x6); \
x13 = (x13)&(x8); \
x13 = (x13)&(x9); \
x12 = x11; \
x12 = (~x12)&(x6); \
x12 = (x12)&(x7); \
x12 = (x12)&(x8); \
x12 = (x12)&(x9); \
x12 = (x12)&(x10); \
x13 = (x13)|(x12); \
x12 = x9; \
x12 = (x12)|(x10); \
x12 = (~x12)&(x6); \
x12 = (x12)&(x7); \
x12 = (x12)&(x8); \
x12 = (x12)&(x11); \
x13 = (x13)|(x12); \
work25 = x13; \
x12 = b5; \
x6 = (x6)^(x12); \
x13 = b37; \
x7 = (x7)^(x13); \
x7 = (x7)|(x6); \
x14 = b30; \
x8 = (x8)^(x14); \
x8 = (x8)|(x7); \
x15 = b62; \
x9 = (x9)^(x15); \
x9 = (x9)|(x8); \
x0 = b22; \
x10 = (x10)^(x0); \
x10 = (x10)|(x9); \
x1 = b54; \
x11 = (x11)^(x1); \
x11 = (x11)|(x10); \
work14 = x11; \
x3 = x14; \
x3 = (x3)|(x1); \
x3 = (~x3)&(x12); \
x3 = (x3)&(x13); \
x3 = (x3)&(x15); \
x3 = (x3)&(x0); \
work26 = x3; \
x3 = x12; \
x3 = (x3)|(x13); \
x3 = (x3)|(x15); \
x3 = (~x3)&(x14); \
x3 = (x3)&(x0); \
x3 = (x3)&(x1); \
work27 = x3; \
x3 = x13; \
x3 = (x3)|(x15); \
x3 = (~x3)&(x12); \
x3 = (x3)&(x14); \
x3 = (x3)&(x0); \
x3 = (x3)&(x1); \
work28 = x3; \
x3 = work27; \
x2 = x12; \
x2 = (x2)|(x15); \
x2 = (x2)|(x0); \
x2 = (~x2)&(x13); \
x2 = (x2)&(x14); \
x2 = (x2)&(x1); \
x3 = (x3)|(x2); \
work29 = x3; \
x3 = x12; \
x3 = (x3)|(x13); \
x3 = (x3)|(x0); \
x3 = (x3)|(x1); \
x3 = (~x3)&(x14); \
x3 = (x3)&(x15); \
x2 = x12; \
x2 = (x2)|(x15); \
x2 = (x2)|(x0); \
x2 = (~x2)&(x13); \
x2 = (x2)&(x14); \
x2 = (x2)&(x1); \
x3 = (x3)|(x2); \
work30 = x3; \
x3 = x13; \
x3 = (x3)|(x0); \
x3 = (x3)|(x1); \
x3 = (~x3)&(x12); \
x3 = (x3)&(x14); \
x3 = (x3)&(x15); \
x2 = x1; \
x2 = (~x2)&(x12); \
x2 = (x2)&(x13); \
x2 = (x2)&(x14); \
x2 = (x2)&(x15); \
x2 = (x2)&(x0); \
x3 = (x3)|(x2); \
x2 = x15; \
x2 = (x2)|(x0); \
x2 = (~x2)&(x12); \
x2 = (x2)&(x13); \
x2 = (x2)&(x14); \
x2 = (x2)&(x1); \
x3 = (x3)|(x2); \
work31 = x3; \
x2 = b29; \
x12 = (x12)^(x2); \
x3 = b61; \
x13 = (x13)^(x3); \
x13 = (x13)|(x12); \
x4 = b21; \
x14 = (x14)^(x4); \
x14 = (x14)|(x13); \
x5 = b53; \
x15 = (x15)^(x5); \
x15 = (x15)|(x14); \
x6 = b13; \
x0 = (x0)^(x6); \
x0 = (x0)|(x15); \
x7 = b45; \
x1 = (x1)^(x7); \
x1 = (x1)|(x0); \
work15 = x1; \
x9 = x4; \
x9 = (x9)|(x7); \
x9 = (~x9)&(x2); \
x9 = (x9)&(x3); \
x9 = (x9)&(x5); \
x9 = (x9)&(x6); \
work32 = x9; \
x9 = x4; \
x9 = (x9)|(x5); \
x9 = (~x9)&(x2); \
x9 = (x9)&(x3); \
x9 = (x9)&(x6); \
x9 = (x9)&(x7); \
work33 = x9; \
x9 = x2; \
x9 = (x9)|(x3); \
x9 = (x9)|(x5); \
x9 = (~x9)&(x4); \
x9 = (x9)&(x6); \
x9 = (x9)&(x7); \
work34 = x9; \
x9 = x3; \
x9 = (x9)|(x5); \
x9 = (~x9)&(x2); \
x9 = (x9)&(x4); \
x9 = (x9)&(x6); \
x9 = (x9)&(x7); \
work35 = x9; \
x9 = work33; \
x8 = x2; \
x8 = (x8)|(x5); \
x8 = (x8)|(x6); \
x8 = (~x8)&(x3); \
x8 = (x8)&(x4); \
x8 = (x8)&(x7); \
x9 = (x9)|(x8); \
work36 = x9; \
x9 = work34; \
x8 = x2; \
x8 = (x8)|(x5); \
x8 = (x8)|(x6); \
x8 = (~x8)&(x3); \
x8 = (x8)&(x4); \
x8 = (x8)&(x7); \
x9 = (x9)|(x8); \
work37 = x9; \
x9 = x3; \
x9 = (x9)|(x6); \
x9 = (x9)|(x7); \
x9 = (~x9)&(x2); \
x9 = (x9)&(x4); \
x9 = (x9)&(x5); \
x8 = x7; \
x8 = (~x8)&(x2); \
x8 = (x8)&(x3); \
x8 = (x8)&(x4); \
x8 = (x8)&(x5); \
x8 = (x8)&(x6); \
x9 = (x9)|(x8); \
x8 = x5; \
x8 = (x8)|(x6); \
x8 = (~x8)&(x2); \
x8 = (x8)&(x3); \
x8 = (x8)&(x4); \
x8 = (x8)&(x7); \
x9 = (x9)|(x8); \
work38 = x9; \
x8 = b20; \
x2 = (x2)^(x8); \
x9 = b52; \
x3 = (x3)^(x9); \
x3 = (x3)|(x2); \
x10 = b12; \
x4 = (x4)^(x10); \
x4 = (x4)|(x3); \
x11 = b44; \
x5 = (x5)^(x11); \
x5 = (x5)|(x4); \
x12 = b4; \
x6 = (x6)^(x12); \
x6 = (x6)|(x5); \
x13 = b36; \
x7 = (x7)^(x13); \
x7 = (x7)|(x6); \
work16 = x7; \
x15 = x10; \
x15 = (x15)|(x13); \
x15 = (~x15)&(x8); \
x15 = (x15)&(x9); \
x15 = (x15)&(x11); \
x15 = (x15)&(x12); \
work39 = x15; \
x15 = x8; \
x15 = (x15)|(x12); \
x15 = (~x15)&(x9); \
x15 = (x15)&(x10); \
x15 = (x15)&(x11); \
x15 = (x15)&(x13); \
work40 = x15; \
x15 = x9; \
x15 = (x15)|(x10); \
x15 = (x15)|(x11); \
x15 = (~x15)&(x8); \
x15 = (x15)&(x12); \
x15 = (x15)&(x13); \
work41 = x15; \
x15 = x10; \
x15 = (x15)|(x11); \
x15 = (~x15)&(x8); \
x15 = (x15)&(x9); \
x15 = (x15)&(x12); \
x15 = (x15)&(x13); \
work42 = x15; \
x15 = x8; \
x15 = (x15)|(x9); \
x15 = (x15)|(x11); \
x15 = (~x15)&(x10); \
x15 = (x15)&(x12); \
x15 = (x15)&(x13); \
work43 = x15; \
x15 = x9; \
x15 = (x15)|(x11); \
x15 = (~x15)&(x8); \
x15 = (x15)&(x10); \
x15 = (x15)&(x12); \
x15 = (x15)&(x13); \
work44 = x15; \
x15 = work42; \
x14 = x8; \
x14 = (x14)|(x11); \
x14 = (x14)|(x12); \
x14 = (~x14)&(x9); \
x14 = (x14)&(x10); \
x14 = (x14)&(x13); \
x15 = (x15)|(x14); \
work45 = x15; \
x15 = work43; \
x14 = x8; \
x14 = (x14)|(x11); \
x14 = (x14)|(x12); \
x14 = (~x14)&(x9); \
x14 = (x14)&(x10); \
x14 = (x14)&(x13); \
x15 = (x15)|(x14); \
work46 = x15; \
x15 = x8; \
x15 = (x15)|(x11); \
x15 = (x15)|(x12); \
x15 = (~x15)&(x9); \
x15 = (x15)&(x10); \
x15 = (x15)&(x13); \
x14 = x8; \
x14 = (x14)|(x9); \
x14 = (x14)|(x10); \
x14 = (~x14)&(x11); \
x14 = (x14)&(x12); \
x14 = (x14)&(x13); \
x15 = (x15)|(x14); \
work47 = x15; \
x15 = x9; \
x15 = (x15)|(x12); \
x15 = (x15)|(x13); \
x15 = (~x15)&(x8); \
x15 = (x15)&(x10); \
x15 = (x15)&(x11); \
x14 = x13; \
x14 = (~x14)&(x8); \
x14 = (x14)&(x9); \
x14 = (x14)&(x10); \
x14 = (x14)&(x11); \
x14 = (x14)&(x12); \
x15 = (x15)|(x14); \
x14 = x11; \
x14 = (x14)|(x12); \
x14 = (~x14)&(x8); \
x14 = (x14)&(x9); \
x14 = (x14)&(x10); \
x14 = (x14)&(x13); \
x15 = (x15)|(x14); \
work48 = x15; \
x14 = b11; \
x8 = (x8)^(x14); \
x15 = b43; \
x9 = (x9)^(x15); \
x9 = (x9)|(x8); \
x0 = b3; \
x10 = (x10)^(x0); \
x10 = (x10)|(x9); \
x1 = b35; \
x11 = (x11)^(x1); \
x11 = (x11)|(x10); \
x2 = b28; \
x12 = (x12)^(x2); \
x12 = (x12)|(x11); \
x3 = b60; \
x13 = (x13)^(x3); \
x13 = (x13)|(x12); \
work17 = x13; \
x5 = x0; \
x5 = (x5)|(x3); \
x5 = (~x5)&(x14); \
x5 = (x5)&(x15); \
x5 = (x5)&(x1); \
x5 = (x5)&(x2); \
work49 = x5; \
x5 = x3; \
x5 = (~x5)&(x14); \
x5 = (x5)&(x15); \
x5 = (x5)&(x0); \
x5 = (x5)&(x1); \
x5 = (x5)&(x2); \
work50 = x5; \
x5 = x14; \
x5 = (x5)|(x0); \
x5 = (x5)|(x2); \
x5 = (~x5)&(x15); \
x5 = (x5)&(x1); \
x5 = (x5)&(x3); \
work51 = x5; \
x5 = x14; \
x5 = (x5)|(x2); \
x5 = (~x5)&(x15); \
x5 = (x5)&(x0); \
x5 = (x5)&(x1); \
x5 = (x5)&(x3); \
work52 = x5; \
x5 = x15; \
x5 = (x5)|(x0); \
x5 = (x5)|(x1); \
x5 = (~x5)&(x14); \
x5 = (x5)&(x2); \
x5 = (x5)&(x3); \
work53 = x5; \
x5 = x0; \
x5 = (x5)|(x1); \
x5 = (~x5)&(x14); \
x5 = (x5)&(x15); \
x5 = (x5)&(x2); \
x5 = (x5)&(x3); \
work54 = x5; \
x5 = x14; \
x5 = (x5)|(x15); \
x5 = (x5)|(x1); \
x5 = (~x5)&(x0); \
x5 = (x5)&(x2); \
x5 = (x5)&(x3); \
work55 = x5; \
x5 = work54; \
x4 = x14; \
x4 = (x4)|(x1); \
x4 = (x4)|(x2); \
x4 = (~x4)&(x15); \
x4 = (x4)&(x0); \
x4 = (x4)&(x3); \
x5 = (x5)|(x4); \
work56 = x5; \
x5 = work55; \
x4 = x14; \
x4 = (x4)|(x1); \
x4 = (x4)|(x2); \
x4 = (~x4)&(x15); \
x4 = (x4)&(x0); \
x4 = (x4)&(x3); \
x5 = (x5)|(x4); \
work57 = x5; \
x5 = work54; \
x4 = x15; \
x4 = (x4)|(x0); \
x4 = (~x4)&(x14); \
x4 = (x4)&(x1); \
x4 = (x4)&(x2); \
x4 = (x4)&(x3); \
x5 = (x5)|(x4); \
work58 = x5; \
x5 = x14; \
x5 = (x5)|(x1); \
x5 = (x5)|(x2); \
x5 = (~x5)&(x15); \
x5 = (x5)&(x0); \
x5 = (x5)&(x3); \
x4 = x14; \
x4 = (x4)|(x15); \
x4 = (x4)|(x0); \
x4 = (~x4)&(x1); \
x4 = (x4)&(x2); \
x4 = (x4)&(x3); \
x5 = (x5)|(x4); \
work59 = x5; \
x4 = b2; \
x14 = (x14)^(x4); \
x5 = b34; \
x15 = (x15)^(x5); \
x15 = (x15)|(x14); \
x6 = b27; \
x0 = (x0)^(x6); \
x0 = (x0)|(x15); \
x7 = b59; \
x1 = (x1)^(x7); \
x1 = (x1)|(x0); \
x8 = b19; \
x2 = (x2)^(x8); \
x2 = (x2)|(x1); \
x9 = b51; \
x3 = (x3)^(x9); \
x3 = (x3)|(x2); \
work18 = x3; \
x11 = x4; \
x11 = (x11)|(x6); \
x11 = (x11)|(x8); \
x11 = (~x11)&(x5); \
x11 = (x11)&(x7); \
x11 = (x11)&(x9); \
work60 = x11; \
x11 = x4; \
x11 = (x11)|(x8); \
x11 = (~x11)&(x5); \
x11 = (x11)&(x6); \
x11 = (x11)&(x7); \
x11 = (x11)&(x9); \
work61 = x11; \
x11 = x6; \
x11 = (x11)|(x7); \
x11 = (~x11)&(x4); \
x11 = (x11)&(x5); \
x11 = (x11)&(x8); \
x11 = (x11)&(x9); \
work62 = x11; \
x11 = x4; \
x11 = (x11)|(x5); \
x11 = (x11)|(x7); \
x11 = (~x11)&(x6); \
x11 = (x11)&(x8); \
x11 = (x11)&(x9); \
work63 = x11; \
x11 = work62; \
x10 = x4; \
x10 = (x10)|(x7); \
x10 = (x10)|(x8); \
x10 = (~x10)&(x5); \
x10 = (x10)&(x6); \
x10 = (x10)&(x9); \
x11 = (x11)|(x10); \
work64 = x11; \
x11 = work62; \
x10 = x5; \
x10 = (x10)|(x6); \
x10 = (~x10)&(x4); \
x10 = (x10)&(x7); \
x10 = (x10)&(x8); \
x10 = (x10)&(x9); \
x11 = (x11)|(x10); \
work65 = x11; \
x11 = x6; \
x11 = (x11)|(x9); \
x11 = (~x11)&(x4); \
x11 = (x11)&(x5); \
x11 = (x11)&(x7); \
x11 = (x11)&(x8); \
x10 = x5; \
x10 = (x10)|(x7); \
x10 = (~x10)&(x4); \
x10 = (x10)&(x6); \
x10 = (x10)&(x8); \
x10 = (x10)&(x9); \
x11 = (x11)|(x10); \
work66 = x11; \
x11 = x4; \
x11 = (x11)|(x7); \
x11 = (x11)|(x8); \
x11 = (~x11)&(x5); \
x11 = (x11)&(x6); \
x11 = (x11)&(x9); \
x10 = x4; \
x10 = (x10)|(x5); \
x10 = (x10)|(x6); \
x10 = (~x10)&(x7); \
x10 = (x10)&(x8); \
x10 = (x10)&(x9); \
x11 = (x11)|(x10); \
work67 = x11; \
x10 = b26; \
x4 = (x4)^(x10); \
x11 = b58; \
x5 = (x5)^(x11); \
x5 = (x5)|(x4); \
x12 = b18; \
x6 = (x6)^(x12); \
x6 = (x6)|(x5); \
x13 = b50; \
x7 = (x7)^(x13); \
x7 = (x7)|(x6); \
x14 = b10; \
x8 = (x8)^(x14); \
x8 = (x8)|(x7); \
x15 = b42; \
x9 = (x9)^(x15); \
x9 = (x9)|(x8); \
work19 = x9; \
x1 = x10; \
x1 = (x1)|(x13); \
x1 = (x1)|(x14); \
x1 = (~x1)&(x11); \
x1 = (x1)&(x12); \
x1 = (x1)&(x15); \
work68 = x1; \
x1 = x10; \
x1 = (x1)|(x12); \
x1 = (x1)|(x14); \
x1 = (~x1)&(x11); \
x1 = (x1)&(x13); \
x1 = (x1)&(x15); \
work69 = x1; \
x1 = x10; \
x1 = (x1)|(x14); \
x1 = (~x1)&(x11); \
x1 = (x1)&(x12); \
x1 = (x1)&(x13); \
x1 = (x1)&(x15); \
work70 = x1; \
x1 = x12; \
x1 = (x1)|(x13); \
x1 = (~x1)&(x10); \
x1 = (x1)&(x11); \
x1 = (x1)&(x14); \
x1 = (x1)&(x15); \
work71 = x1; \
x1 = work68; \
x0 = x10; \
x0 = (x0)|(x11); \
x0 = (x0)|(x12); \
x0 = (~x0)&(x13); \
x0 = (x0)&(x14); \
x0 = (x0)&(x15); \
x1 = (x1)|(x0); \
work72 = x1; \
x1 = work71; \
x0 = x11; \
x0 = (x0)|(x12); \
x0 = (~x0)&(x10); \
x0 = (x0)&(x13); \
x0 = (x0)&(x14); \
x0 = (x0)&(x15); \
x1 = (x1)|(x0); \
work73 = x1; \
x1 = x12; \
x1 = (x1)|(x15); \
x1 = (~x1)&(x10); \
x1 = (x1)&(x11); \
x1 = (x1)&(x13); \
x1 = (x1)&(x14); \
x0 = x11; \
x0 = (x0)|(x13); \
x0 = (~x0)&(x10); \
x0 = (x0)&(x12); \
x0 = (x0)&(x14); \
x0 = (x0)&(x15); \
x1 = (x1)|(x0); \
work74 = x1; \
x0 = b17; \
x10 = (x10)^(x0); \
x1 = b49; \
x11 = (x11)^(x1); \
x11 = (x11)|(x10); \
x2 = b9; \
x12 = (x12)^(x2); \
x12 = (x12)|(x11); \
x3 = b41; \
x13 = (x13)^(x3); \
x13 = (x13)|(x12); \
x4 = b1; \
x14 = (x14)^(x4); \
x14 = (x14)|(x13); \
x5 = b33; \
x15 = (x15)^(x5); \
x15 = (x15)|(x14); \
work20 = x15; \
x7 = x0; \
x7 = (x7)|(x2); \
x7 = (x7)|(x4); \
x7 = (~x7)&(x1); \
x7 = (x7)&(x3); \
x7 = (x7)&(x5); \
work75 = x7; \
x7 = x0; \
x7 = (x7)|(x4); \
x7 = (~x7)&(x1); \
x7 = (x7)&(x2); \
x7 = (x7)&(x3); \
x7 = (x7)&(x5); \
work76 = x7; \
x7 = x0; \
x7 = (x7)|(x1); \
x7 = (x7)|(x2); \
x7 = (~x7)&(x3); \
x7 = (x7)&(x4); \
x7 = (x7)&(x5); \
work77 = x7; \
x6 = x0; \
x6 = (x6)|(x3); \
x6 = (x6)|(x4); \
x6 = (~x6)&(x1); \
x6 = (x6)&(x2); \
x6 = (x6)&(x5); \
x7 = (x7)|(x6); \
work78 = x7; \
x7 = x2; \
x7 = (x7)|(x3); \
x7 = (~x7)&(x0); \
x7 = (x7)&(x1); \
x7 = (x7)&(x4); \
x7 = (x7)&(x5); \
x6 = x1; \
x6 = (x6)|(x2); \
x6 = (~x6)&(x0); \
x6 = (x6)&(x3); \
x6 = (x6)&(x4); \
x6 = (x6)&(x5); \
x7 = (x7)|(x6); \
work79 = x7; \
x6 = b8; \
x0 = (x0)^(x6); \
x7 = b40; \
x1 = (x1)^(x7); \
x1 = (x1)|(x0); \
x8 = b0; \
x2 = (x2)^(x8); \
x2 = (x2)|(x1); \
x9 = b32; \
x3 = (x3)^(x9); \
x3 = (x3)|(x2); \
x10 = b25; \
x4 = (x4)^(x10); \
x4 = (x4)|(x3); \
x11 = b57; \
x5 = (x5)^(x11); \
x5 = (x5)|(x4); \
work21 = x5; \
x13 = x6; \
x13 = (x13)|(x8); \
x13 = (x13)|(x10); \
x13 = (~x13)&(x7); \
x13 = (x13)&(x9); \
x13 = (x13)&(x11); \
work80 = x13; \
x13 = x7; \
x13 = (x13)|(x8); \
x13 = (~x13)&(x6); \
x13 = (x13)&(x9); \
x13 = (x13)&(x10); \
x13 = (x13)&(x11); \
work81 = x13; \
x13 = x6; \
x13 = (x13)|(x9); \
x13 = (x13)|(x10); \
x13 = (~x13)&(x7); \
x13 = (x13)&(x8); \
x13 = (x13)&(x11); \
x12 = x6; \
x12 = (x12)|(x7); \
x12 = (x12)|(x8); \
x12 = (~x12)&(x9); \
x12 = (x12)&(x10); \
x12 = (x12)&(x11); \
x13 = (x13)|(x12); \
work82 = x13; \
x7 = (x7)|(x6); \
x14 = b24; \
x8 = (x8)^(x14); \
x8 = (x8)|(x7); \
x15 = b56; \
x9 = (x9)^(x15); \
x9 = (x9)|(x8); \
x0 = b16; \
x10 = (x10)^(x0); \
x10 = (x10)|(x9); \
x1 = b48; \
x11 = (x11)^(x1); \
x11 = (x11)|(x10); \
work22 = x11; \
x3 = x14; \
x3 = (~x3)&(x15); \
x3 = (x3)&(x0); \
x3 = (x3)&(x1); \
work83 = x3; \

//Assembly
#define ASSEMBLY \
x0 = work14; \
x0 = x0|work15; \
x0 = x0|work16; \
x0 = x0|work17; \
x0 = x0|work18; \
x0 = x0|work19; \
x0 = x0|work20; \
work1 = x0; \
x1 = work15; \
x1 = x1|work16; \
x1 = x1|work17; \
x1 = x1|work18; \
x1 = x1|work19; \
x1 = x1|work20; \
x1 = x1|work21; \
work1 = x1; \
x0 = (x0)&(x1); \
x1 = work16; \
x1 = x1|work17; \
x1 = x1|work18; \
x1 = x1|work19; \
x1 = x1|work20; \
x1 = x1|work21; \
x1 = x1|work22; \
work1 = x1; \
x0 = (x0)&(x1); \
x0 = ~x0; \
work1 = x0; \
x1 = work25; \
x1 = x1&work29; \
x1 = x1&work36; \
x1 = x1&work47; \
x1 = x1&work58; \
x1 = x1&work67; \
work2 = x1; \
x0 = (x0)|(x1); \
x1 = work31; \
x1 = x1&work37; \
x1 = x1&work45; \
x1 = x1&work59; \
x1 = x1&work65; \
x1 = x1&work72; \
work3 = x1; \
x0 = (x0)|(x1); \
x1 = work38; \
x1 = x1&work46; \
x1 = x1&work56; \
x1 = x1&work67; \
x1 = x1&work73; \
x1 = x1&work78; \
work4 = x1; \
x0 = (x0)|(x1); \
x1 = work48; \
x1 = x1&work57; \
x1 = x1&work64; \
x1 = x1&work72; \
x1 = x1&work79; \
x1 = x1&work82; \
work5 = x1; \
x0 = (x0)|(x1); \
x1 = work24; \
x1 = x1&work28; \
x1 = x1&work35; \
x1 = x1&work41; \
x1 = x1&work51; \
x1 = x1&work66; \
x1 = x1&work70; \
x1 = x1&work75; \
work6 = x1; \
x0 = (x0)|(x1); \
x1 = work30; \
x1 = x1&work35; \
x1 = x1&work44; \
x1 = x1&work53; \
x1 = x1&work60; \
x1 = x1&work74; \
x1 = x1&work76; \
x1 = x1&work80; \
work7 = x1; \
x0 = (x0)|(x1); \
x1 = work50; \
x1 = x1&work63; \
x1 = x1&work68; \
x1 = x1&work77; \
x1 = x1&work81; \
x1 = (x1)&(x3); \
work8 = x1; \
x0 = (x0)|(x1); \
x1 = work23; \
x1 = x1&work27; \
x1 = x1&work33; \
x1 = x1&work40; \
x1 = x1&work51; \
work9 = x1; \
x0 = (x0)|(x1); \
x1 = work26; \
x1 = x1&work34; \
x1 = x1&work42; \
x1 = x1&work52; \
x1 = x1&work60; \
work10 = x1; \
x0 = (x0)|(x1); \
x1 = work32; \
x1 = x1&work43; \
x1 = x1&work54; \
x1 = x1&work61; \
x1 = x1&work69; \
work11 = x1; \
x0 = (x0)|(x1); \
x1 = work39; \
x1 = x1&work55; \
x1 = x1&work62; \
x1 = x1&work70; \
x1 = x1&work75; \
work12 = x1; \
x0 = (x0)|(x1); \
x1 = work49; \
x1 = x1&work63; \
x1 = x1&work71; \
x1 = x1&work76; \
x1 = x1&work80; \
work13 = x1; \
x0 = (x0)|(x1); \
work0 = x0; \
gwork_area( 0 ) = work0; \
