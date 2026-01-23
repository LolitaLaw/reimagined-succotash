#ifndef __GPL811X__H__
#define __GPL811X__H__

#include <soc/cpu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/portmacro.h"
#include "esp_log.h"
#include "BSP/spi/bsp_spi2.h"
#include "BSP/spi/bsp_soft_spi.h"
#include "BSP/bsp_config.h"

#define GPL811X_OTP
#define GPL811P08Ax 1

#define P_System_Ctrl_Base 0x0000 // 基地址
#define P_AUX_Ctrl_Base 0x0050    // 辅助控制寄存器基地址
#define P_SPI_Ctrl_Base 0x3010    // SPI控制寄存器基地址
#define P_ADC_Ctrl_Base 0x3040    // ADC控制寄存器基地址

#if defined(GPL811P04Ax) || defined(GPL811P04Bx) || defined(GPL811P08Ax) || defined(GPL811P08Bx)
#define GPL811X
#endif

#ifdef GPL811X
//==================  IO Register(寄存器)  ==================
// PortA, PortC, PortD, PortE 都包含数据、方向和属性寄存器三个部分
#define P_IO_PortA_Dir (P_System_Ctrl_Base + 0x00)  //  0x00;
#define P_IO_PortA_Data (P_System_Ctrl_Base + 0x02) //  0x02;

#define P_IO_PortC_Dir (P_System_Ctrl_Base + 0x01)  //  0x01;
#define P_IO_PortC_Data (P_System_Ctrl_Base + 0x05) //  0x05;

#define P_IO_PortD_Attrib (P_System_Ctrl_Base + 0x15) //* 属性寄存器 Attribution
#define P_IO_PortD_Dir (P_System_Ctrl_Base + 0x11)    //* 方向寄存器 Direction
#define P_IO_PortD_Data (P_System_Ctrl_Base + 0x13)   //* 数据寄存器 Data

#define P_IO_PortE_Dir (P_System_Ctrl_Base + 0x0B)       //* 方向寄存器 Direction
#define P_IO_PortE_Data (P_System_Ctrl_Base + 0x03)      //* 输入数据寄存器 Data
#define P_IO_PortE_DataLatch (P_System_Ctrl_Base + 0x04) //* 数据锁存寄存器 Data Latch

#define P_IO_Ctrl (P_System_Ctrl_Base + 0x06) //  0x06;
// 按键矩阵扫描掩码
#define D_IOA_3_2_KeyScan 0b10000000 // A口 Bit3~2
#define D_IOA_1_0_KeyScan 0b01000000 // A口 Bit1~0
#define D_IOA_3_0_KeyScan 0b11000000 // A口 Bit3~0
#define D_IOC_3_0_KeyScan 0b00010000 // C口 Bit3~0
#define D_IOE_7_4_KeyScan 0b00001000 // E口 Bit7~4
#define D_IOE_3_2_KeyScan 0b00000100 // E口 Bit3~2
// #define D_IOE_0_KeyScan 0b00000001   // E口 Bit0
// #define D_IOE_1_KeyScan 0b00000010   // E口 Bit1
#define D_IOE_2_KeyScan 0b00000100   // E口 Bit2
#define D_IOE_3_KeyScan 0b00001000   // E口 Bit3
#define D_IOE_4_KeyScan 0b00010000   // E口 Bit4
#define D_IOE_5_KeyScan 0b00100000   // E口 Bit5
#define D_IOE_6_KeyScan 0b01000000   // E口 Bit6
#define D_IOE_7_KeyScan 0b10000000   // E口 Bit7
#define D_IOE_7_0_KeyScan 0b00001111 // E口 Bit7~0

#define D_IOD_3_KeyScan 0b00001000 // D口 Bit3
#define D_IOD_4_KeyScan 0b00010000 // D口 Bit4
// #define D_IOD_7_KeyScan 0b10000000   // D口 Bit7
// #define D_IOD_6_KeyScan 0b01000000   // D口 Bit6
// #define D_IOD_5_KeyScan 0b00100000   // D口 Bit5
// #define D_IOD_2_KeyScan 0b00000100   // D口 Bit2
// #define D_IOD_1_KeyScan 0b00000010   // D口 Bit1
// #define D_IOD_0_KeyScan 0b00000001   // D口 Bit0
#define D_IOD_5_0_KeyScan 0b00111111 // D口 Bit5~0
//==================  Sleep/WakeUp  =================
#define P_WAKEUP_Ctrl (P_System_Ctrl_Base + 0x08) //  0x08;
#define D_WakeupTimer 0b00010000                  // 00010000B
#define D_WakeupTmx 0b00001000                    // 00001000B
#define D_WakeupTMBB 0b00001000                   // 00001000B
#define D_Wakeup128Hz 0b00000100                  // 00000100B
#define D_Wakeup2Hz 0b00000010                    // 00000010B
#define D_WakeupTMBA 0b00000010                   // 00000010B
#define D_WakeupKey 0b00000001                    // 00000001B

#define P_SYSTEM_Ctrl (P_System_Ctrl_Base + 0x09) //  0x09;

//=================  Timer/Counter  =================
#define P_TIMER_TimeBase_Sel (P_System_Ctrl_Base + 0x0A) //  0x0A;
#define D_TimeXMask 0b10000111                           // 10000111B
#define D_T2Hz 0b00000000                                // 00000000B
#define D_TMBA_2Hz 0b00000000                            // 00000000B
#define D_T1Hz 0b10000000                                // 10000000B
#define D_TMBA_1Hz 0b10000000                            // 10000000B
#define D_T4Hz 0b00000000                                // 00000000B
#define D_TMBB_4Hz 0b00000000                            // 00000000B
#define D_T8Hz 0b00000001                                // 00000001B
#define D_TMBB_8Hz 0b00000001                            // 00000001B
#define D_T16Hz 0b00000010                               // 00000010B
#define D_TMBB_16Hz 0b00000010                           // 00000010B
#define D_T32Hz 0b00000011                               // 00000011B
#define D_TMBB_32Hz 0b00000011                           // 00000011B
#define D_T64Hz 0b00000100                               // 00000100B
#define D_TMBB_64Hz 0b00000100                           // 00000100B
#define D_T128Hz 0b00000101                              // 00000101B
#define D_TMBB_128Hz 0b00000101                          // 00000101B
#define D_T1kHz 0b00000110                               // 00000110B
#define D_TMBB_1KHz 0b00000110                           // 00000110B
#define D_T4kHz 0b00000111                               // 00000111B
#define D_TMBB_4KHz 0b00000111                           // 00000111B

#define P_TIMER_Timer0_Ctrl (P_System_Ctrl_Base + 0x22) //  0x22;
#define D_TimerEn 0x80                                  // 10000000B
#define D_TimerSrcExtA 0b00000000                       // 00000000B
#define D_TimerSrcExtA2 0b00100000                      // 00100000B
#define D_TimerSrcA_HF 0b00000000                       // 00000000B
#define D_TimerSrcA_HFDIV 0b00000001                    // 00000001B
#define D_TimerSrcA_EXT 0b00000010                      // 00000010B
#define D_TimerSrcA_VDD 0x03                            // 00000011B
#define D_TimerSrcB_LF 0x00                             // 00000000B
#define D_TimerSrcB_TMBB 0b00000100                     // 00000100B
#define D_TimerSrcB_EXTA 0b00001000                     // 00001000B
#define D_TimerSrcB_VDD 0b00001100                      // 00001100B

#define P_TIMER_TM0Data_LB (P_System_Ctrl_Base + 0x20)   //  0x20;
#define P_TIMER_TM0Data_HB (P_System_Ctrl_Base + 0x21)   //  0x21;
#define P_TIMER_TM0Load_Ctrl (P_System_Ctrl_Base + 0x23) //  0x23;

#define P_TIMER_TM1Data (P_System_Ctrl_Base + 0x25)     //  0x25;
#define P_TIMER_Timer1_Ctrl (P_System_Ctrl_Base + 0x26) //  0x26;

#define D_Timer1En 0b10000000     // 10000000B
#define D_Timer1_LF 0b00000000    // 00000000B
#define D_Timer1_TMBB 0b00000001  // 00000001B
#define D_Timer1_HF 0b00000010    // 00000010B
#define D_Timer1_HFDIV 0b00000011 // 00000011B

#define P_TIMER_TM1Load_Ctrl (P_System_Ctrl_Base + 0x27) //  0x27;

//==============  CPU and CLK Register  =============
#define P_CLK_CPU_Ctrl (P_System_Ctrl_Base + 0x18) //  0x18;
#define D_RCHF_Msk 0b11100000                      // 11100000B
#define D_RCHF225K 0b00000000                      // 00000000B;225k
#define D_RCHF500K 0b01000000                      // 01000000B;500k
#define D_RCHF1800K 0b10000000                     // 10000000B;1800k
#define D_RCHF1000K 0b11000000                     // 11000000B;1000k

#define D_CPU_Msk 0b00000111  // 00000111B
#define D_CPU_HF 0b00000000   // 00000000B;HF
#define D_CPU_HF2 0b00000001  // 00000001B;HF/2
#define D_CPU_HF4 0b00000010  // 00000010B;HF/4
#define D_CPU_HF8 0b00000011  // 00000011B;HF/8
#define D_CPU_HF16 0b00000100 // 00000100B;HF/16

#define P_CLK_HFDIV_Ctrl (P_System_Ctrl_Base + 0x1A) //  0x1A;
#define D_HFDIV_HF2 0b00000000                       // 00000000B;HF/2
#define D_HFDIV_HF4 0b00000001                       // 00000001B;HF/4
#define D_HFDIV_HF8 0b00000010                       // 00000010B;HF/8
#define D_HFDIV_HF16 0b00000011                      // 00000011B;HF/16

#define P_CLK_32768_En (P_System_Ctrl_Base + 0x0C) //  0x0C;
#define D_LFEnable 0b00000000                      // 00000000B       ;low active
#define D_LFDisable 0b10000000                     // 10000000B

//====================  INT/NMI  ====================
#define P_INT_Ctrl (P_System_Ctrl_Base + 0x0D) //  0x0D;
#define D_IntMask 0b11111111                   // 11111111B
#define D_LvdInt 0b01000000                    // 01000000B
#define D_Timer0Int 0b00100000                 // 00100000B
#define D_TMBBInt 0b00010000                   // 00010000B
#define D_TMBAInt 0b00001000                   // 00001000B
#define D_128HzInt 0b00000100                  // 00000100B
#define D_2KHzInt 0b00000010                   // 00000010B
#define D_ExtInt 0b00000001                    // 00000001B

#define P_INT_Ext_Clear (P_System_Ctrl_Base + 0x40)       //  0x40;
#define P_INT_2KHz_Clear (P_System_Ctrl_Base + 0x41)      //  0x41;
#define P_INT_128Hz_Clear (P_System_Ctrl_Base + 0x42)     //  0x42;
#define P_INT_TimeBaseA_Clear (P_System_Ctrl_Base + 0x43) //  0x43;
#define P_INT_TimeBaseB_Clear (P_System_Ctrl_Base + 0x44) //  0x44;
#define P_INT_Timer0_Clear (P_System_Ctrl_Base + 0x45)    //  0x45;

#define P_INT_Ctrl2 (P_System_Ctrl_Base + 0x2D) //  0x2D;
#define D_Int2Mask 0b00000011                   // 00000011B
#define D_Timer1Int 0b00000001                  // 00000001B

#define P_INT_Timer1_Clear (P_System_Ctrl_Base + 0x48) //  0x48;

//===================  LCD Driver  ==================
#define P_LCD_Ctrl (P_System_Ctrl_Base + 0x07) //  0x07;
#define D_LcdConfigMask 0b11101001             // 11101001B
#define D_Duty_1_11 0b00000000                 // 00000000B
#define D_Duty_1_10 0b01000000                 // 01000000B
#define D_Duty_1_9 0b10000000                  // 10000000B
#define D_Duty_1_8 0b11000000                  // 11000000B
#define D_Duty_1_6 0b00001000                  // 00001000B
#define D_Duty_1_5 0b01001000                  // 01001000B
#define D_Duty_1_4 0b10001000                  // 10001000B
#define D_Duty_1_3 0b11001000                  // 11001000B
#define D_Bias_1_4 0b00000000                  // 00000000B
#define D_Bias_1_3 0b00100000                  // 00100000B
#define D_ScanEnable 0b00000001                // 00000001B

#define P_LCD_VLCD_Ctrl (P_System_Ctrl_Base + 0x19) //  0x19;
#define D_VLCD_Msk 0b00000111                       // 00000111B
#define D_Bias_1_3_VLCD_3V0 0b00000000              // 00000000B
#define D_Bias_1_3_VLCD_3V375 0b00000001            // 00000001B
#define D_Bias_1_3_VLCD_3V75 0b00000010             // 00000010B
#define D_Bias_1_3_VLCD_4V125 0b00000011            // 00000011B
#define D_Bias_1_3_VLCD_4V5 0b00000111              // 00000111B

#define D_Bias_1_4_VLCD_4V0 0b00000000 // 00000000B
#define D_Bias_1_4_VLCD_4V5 0b00000001 // 00000001B
#define D_Bias_1_4_VLCD_5V0 0b00000010 // 00000010B
#define D_Bias_1_4_VLCD_5V5 0b00000011 // 00000011B
#define D_Bias_1_4_VLCD_6V0 0b00000111 // 00000111B

//======================  WDT  ======================
#define P_WDT_Flag_Clear (P_System_Ctrl_Base + 0x0F) //  0x0F;

//==================  ACON Register  ==================
#define P_ACON_Reset_Flag (P_System_Ctrl_Base + 0x0E) //  0x0E;
#define D_FlagC 0b00000001                            // 00000001B;Reset in normal mode
#define D_FlagON 0b00000010                           // 00000010B;Reset in standby mode

//==================  AUX Register  ===================
#define P_AUX_DataX (P_AUX_Ctrl_Base + 0x00)        // 0x50;
#define P_AUX_DataY (P_AUX_Ctrl_Base + 0x01)        // 0x51;
#define P_AUX_DataX_0H (P_AUX_Ctrl_Base + 0x02)     // 0x52;
#define P_AUX_DataX_0L (P_AUX_Ctrl_Base + 0x03)     // 0x53;
#define P_AUX_DataY_0H (P_AUX_Ctrl_Base + 0x04)     // 0x54;
#define P_AUX_DataY_0L (P_AUX_Ctrl_Base + 0x05)     // 0x55;
#define P_AUX_DataX_H0 (P_AUX_Ctrl_Base + 0x06)     // 0x56;
#define P_AUX_DataX_L0 (P_AUX_Ctrl_Base + 0x07)     // 0x57;
#define P_AUX_DataY_H0 (P_AUX_Ctrl_Base + 0x08)     // 0x58;
#define P_AUX_DataY_L0 (P_AUX_Ctrl_Base + 0x09)     // 0x59;
#define P_AUX_DataX_LH (P_AUX_Ctrl_Base + 0x0A)     // 0x5A;
#define P_AUX_DataY_LH (P_AUX_Ctrl_Base + 0x0B)     // 0x5B;
#define P_AUX_DataXY_XLYH (P_AUX_Ctrl_Base + 0x0C)  // 0x5C;
#define P_AUX_DataXY_YLXH (P_AUX_Ctrl_Base + 0x0D)  // 0x5D;
#define P_AUX_DataX_Mirror (P_AUX_Ctrl_Base + 0x0E) // 0x5E;
#define P_AUX_DataY_Mirror (P_AUX_Ctrl_Base + 0x0F) // 0x5F;

//=================  AUDIO Register  ================
#define P_AUDIO_BZ_Ctrl (P_System_Ctrl_Base + 0x16) // 0x16;
#define D_BuzzerOutEn 0b00000011                    // 00000011B
#define D_BuzzerOutDisable 0b00000000               // 00000000B
#define D_PD0BuzOut_PD1NormalIO 0x01                // 00000001B
#define D_PD1BuzOut_PD0NormalIO 0b00000010          // 00000010B

//======================  Bank  =====================
#if defined(GPL811P08Ax) || defined(GPL811P08Bx)
#define P_BANK_Sel (P_System_Ctrl_Base + 0x30) // 0x30;
#define D_BankMsk 0b00000001                   // 00000001B
#endif
//==================  LVD Register  ===================
#define P_LVD_Ctrl (P_System_Ctrl_Base + 0x1B) // 0x1B;
//    #define D_LVD_2V0                0b00000000                    //00000000B
//    #define D_LVD_2V2                0b00000001                    //00000001B
//    #define D_LVD_2V4                0b00000010                    //00000010B
//    #define D_LVD_2V6                0b00000011                    //00000011B
//----------------------------------------------------------------------------------
// Modify at 2016-7-21 11:23:37
// Ref. to "GPL811P0XB_pro_V10_R.pdf" Page 59 13.2 $001B P_LVD_Status
//----------------------------------------------------------------------------------
#define D_LVD_2V0 0b00000000    // 00000000B (R/W)
#define D_LVD_2V2 0b01000000    // 01000000B (R/W)
#define D_LVD_2V4 0b10000000    // 10000000B (R/W)
#define D_LVD_2V6 0b11000000    // 11000000B (R/W)
#define D_LVD_Status 0b00000001 // 00000001B (R)

#define P_INT_LVD_Clear (P_System_Ctrl_Base + 0x46) // 0x46;

//==================  SPI Register  ===================
#define P_SPI_Ctrl (P_SPI_Ctrl_Base + 0x00)      // 0x3010;
#define P_SPI_TX_Status (P_SPI_Ctrl_Base + 0x01) // 0x3011;
#define P_SPI_TX_Ctrl (P_SPI_Ctrl_Base + 0x02)   // 0x3012;
#define P_SPI_TX_Data (P_SPI_Ctrl_Base + 0x03)   // 0x3013;
#define P_SPI_RX_Status (P_SPI_Ctrl_Base + 0x04) // 0x3014;
#define P_SPI_RX_Ctrl (P_SPI_Ctrl_Base + 0x05)   // 0x3015;
#define P_SPI_RX_Data (P_SPI_Ctrl_Base + 0x06)   // 0x3016;
#define P_SPI_Misc_Ctrl (P_SPI_Ctrl_Base + 0x07) // 0x3017;
#define P_SPI_Port_Ctrl (P_SPI_Ctrl_Base + 0x08) // 0x3018;

//==============  RFC Control Register  =============
#define P_RFC_Ctrl (P_System_Ctrl_Base + 0x17) // 0x17;

//==============  ADC Control Register  =============
#if defined(GPL811P04Bx) || defined(GPL811P08Bx)
#define P_ADC_Ctrl (P_ADC_Ctrl_Base + 0x00)      // 0x3040
#define P_ADC_INT_Ctrl (P_ADC_Ctrl_Base + 0x01)  // 0x3041
#define P_ADC_Conv_Ctrl (P_ADC_Ctrl_Base + 0x04) // 0x3044
#define P_ADC_DataL (P_ADC_Ctrl_Base + 0x05)     // 0x3045
#define P_ADC_DataH (P_ADC_Ctrl_Base + 0x06)     // 0x3046
#endif

#endif

#define P_LCDRAM_Addr 0x3E00

extern uint8_t T_DirBuf[4]; //
extern uint8_t T_DataBuf[4];
#define PortA 0
#define PortC 1
#define PortD 2
#define PortE 3

// GPIO_PIN
#define GPIO_PIN_0 0x01
#define GPIO_PIN_1 0x02
#define GPIO_PIN_2 0x04
#define GPIO_PIN_3 0x08
#define GPIO_PIN_4 0x10
#define GPIO_PIN_5 0x20
#define GPIO_PIN_6 0x40
#define GPIO_PIN_7 0x80

#define KEY_PD_MASK 0x18
#define KEY_PE_MASK 0xFC
// GPIOA 开启下拉输入
#define GPIOA_OPENPD_INPUT(_PIN)                                \
    clr(T_DirBuf[PortA], _PIN),                                 \
        clr(T_DataBuf[PortA], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortA_Dir, T_DirBuf[PortA]), \
        F_GPL811Reg_WriteData(P_IO_PortA_Data, T_DataBuf[PortA])

// GPIOC 开启下拉输入
#define GPIOC_OPENPD_INPUT(_PIN)                                \
    clr(T_DirBuf[PortC], _PIN),                                 \
        clr(T_DataBuf[PortC], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortC_Dir, T_DirBuf[PortC]), \
        F_GPL811Reg_WriteData(P_IO_PortC_Data, T_DataBuf[PortC])

// GPIOE 开启下拉输入
#define GPIOE_OPENPD_INPUT(_PIN)                                \
    clr(T_DirBuf[PortE], _PIN),                                 \
        clr(T_DataBuf[PortE], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortE_Dir, T_DirBuf[PortE]), \
        F_GPL811Reg_WriteData(P_IO_PortE_Data, T_DataBuf[PortE])

// GPIOD 开启下拉输入
#define GPIOD_OPENPD_INPUT(_PIN)                                \
    clr(T_DirBuf[PortD], _PIN),                                 \
        clr(T_DataBuf[PortD], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortD_Dir, T_DirBuf[PortD]), \
        F_GPL811Reg_WriteData(P_IO_PortD_Data, T_DataBuf[PortD])

//--------------------------------------------------------
//                      IO口 悬浮输入
//--------------------------------------------------------
// GPIOA 悬浮输入
#define GPIOA_FLOAT_INPUT(_PIN)                                 \
    clr(T_DirBuf[PortA], _PIN),                                 \
        set(T_DataBuf[PortA], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortA_Dir, T_DirBuf[PortA]), \
        F_GPL811Reg_WriteData(P_IO_PortA_Data, T_DataBuf[PortA])

// GPIOC 悬浮输入
#define GPIOC_FLOAT_INPUT(_PIN)                                 \
    clr(T_DirBuf[PortC], _PIN),                                 \
        set(T_DataBuf[PortC], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortC_Dir, T_DirBuf[PortC]), \
        F_GPL811Reg_WriteData(P_IO_PortC_Data, T_DataBuf[PortC])

// GPIOE 悬浮输入
#define GPIOE_FLOAT_INPUT(_PIN)                                 \
    clr(T_DirBuf[PortE], _PIN),                                 \
        set(T_DataBuf[PortE], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortE_Dir, T_DirBuf[PortE]), \
        F_GPL811Reg_WriteData(P_IO_PortE_Data, T_DataBuf[PortE])

// GPIOD 悬浮输入
#define GPIOD_FLOAT_INPUT(_PIN)                                 \
    clr(T_DirBuf[PortD], _PIN),                                 \
        set(T_DataBuf[PortD], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortD_Dir, T_DirBuf[PortD]), \
        F_GPL811Reg_WriteData(P_IO_PortD_Data, T_DataBuf[PortD])

//--------------------------------------------------------
//                      IO口 输出1
//--------------------------------------------------------
// GPIOA 输出1
#define GPIOA_OUTPUT1(_PIN)                                     \
    set(T_DirBuf[PortA], _PIN),                                 \
        set(T_DataBuf[PortA], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortA_Dir, T_DirBuf[PortA]), \
        F_GPL811Reg_WriteData(P_IO_PortA_Data, T_DataBuf[PortA])

// GPIOC 输出1
#define GPIOC_OUTPUT1(_PIN)                                     \
    set(T_DirBuf[PortC], _PIN),                                 \
        set(T_DataBuf[PortC], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortC_Dir, T_DirBuf[PortC]), \
        F_GPL811Reg_WriteData(P_IO_PortC_Data, T_DataBuf[PortC])

// GPIOE 输出1
#define GPIOE_OUTPUT1(_PIN)                                     \
    set(T_DirBuf[PortE], _PIN),                                 \
        set(T_DataBuf[PortE], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortE_Dir, T_DirBuf[PortE]), \
        F_GPL811Reg_WriteData(P_IO_PortE_Data, T_DataBuf[PortE])

// GPIOD 输出1
#define GPIOD_OUTPUT1(_PIN)                                     \
    set(T_DirBuf[PortD], _PIN),                                 \
        set(T_DataBuf[PortD], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortD_Dir, T_DirBuf[PortD]), \
        F_GPL811Reg_WriteData(P_IO_PortD_Data, T_DataBuf[PortD])

//--------------------------------------------------------
//                      IO口 输出0
//--------------------------------------------------------
// GPIOA 输出0
#define GPIOA_OUTPUT0(_PIN)                                     \
    set(T_DirBuf[PortA], _PIN),                                 \
        clr(T_DataBuf[PortA], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortA_Dir, T_DirBuf[PortA]), \
        F_GPL811Reg_WriteData(P_IO_PortA_Data, T_DataBuf[PortA])

// GPIOC 输出0
#define GPIOC_OUTPUT0(_PIN)                                     \
    set(T_DirBuf[PortC], _PIN),                                 \
        clr(T_DataBuf[PortC], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortC_Dir, T_DirBuf[PortC]), \
        F_GPL811Reg_WriteData(P_IO_PortC_Data, T_DataBuf[PortC])

// GPIOE 输出0
#define GPIOE_OUTPUT0(_PIN)                                     \
    set(T_DirBuf[PortE], _PIN),                                 \
        clr(T_DataBuf[PortE], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortE_Dir, T_DirBuf[PortE]), \
        F_GPL811Reg_WriteData(P_IO_PortE_Data, T_DataBuf[PortE])

// GPIOD 输出0
#define GPIOD_OUTPUT0(_PIN)                                     \
    set(T_DirBuf[PortD], _PIN),                                 \
        clr(T_DataBuf[PortD], _PIN),                            \
        F_GPL811Reg_WriteData(P_IO_PortD_Dir, T_DirBuf[PortD]), \
        F_GPL811Reg_WriteData(P_IO_PortD_Data, T_DataBuf[PortD])

#define D_BuzzerRata (0x10000 - (30720 / 4000))
#define BUZ_PIN GPIO_PIN_0

#define DC_PIN GPIO_PIN_1

#define JP_MASK 0x3E
#define _JP1_B 0x02
#define _JP2_B 0x04
#define _JP3_B 0x08
#define _JP4_B 0x10
#define _JP5_B 0x20

#define KEY_MASK 0x7C
#define KEY_SET 0x04
#define KEY_UP 0x08
#define KEY_DOWN 0x10
#define KEY_TEMP 0x20
#define _KEY_LIGHT 0x40

// void F_GPL811Reg_WriteData(uint16_t RegAddr, uint8_t data);
// uint8_t F_GPL811Reg_ReadData(uint16_t RegAddr);

// void F_GPL811Reg_SetBits(uint16_t RegAddr, uint8_t bits);
// void F_GPL811Reg_ClrBits(uint16_t RegAddr, uint8_t bits);
// void F_GPL811Reg_TogBits(uint16_t RegAddr, uint8_t bits);
// uint8_t F_GPL811Reg_TstBits(uint16_t RegAddr, uint8_t bits);

// void F_GPL811X_Init(void);
// void F_detect_jp_status(void);
// void F_WaitGPL811Ready(void);
// void F_InitLcd(void);
void lcd_init(void);

#endif
