#define DES \
a1 = k12 ^ e0; \
a2 = (kreg25) ^ e1; \
a3 = (kreg12) ^ e2; \
a4 = (kreg31) ^ e3; \
a5 = (kreg27) ^ e4; \
a6 = k20 ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = (kreg13) ^ e6; \
a2 = (kreg34) ^ e7; \
a3 = k5 ^ e8; \
a4 = k13 ^ e9; \
a5 = k18 ^ e10; \
a6 = (kreg19) ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = k4 ^ b7; \
a2 = (kreg11) ^ b8; \
a3 = (kreg5) ^ b9; \
a4 = (kreg6) ^ b10; \
a5 = (kreg17) ^ b11; \
a6 = (kreg33) ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = (kreg32) ^ b11; \
a2 = k6 ^ b12; \
a3 = (kreg10) ^ b13; \
a4 = (kreg4) ^ b14; \
a5 = k19 ^ b15; \
a6 = (kreg20) ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = k15 ^ e24; \
a2 = (kreg3) ^ e25; \
a3 = (kreg7) ^ e26; \
a4 = (kreg22) ^ e27; \
a5 = (kreg9) ^ e28; \
a6 = k3 ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = (kreg14) ^ e30; \
a2 = (kreg1) ^ e31; \
a3 = k2 ^ e32; \
a4 = (kreg23) ^ e33; \
a5 = k14 ^ e34; \
a6 = (kreg2) ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = (kreg30) ^ b23; \
a2 = k16 ^ b24; \
a3 = (kreg8) ^ b25; \
a4 = (kreg28) ^ b26; \
a5 = k7 ^ b27; \
a6 = k17 ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = (kreg16) ^ b27; \
a2 = k8 ^ b28; \
a3 = k9 ^ b29; \
a4 = (kreg29) ^ b30; \
a5 = (kreg21) ^ b31; \
a6 = (kreg0) ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = k5 ^ e48; \
a2 = (kreg18) ^ e49; \
a3 = (kreg5) ^ e50; \
a4 = (kreg24) ^ e51; \
a5 = (kreg20) ^ e52; \
a6 = k13 ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = (kreg6) ^ e54; \
a2 = (kreg27) ^ e55; \
a3 = (kreg32) ^ e56; \
a4 = k6 ^ e57; \
a5 = k11 ^ e58; \
a6 = (kreg12) ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = (kreg31) ^ b39; \
a2 = (kreg4) ^ b40; \
a3 = k19 ^ b41; \
a4 = k20 ^ b42; \
a5 = (kreg10) ^ b43; \
a6 = (kreg26) ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = (kreg25) ^ b43; \
a2 = (kreg33) ^ b44; \
a3 = (kreg34) ^ b45; \
a4 = k18 ^ b46; \
a5 = k12 ^ b47; \
a6 = (kreg13) ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = k8 ^ e72; \
a2 = k17 ^ e73; \
a3 = (kreg0) ^ e74; \
a4 = (kreg15) ^ e75; \
a5 = (kreg2) ^ e76; \
a6 = (kreg28) ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = (kreg7) ^ e78; \
a2 = k15 ^ e79; \
a3 = (kreg3) ^ e80; \
a4 = (kreg16) ^ e81; \
a5 = k7 ^ e82; \
a6 = k16 ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = (kreg23) ^ b55; \
a2 = k9 ^ b56; \
a3 = (kreg1) ^ b57; \
a4 = (kreg21) ^ b58; \
a5 = k0 ^ b59; \
a6 = k10 ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = (kreg9) ^ b59; \
a2 = k1 ^ b60; \
a3 = k2 ^ b61; \
a4 = (kreg22) ^ b62; \
a5 = (kreg14) ^ b63; \
a6 = k14 ^ b32; \
S8( b4 , b26 , b14 , b20 ); \
a1 = (kreg25) ^ e0; \
a2 = (kreg4) ^ e1; \
a3 = k12 ^ e2; \
a4 = (kreg10) ^ e3; \
a5 = (kreg6) ^ e4; \
a6 = (kreg33) ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = k13 ^ e6; \
a2 = (kreg13) ^ e7; \
a3 = (kreg18) ^ e8; \
a4 = (kreg26) ^ e9; \
a5 = (kreg31) ^ e10; \
a6 = k19 ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = (kreg17) ^ b7; \
a2 = k11 ^ b8; \
a3 = k5 ^ b9; \
a4 = k6 ^ b10; \
a5 = (kreg27) ^ b11; \
a6 = (kreg12) ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = (kreg11) ^ b11; \
a2 = (kreg19) ^ b12; \
a3 = (kreg20) ^ b13; \
a4 = k4 ^ b14; \
a5 = (kreg32) ^ b15; \
a6 = k20 ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = (kreg30) ^ e24; \
a2 = k3 ^ e25; \
a3 = k7 ^ e26; \
a4 = (kreg1) ^ e27; \
a5 = k9 ^ e28; \
a6 = (kreg14) ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = k14 ^ e30; \
a2 = k1 ^ e31; \
a3 = k10 ^ e32; \
a4 = (kreg2) ^ e33; \
a5 = (kreg29) ^ e34; \
a6 = k2 ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = (kreg9) ^ b23; \
a2 = (kreg3) ^ b24; \
a3 = k8 ^ b25; \
a4 = (kreg7) ^ b26; \
a5 = (kreg22) ^ b27; \
a6 = (kreg28) ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = k16 ^ b27; \
a2 = (kreg23) ^ b28; \
a3 = k17 ^ b29; \
a4 = (kreg8) ^ b30; \
a5 = (kreg0) ^ b31; \
a6 = k0 ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = (kreg11) ^ e48; \
a2 = k11 ^ e49; \
a3 = (kreg32) ^ e50; \
a4 = (kreg27) ^ e51; \
a5 = k13 ^ e52; \
a6 = (kreg19) ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = (kreg33) ^ e54; \
a2 = k20 ^ e55; \
a3 = (kreg4) ^ e56; \
a4 = (kreg12) ^ e57; \
a5 = (kreg17) ^ e58; \
a6 = k5 ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = (kreg34) ^ b39; \
a2 = (kreg31) ^ b40; \
a3 = (kreg25) ^ b41; \
a4 = (kreg26) ^ b42; \
a5 = (kreg13) ^ b43; \
a6 = k19 ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = k18 ^ b43; \
a2 = (kreg5) ^ b44; \
a3 = (kreg6) ^ b45; \
a4 = (kreg24) ^ b46; \
a5 = (kreg18) ^ b47; \
a6 = k6 ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = (kreg16) ^ e72; \
a2 = (kreg21) ^ e73; \
a3 = (kreg29) ^ e74; \
a4 = k8 ^ e75; \
a5 = (kreg3) ^ e76; \
a6 = (kreg0) ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = k0 ^ e78; \
a2 = (kreg23) ^ e79; \
a3 = (kreg28) ^ e80; \
a4 = k9 ^ e81; \
a5 = (kreg15) ^ e82; \
a6 = k17 ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = k16 ^ b55; \
a2 = k10 ^ b56; \
a3 = (kreg30) ^ b57; \
a4 = k14 ^ b58; \
a5 = (kreg8) ^ b59; \
a6 = (kreg14) ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = k2 ^ b59; \
a2 = (kreg9) ^ b60; \
a3 = k3 ^ b61; \
a4 = k15 ^ b62; \
a5 = k7 ^ b63; \
a6 = (kreg22) ^ b32; \
S8( b4 , b26 , b14 , b20 ); \
a1 = k18 ^ e0; \
a2 = (kreg31) ^ e1; \
a3 = (kreg18) ^ e2; \
a4 = (kreg13) ^ e3; \
a5 = (kreg33) ^ e4; \
a6 = (kreg5) ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = (kreg19) ^ e6; \
a2 = k6 ^ e7; \
a3 = k11 ^ e8; \
a4 = k19 ^ e9; \
a5 = (kreg34) ^ e10; \
a6 = (kreg25) ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = (kreg20) ^ b7; \
a2 = (kreg17) ^ b8; \
a3 = (kreg11) ^ b9; \
a4 = (kreg12) ^ b10; \
a5 = k20 ^ b11; \
a6 = k5 ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = k4 ^ b11; \
a2 = k12 ^ b12; \
a3 = k13 ^ b13; \
a4 = (kreg10) ^ b14; \
a5 = (kreg4) ^ b15; \
a6 = (kreg26) ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = (kreg2) ^ e24; \
a2 = (kreg7) ^ e25; \
a3 = (kreg15) ^ e26; \
a4 = (kreg30) ^ e27; \
a5 = k10 ^ e28; \
a6 = k7 ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = (kreg22) ^ e30; \
a2 = (kreg9) ^ e31; \
a3 = (kreg14) ^ e32; \
a4 = (kreg3) ^ e33; \
a5 = (kreg1) ^ e34; \
a6 = k3 ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = k2 ^ b23; \
a2 = (kreg28) ^ b24; \
a3 = (kreg16) ^ b25; \
a4 = k0 ^ b26; \
a5 = k15 ^ b27; \
a6 = (kreg0) ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = k17 ^ b27; \
a2 = k16 ^ b28; \
a3 = (kreg21) ^ b29; \
a4 = k1 ^ b30; \
a5 = (kreg29) ^ b31; \
a6 = (kreg8) ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = k4 ^ e48; \
a2 = (kreg17) ^ e49; \
a3 = (kreg4) ^ e50; \
a4 = k20 ^ e51; \
a5 = (kreg19) ^ e52; \
a6 = k12 ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = (kreg5) ^ e54; \
a2 = (kreg26) ^ e55; \
a3 = (kreg31) ^ e56; \
a4 = k5 ^ e57; \
a5 = (kreg20) ^ e58; \
a6 = (kreg11) ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = (kreg6) ^ b39; \
a2 = (kreg34) ^ b40; \
a3 = k18 ^ b41; \
a4 = k19 ^ b42; \
a5 = k6 ^ b43; \
a6 = (kreg25) ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = (kreg24) ^ b43; \
a2 = (kreg32) ^ b44; \
a3 = (kreg33) ^ b45; \
a4 = (kreg27) ^ b46; \
a5 = k11 ^ b47; \
a6 = (kreg12) ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = k9 ^ e72; \
a2 = k14 ^ e73; \
a3 = (kreg1) ^ e74; \
a4 = (kreg16) ^ e75; \
a5 = (kreg28) ^ e76; \
a6 = (kreg29) ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = (kreg8) ^ e78; \
a2 = k16 ^ e79; \
a3 = (kreg0) ^ e80; \
a4 = k10 ^ e81; \
a5 = k8 ^ e82; \
a6 = (kreg21) ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = k17 ^ b55; \
a2 = (kreg14) ^ b56; \
a3 = (kreg2) ^ b57; \
a4 = (kreg22) ^ b58; \
a5 = k1 ^ b59; \
a6 = k7 ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = k3 ^ b59; \
a2 = k2 ^ b60; \
a3 = (kreg7) ^ b61; \
a4 = (kreg23) ^ b62; \
a5 = (kreg15) ^ b63; \
a6 = k15 ^ b32; \
S8( b4 , b26 , b14 , b20 ); \
a1 = (kreg24) ^ e0; \
a2 = (kreg34) ^ e1; \
a3 = k11 ^ e2; \
a4 = k6 ^ e3; \
a5 = (kreg5) ^ e4; \
a6 = (kreg32) ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = k12 ^ e6; \
a2 = (kreg12) ^ e7; \
a3 = (kreg17) ^ e8; \
a4 = (kreg25) ^ e9; \
a5 = (kreg6) ^ e10; \
a6 = k18 ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = k13 ^ b7; \
a2 = (kreg20) ^ b8; \
a3 = k4 ^ b9; \
a4 = k5 ^ b10; \
a5 = (kreg26) ^ b11; \
a6 = (kreg11) ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = (kreg10) ^ b11; \
a2 = (kreg18) ^ b12; \
a3 = (kreg19) ^ b13; \
a4 = (kreg13) ^ b14; \
a5 = (kreg31) ^ b15; \
a6 = k19 ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = (kreg3) ^ e24; \
a2 = k0 ^ e25; \
a3 = k8 ^ e26; \
a4 = (kreg2) ^ e27; \
a5 = (kreg14) ^ e28; \
a6 = (kreg15) ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = k15 ^ e30; \
a2 = k2 ^ e31; \
a3 = k7 ^ e32; \
a4 = (kreg28) ^ e33; \
a5 = (kreg30) ^ e34; \
a6 = (kreg7) ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = k3 ^ b23; \
a2 = (kreg0) ^ b24; \
a3 = k9 ^ b25; \
a4 = (kreg8) ^ b26; \
a5 = (kreg23) ^ b27; \
a6 = (kreg29) ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = (kreg21) ^ b27; \
a2 = k17 ^ b28; \
a3 = k14 ^ b29; \
a4 = (kreg9) ^ b30; \
a5 = (kreg1) ^ b31; \
a6 = k1 ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = (kreg10) ^ e48; \
a2 = (kreg20) ^ e49; \
a3 = (kreg31) ^ e50; \
a4 = (kreg26) ^ e51; \
a5 = k12 ^ e52; \
a6 = (kreg18) ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = (kreg32) ^ e54; \
a2 = k19 ^ e55; \
a3 = (kreg34) ^ e56; \
a4 = (kreg11) ^ e57; \
a5 = k13 ^ e58; \
a6 = k4 ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = (kreg33) ^ b39; \
a2 = (kreg6) ^ b40; \
a3 = (kreg24) ^ b41; \
a4 = (kreg25) ^ b42; \
a5 = (kreg12) ^ b43; \
a6 = k18 ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = (kreg27) ^ b43; \
a2 = (kreg4) ^ b44; \
a3 = (kreg5) ^ b45; \
a4 = k20 ^ b46; \
a5 = (kreg17) ^ b47; \
a6 = k5 ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = k10 ^ e72; \
a2 = (kreg22) ^ e73; \
a3 = (kreg30) ^ e74; \
a4 = k9 ^ e75; \
a5 = (kreg0) ^ e76; \
a6 = (kreg1) ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = k1 ^ e78; \
a2 = k17 ^ e79; \
a3 = (kreg29) ^ e80; \
a4 = (kreg14) ^ e81; \
a5 = (kreg16) ^ e82; \
a6 = k14 ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = (kreg21) ^ b55; \
a2 = k7 ^ b56; \
a3 = (kreg3) ^ b57; \
a4 = k15 ^ b58; \
a5 = (kreg9) ^ b59; \
a6 = (kreg15) ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = (kreg7) ^ b59; \
a2 = k3 ^ b60; \
a3 = k0 ^ b61; \
a4 = k16 ^ b62; \
a5 = k8 ^ b63; \
a6 = (kreg23) ^ b32; \
S8( b4 , b26 , b14 , b20 ); \
a1 = (kreg34) ^ e0; \
a2 = (kreg13) ^ e1; \
a3 = (kreg24) ^ e2; \
a4 = (kreg19) ^ e3; \
a5 = k5 ^ e4; \
a6 = (kreg11) ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = (kreg25) ^ e6; \
a2 = k12 ^ e7; \
a3 = (kreg27) ^ e8; \
a4 = (kreg4) ^ e9; \
a5 = k6 ^ e10; \
a6 = (kreg31) ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = (kreg26) ^ b7; \
a2 = k20 ^ b8; \
a3 = (kreg17) ^ b9; \
a4 = (kreg18) ^ b10; \
a5 = (kreg5) ^ b11; \
a6 = k11 ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = (kreg20) ^ b11; \
a2 = k18 ^ b12; \
a3 = k19 ^ b13; \
a4 = k13 ^ b14; \
a5 = (kreg10) ^ b15; \
a6 = (kreg32) ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = k3 ^ e24; \
a2 = (kreg15) ^ e25; \
a3 = (kreg23) ^ e26; \
a4 = k2 ^ e27; \
a5 = k14 ^ e28; \
a6 = k15 ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = (kreg30) ^ e30; \
a2 = k10 ^ e31; \
a3 = (kreg22) ^ e32; \
a4 = (kreg7) ^ e33; \
a5 = (kreg9) ^ e34; \
a6 = k7 ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = (kreg14) ^ b23; \
a2 = k0 ^ b24; \
a3 = k17 ^ b25; \
a4 = k8 ^ b26; \
a5 = (kreg2) ^ b27; \
a6 = (kreg8) ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = (kreg0) ^ b27; \
a2 = (kreg28) ^ b28; \
a3 = (kreg29) ^ b29; \
a4 = k9 ^ b30; \
a5 = k1 ^ b31; \
a6 = (kreg16) ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = (kreg20) ^ e48; \
a2 = k20 ^ e49; \
a3 = (kreg10) ^ e50; \
a4 = (kreg5) ^ e51; \
a5 = (kreg25) ^ e52; \
a6 = k18 ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = (kreg11) ^ e54; \
a2 = (kreg32) ^ e55; \
a3 = (kreg13) ^ e56; \
a4 = k11 ^ e57; \
a5 = (kreg26) ^ e58; \
a6 = (kreg17) ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = (kreg12) ^ b39; \
a2 = k6 ^ b40; \
a3 = (kreg34) ^ b41; \
a4 = (kreg4) ^ b42; \
a5 = k12 ^ b43; \
a6 = (kreg31) ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = (kreg6) ^ b43; \
a2 = k4 ^ b44; \
a3 = k5 ^ b45; \
a4 = (kreg33) ^ b46; \
a5 = (kreg27) ^ b47; \
a6 = (kreg18) ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = (kreg21) ^ e72; \
a2 = (kreg1) ^ e73; \
a3 = (kreg9) ^ e74; \
a4 = k17 ^ e75; \
a5 = k0 ^ e76; \
a6 = k1 ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = (kreg16) ^ e78; \
a2 = (kreg28) ^ e79; \
a3 = (kreg8) ^ e80; \
a4 = k14 ^ e81; \
a5 = k16 ^ e82; \
a6 = (kreg29) ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = (kreg0) ^ b55; \
a2 = (kreg22) ^ b56; \
a3 = k3 ^ b57; \
a4 = (kreg30) ^ b58; \
a5 = k9 ^ b59; \
a6 = k15 ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = k7 ^ b59; \
a2 = (kreg14) ^ b60; \
a3 = (kreg15) ^ b61; \
a4 = (kreg3) ^ b62; \
a5 = (kreg23) ^ b63; \
a6 = (kreg2) ^ b32; \
S8( b4 , b26 , b14 , b20 ); \
a1 = (kreg6) ^ e0; \
a2 = k6 ^ e1; \
a3 = (kreg27) ^ e2; \
a4 = k12 ^ e3; \
a5 = (kreg11) ^ e4; \
a6 = k4 ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = k18 ^ e6; \
a2 = (kreg18) ^ e7; \
a3 = k20 ^ e8; \
a4 = (kreg31) ^ e9; \
a5 = (kreg12) ^ e10; \
a6 = (kreg34) ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = k19 ^ b7; \
a2 = (kreg26) ^ b8; \
a3 = (kreg20) ^ b9; \
a4 = k11 ^ b10; \
a5 = (kreg32) ^ b11; \
a6 = (kreg17) ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = k13 ^ b11; \
a2 = (kreg24) ^ b12; \
a3 = (kreg25) ^ b13; \
a4 = (kreg19) ^ b14; \
a5 = (kreg13) ^ b15; \
a6 = (kreg4) ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = (kreg7) ^ e24; \
a2 = k8 ^ e25; \
a3 = k16 ^ e26; \
a4 = k3 ^ e27; \
a5 = (kreg22) ^ e28; \
a6 = (kreg23) ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = (kreg2) ^ e30; \
a2 = (kreg14) ^ e31; \
a3 = k15 ^ e32; \
a4 = k0 ^ e33; \
a5 = k2 ^ e34; \
a6 = (kreg15) ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = k7 ^ b23; \
a2 = (kreg8) ^ b24; \
a3 = (kreg21) ^ b25; \
a4 = (kreg16) ^ b26; \
a5 = (kreg3) ^ b27; \
a6 = k1 ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = (kreg29) ^ b27; \
a2 = (kreg0) ^ b28; \
a3 = (kreg1) ^ b29; \
a4 = k10 ^ b30; \
a5 = (kreg9) ^ b31; \
a6 = k9 ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = k13 ^ e48; \
a2 = (kreg26) ^ e49; \
a3 = (kreg13) ^ e50; \
a4 = (kreg32) ^ e51; \
a5 = k18 ^ e52; \
a6 = (kreg24) ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = k4 ^ e54; \
a2 = (kreg4) ^ e55; \
a3 = k6 ^ e56; \
a4 = (kreg17) ^ e57; \
a5 = k19 ^ e58; \
a6 = (kreg20) ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = k5 ^ b39; \
a2 = (kreg12) ^ b40; \
a3 = (kreg6) ^ b41; \
a4 = (kreg31) ^ b42; \
a5 = (kreg18) ^ b43; \
a6 = (kreg34) ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = (kreg33) ^ b43; \
a2 = (kreg10) ^ b44; \
a3 = (kreg11) ^ b45; \
a4 = (kreg5) ^ b46; \
a5 = k20 ^ b47; \
a6 = k11 ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = k14 ^ e72; \
a2 = (kreg30) ^ e73; \
a3 = k2 ^ e74; \
a4 = (kreg21) ^ e75; \
a5 = (kreg8) ^ e76; \
a6 = (kreg9) ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = k9 ^ e78; \
a2 = (kreg0) ^ e79; \
a3 = k1 ^ e80; \
a4 = (kreg22) ^ e81; \
a5 = k17 ^ e82; \
a6 = (kreg1) ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = (kreg29) ^ b55; \
a2 = k15 ^ b56; \
a3 = (kreg7) ^ b57; \
a4 = (kreg2) ^ b58; \
a5 = k10 ^ b59; \
a6 = (kreg23) ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = (kreg15) ^ b59; \
a2 = k7 ^ b60; \
a3 = k8 ^ b61; \
a4 = (kreg28) ^ b62; \
a5 = k16 ^ b63; \
a6 = (kreg3) ^ b32; \
S8( b4 , b26 , b14 , b20 ); \
a1 = (kreg33) ^ e0; \
a2 = (kreg12) ^ e1; \
a3 = k20 ^ e2; \
a4 = (kreg18) ^ e3; \
a5 = k4 ^ e4; \
a6 = (kreg10) ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = (kreg24) ^ e6; \
a2 = k11 ^ e7; \
a3 = (kreg26) ^ e8; \
a4 = (kreg34) ^ e9; \
a5 = k5 ^ e10; \
a6 = (kreg6) ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = (kreg25) ^ b7; \
a2 = k19 ^ b8; \
a3 = k13 ^ b9; \
a4 = (kreg17) ^ b10; \
a5 = (kreg4) ^ b11; \
a6 = (kreg20) ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = (kreg19) ^ b11; \
a2 = (kreg27) ^ b12; \
a3 = k18 ^ b13; \
a4 = k12 ^ b14; \
a5 = k6 ^ b15; \
a6 = (kreg31) ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = k0 ^ e24; \
a2 = (kreg16) ^ e25; \
a3 = k17 ^ e26; \
a4 = (kreg7) ^ e27; \
a5 = k15 ^ e28; \
a6 = k16 ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = (kreg3) ^ e30; \
a2 = k7 ^ e31; \
a3 = (kreg23) ^ e32; \
a4 = (kreg8) ^ e33; \
a5 = k3 ^ e34; \
a6 = k8 ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = (kreg15) ^ b23; \
a2 = k1 ^ b24; \
a3 = k14 ^ b25; \
a4 = k9 ^ b26; \
a5 = (kreg28) ^ b27; \
a6 = (kreg9) ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = (kreg1) ^ b27; \
a2 = (kreg29) ^ b28; \
a3 = (kreg30) ^ b29; \
a4 = (kreg14) ^ b30; \
a5 = k2 ^ b31; \
a6 = k10 ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = (kreg19) ^ e48; \
a2 = k19 ^ e49; \
a3 = k6 ^ e50; \
a4 = (kreg4) ^ e51; \
a5 = (kreg24) ^ e52; \
a6 = (kreg27) ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = (kreg10) ^ e54; \
a2 = (kreg31) ^ e55; \
a3 = (kreg12) ^ e56; \
a4 = (kreg20) ^ e57; \
a5 = (kreg25) ^ e58; \
a6 = k13 ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = (kreg11) ^ b39; \
a2 = k5 ^ b40; \
a3 = (kreg33) ^ b41; \
a4 = (kreg34) ^ b42; \
a5 = k11 ^ b43; \
a6 = (kreg6) ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = (kreg5) ^ b43; \
a2 = (kreg13) ^ b44; \
a3 = k4 ^ b45; \
a4 = (kreg32) ^ b46; \
a5 = (kreg26) ^ b47; \
a6 = (kreg17) ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = (kreg22) ^ e72; \
a2 = (kreg2) ^ e73; \
a3 = k3 ^ e74; \
a4 = k14 ^ e75; \
a5 = k1 ^ e76; \
a6 = k2 ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = k10 ^ e78; \
a2 = (kreg29) ^ e79; \
a3 = (kreg9) ^ e80; \
a4 = k15 ^ e81; \
a5 = (kreg21) ^ e82; \
a6 = (kreg30) ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = (kreg1) ^ b55; \
a2 = (kreg23) ^ b56; \
a3 = k0 ^ b57; \
a4 = (kreg3) ^ b58; \
a5 = (kreg14) ^ b59; \
a6 = k16 ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = k8 ^ b59; \
a2 = (kreg15) ^ b60; \
a3 = (kreg16) ^ b61; \
a4 = (kreg0) ^ b62; \
a5 = k17 ^ b63; \
a6 = (kreg28) ^ b32; \
S8( b4 , b26 , b14 , b20 ); \
a1 = (kreg5) ^ e0; \
a2 = k5 ^ e1; \
a3 = (kreg26) ^ e2; \
a4 = k11 ^ e3; \
a5 = (kreg10) ^ e4; \
a6 = (kreg13) ^ e5; \
S1( b40 , b48 , b54 , b62 ); \
a1 = (kreg27) ^ e6; \
a2 = (kreg17) ^ e7; \
a3 = k19 ^ e8; \
a4 = (kreg6) ^ e9; \
a5 = (kreg11) ^ e10; \
a6 = (kreg33) ^ e11; \
S2( b44 , b59 , b33 , b49 ); \
a1 = k18 ^ b7; \
a2 = (kreg25) ^ b8; \
a3 = (kreg19) ^ b9; \
a4 = (kreg20) ^ b10; \
a5 = (kreg31) ^ b11; \
a6 = k13 ^ b12; \
S3( b55 , b47 , b61 , b37 ); \
a1 = k12 ^ b11; \
a2 = k20 ^ b12; \
a3 = (kreg24) ^ b13; \
a4 = (kreg18) ^ b14; \
a5 = (kreg12) ^ b15; \
a6 = (kreg34) ^ b16; \
S4( b57 , b51 , b41 , b32 ); \
a1 = (kreg8) ^ e24; \
a2 = k9 ^ e25; \
a3 = (kreg21) ^ e26; \
a4 = k0 ^ e27; \
a5 = (kreg23) ^ e28; \
a6 = k17 ^ e29; \
S5( b39 , b45 , b56 , b34 ); \
a1 = (kreg28) ^ e30; \
a2 = (kreg15) ^ e31; \
a3 = k16 ^ e32; \
a4 = k1 ^ e33; \
a5 = (kreg7) ^ e34; \
a6 = (kreg16) ^ e35; \
S6( b35 , b60 , b42 , b50 ); \
a1 = k8 ^ b23; \
a2 = (kreg9) ^ b24; \
a3 = (kreg22) ^ b25; \
a4 = k10 ^ b26; \
a5 = (kreg0) ^ b27; \
a6 = k2 ^ b28; \
S7( b63 , b43 , b53 , b38 ); \
a1 = (kreg30) ^ b27; \
a2 = (kreg1) ^ b28; \
a3 = (kreg2) ^ b29; \
a4 = k7 ^ b30; \
a5 = k3 ^ b31; \
a6 = (kreg14) ^ b0; \
S8( b36 , b58 , b46 , b52 ); \
a1 = k19 ^ e48; \
a2 = (kreg32) ^ e49; \
a3 = (kreg19) ^ e50; \
a4 = k4 ^ e51; \
a5 = (kreg34) ^ e52; \
a6 = (kreg6) ^ e53; \
S1( b8 , b16 , b22 , b30 ); \
a1 = (kreg20) ^ e54; \
a2 = (kreg10) ^ e55; \
a3 = k12 ^ e56; \
a4 = k20 ^ e57; \
a5 = (kreg4) ^ e58; \
a6 = (kreg26) ^ e59; \
S2( b12 , b27 , b1 , b17 ); \
a1 = k11 ^ b39; \
a2 = (kreg18) ^ b40; \
a3 = (kreg12) ^ b41; \
a4 = (kreg13) ^ b42; \
a5 = (kreg24) ^ b43; \
a6 = k6 ^ b44; \
S3( b23 , b15 , b29 , b5 ); \
a1 = k5 ^ b43; \
a2 = k13 ^ b44; \
a3 = (kreg17) ^ b45; \
a4 = (kreg11) ^ b46; \
a5 = (kreg5) ^ b47; \
a6 = (kreg27) ^ b48; \
S4( b25 , b19 , b9 , b0 ); \
a1 = (kreg1) ^ e72; \
a2 = k2 ^ e73; \
a3 = (kreg14) ^ e74; \
a4 = (kreg29) ^ e75; \
a5 = (kreg16) ^ e76; \
a6 = k10 ^ e77; \
S5( b7 , b13 , b24 , b2 ); \
a1 = (kreg21) ^ e78; \
a2 = (kreg8) ^ e79; \
a3 = k9 ^ e80; \
a4 = (kreg30) ^ e81; \
a5 = (kreg0) ^ e82; \
a6 = (kreg9) ^ e83; \
S6( b3 , b28 , b10 , b18 ); \
a1 = k1 ^ b55; \
a2 = (kreg2) ^ b56; \
a3 = (kreg15) ^ b57; \
a4 = k3 ^ b58; \
a5 = k14 ^ b59; \
a6 = (kreg3) ^ b60; \
S7( b31 , b11 , b21 , b6 ); \
a1 = (kreg23) ^ b59; \
a2 = k15 ^ b60; \
a3 = k16 ^ b61; \
a4 = k0 ^ b62; \
a5 = (kreg28) ^ b63; \
a6 = (kreg7) ^ b32; \
S8( b4 , b26 , b14 , b20 ); 
