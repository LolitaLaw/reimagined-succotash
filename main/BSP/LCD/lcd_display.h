#ifndef __LCD_DISPLAY_H__
#define __LCD_DISPLAY_H__

#include <stdint.h>
#include <stdbool.h>

// LCD RAM基地址
#define LCD_RAM_BASE_ADDR 0x3E00

/* COM占bit23~31，SEG占bit0~22 */
/* COM */
#define COM0 0x0000
#define COM1 0x0800
#define COM2 0x1000
#define COM3 0x1800
#define COM4 0x2000
#define COM5 0x2800
#define COM6 0x3000
#define COM7 0x3800
#define COM8 0x4000

/* WN18xx WN19xx SEG */
#define SEG0 0x0001
#define SEG1 0x0002
#define SEG2 0x0004
#define SEG3 0x0008
#define SEG4 0x0010
#define SEG5 0x0020
#define SEG6 0x0040
#define SEG7 0x0080

#define SEG8 0x0101
#define SEG9 0x0102
#define SEG10 0x0104
#define SEG11 0x0108
#define SEG12 0x0110
#define SEG13 0x0120
#define SEG14 0x0140
#define SEG15 0x0180

#define SEG16 0x0201
#define SEG17 0x0202
#define SEG18 0x0204
#define SEG19 0x0208
#define SEG20 0x0210
#define SEG21 0x0220
#define SEG22 0x0240
#define SEG23 0x0280

#define SEG24 0x0301
#define SEG25 0x0302
#define SEG26 0x0304
#define SEG27 0x0308
#define SEG28 0x0310
#define SEG29 0x0320
#define SEG30 0x0340
#define SEG31 0x0380

#define SEG32 0x0401
#define SEG33 0x0402
#define SEG34 0x0404
#define SEG35 0x0408
#define SEG36 0x0410
#define SEG37 0x0420
#define SEG38 0x0440
#define SEG39 0x0480

#define SEG40 0x0501
#define SEG41 0x0502
#define SEG42 0x0504
#define SEG43 0x0508
#define SEG44 0x0510
#define SEG45 0x0520
#define SEG46 0x0540
#define SEG47 0x0580

#define SEG48 0x0601
#define SEG49 0x0602
#define SEG50 0x0604
#define SEG51 0x0608
#define SEG52 0x0610
#define SEG53 0x0620
#define SEG54 0x0640
#define SEG55 0x0680

#define SEG56 0x0701
#define SEG57 0x0702
#define SEG58 0x0704
#define SEG59 0x0708
#define SEG60 0x0710
#define SEG61 0x0720
#define SEG62 0x0740
#define SEG63 0x0780

/* WN38xx WN39xx SEG */
#define ___SEG56 0x0001
#define ___SEG55 0x0002
#define ___SEG54 0x0004
#define ___SEG53 0x0008
#define ___SEG59 0x0010
#define ___SEG58 0x0020
#define ___SEG57 0x0040
#define ___SEG52 0x0080

#define ___SEG51 0x0101
#define ___SEG50 0x0102
#define ___SEG49 0x0104
#define ___SEG48 0x0108
#define ___SEG47 0x0110
#define ___SEG46 0x0120
#define ___SEG45 0x0140
#define ___SEG44 0x0180

#define ___SEG43 0x0201
#define ___SEG42 0x0202
#define ___SEG41 0x0204
#define ___SEG40 0x0208
#define ___SEG39 0x0210
#define ___SEG38 0x0220
#define ___SEG37 0x0240
#define ___SEG36 0x0280

#define ___SEG35 0x0301
#define ___SEG34 0x0302
#define ___SEG33 0x0304
#define ___SEG32 0x0308
#define ___SEG31 0x0310
#define ___SEG30 0x0320
#define ___SEG29 0x0340
#define ___SEG28 0x0380

#define ___SEG27 0x0401
#define ___SEG26 0x0402
#define ___SEG25 0x0404
#define ___SEG24 0x0408
#define ___SEG23 0x0410
#define ___SEG22 0x0420
#define ___SEG21 0x0440
#define ___SEG20 0x0480

#define ___SEG19 0x0501
#define ___SEG18 0x0502
#define ___SEG17 0x0504
#define ___SEG16 0x0508
#define ___SEG15 0x0510
#define ___SEG14 0x0520
#define ___SEG13 0x0540
#define ___SEG12 0x0580

#define ___SEG11 0x0601
#define ___SEG10 0x0602
#define ___SEG9 0x0604
#define ___SEG8 0x0608
#define ___SEG7 0x0610
#define ___SEG0 0x0620
#define ___SEG1 0x0640
#define ___SEG2 0x0680

#define ___SEG3 0x0701
#define ___SEG6 0x0702
#define ___SEG5 0x0704
#define ___SEG4 0x0708

// 字符映射表索引
enum
{
    Shuzi_0 = 0,
    Shuzi_1,
    Shuzi_2,
    Shuzi_3,
    Shuzi_4,
    Shuzi_5,
    Shuzi_6,
    Shuzi_7,
    Shuzi_8,
    Shuzi_9,
    Shuzi_A,
    Shuzi_b,
    Shuzi_C,
    Shuzi_d,
    Shuzi_E,
    Shuzi_F,
    Shuzi_H,
    Shuzi_I,
    Shuzi_L,
    Shuzi_N,
    Shuzi_O,
    Shuzi_P,
    Shuzi_U,
    Shuzi_o1,
    Shuzi_o2,
    Shuzi_NEG,
    Shuzi_NULL,
    Shuzi_Max
};

#endif // LCD_DISPLAY