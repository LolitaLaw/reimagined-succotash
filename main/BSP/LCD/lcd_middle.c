#include "bsp_config.h"

/*------------------------------------------------------------------
              'a1'  1 2 for time.tm_hour
--------------------------------------------------------------------*/
// const uint16_t number_a1[9] =
// {
//     _1B + _1C,                      //1
//     _1A + _1B + _1G + _1E + _1D, 	//2
// };

// /*------------------------------------------------------------------
//               'a1'  1 2 for time day or month
// --------------------------------------------------------------------*/
// const uint16_t number_a2[9] =
// {
//     _2B + _2C,                      //2
//     _2A + _2B + _2G + _2E + _2D, 	//2
//     _2A + _2B + _2C + _2D + _2G,    //3
// };

/*------------------------------------------------------------------
              '8'       a    b    c    d    e    f    g
--------------------------------------------------------------------*/
/* WS3900 屏幕上所有数字 */

const uint16_t number_a1_L4[9] =
    {
        _S167_L4 + _S167_L4,            // 1
        _S166_L4 + _S167_L4 + _S168_L4, // 2
};

const uint16_t number1_L4[7] = {_1A_L4, _1B_L4, _1C_L4, _1D_L4, _1E_L4, _1F_L4, _1G_L4};
const uint16_t number2_L4[7] = {_2A_L4, _2B_L4, _2C_L4, _2D_L4, _2E_L4, _2F_L4, _2G_L4};
const uint16_t number3_L4[7] = {_3A_L4, _3B_L4, _3C_L4, _3D_L4, _3E_L4, _3F_L4, _3G_L4};

const uint16_t number4_L4[7] = {_4A_L4, _4B_L4, _4C_L4, _4D_L4, _4E_L4, _4F_L4, _4G_L4};

const uint16_t number5_L4[7] = {_5A_L4, _5B_L4, _5C_L4, _5D_L4, _5E_L4, _5F_L4, _5G_L4};
const uint16_t number6_L4[7] = {_6A_L4, _6B_L4, _6C_L4, _6D_L4, _6E_L4, _6F_L4, _6G_L4};
const uint16_t number7_L4[7] = {_7A_L4, _7B_L4, _7C_L4, _7D_L4, _7E_L4, _7F_L4, _7G_L4};
const uint16_t number8_L4[7] = {_8A_L4, _8B_L4, _8C_L4, _8D_L4, _8E_L4, _8F_L4, _8G_L4};
const uint16_t number9_L4[7] = {_9A_L4, _9B_L4, _9C_L4, _9D_L4, _9E_L4, _9F_L4, _9G_L4};

const uint16_t number10_L4[7] = {_10A_L4, _10B_L4, _10C_L4, _10D_L4, _10E_L4, _10F_L4, _10G_L4};
const uint16_t number11_L4[7] = {_11A_L4, _11B_L4, _11C_L4, _11D_L4, _11E_L4, _11F_L4, _11G_L4};

const uint16_t number12_L4[7] = {_12A_L4, _12B_L4, _12C_L4, _12D_L4, _12E_L4, _12F_L4, _12G_L4};

const uint16_t number13_L4[7] = {_13A_L4, _13B_L4, _13C_L4, _13D_L4, _13E_L4, _13F_L4, _13G_L4};
const uint16_t number14_L4[7] = {_14A_L4, _14B_L4, _14C_L4, _14D_L4, _14E_L4, _14F_L4, _14G_L4};

const uint16_t number15_L4[7] = {_15A_L4, _15B_L4, _15C_L4, _15D_L4, _15E_L4, _15F_L4, _15G_L4};

const uint16_t number16_L4[7] = {_16A_L4, _16B_L4, _16C_L4, _16D_L4, _16E_L4, _16F_L4, _16G_L4};
const uint16_t number17_L4[7] = {_17A_L4, _17B_L4, _17C_L4, _17D_L4, _17E_L4, _17F_L4, _17G_L4};

const uint16_t number18_L4[7] = {_18A_L4, _18B_L4, _18C_L4, _18D_L4, _18E_L4, _18F_L4, _18G_L4};
const uint16_t number19_L4[7] = {_19A_L4, _19B_L4, _19C_L4, _19D_L4, _19E_L4, _19F_L4, _19G_L4};

const uint16_t number20_L4[7] = {_20A_L4, _20B_L4, _20C_L4, _20D_L4, _20E_L4, _20F_L4, _20G_L4};

const uint16_t number21_L4[7] = {_21A_L4, _21B_L4, _21C_L4, _21D_L4, _21E_L4, _21F_L4, _21G_L4};
const uint16_t number22_L4[7] = {_22A_L4, _22B_L4, _22C_L4, _22D_L4, _22E_L4, _22F_L4, _22G_L4};

const uint16_t number23_L4[7] = {_23A_L4, _23B_L4, _23C_L4, _23D_L4, _23E_L4, _23F_L4, _23G_L4};
const uint16_t number24_L4[7] = {_24A_L4, _24B_L4, _24C_L4, _24D_L4, _24E_L4, _24F_L4, _24G_L4};
const uint16_t number25_L4[7] = {_25A_L4, _25B_L4, _25C_L4, _25D_L4, _25E_L4, _25F_L4, _25G_L4};
const uint16_t number26_L4[7] = {_26A_L4, _26B_L4, _26C_L4, _26D_L4, _26E_L4, _26F_L4, _26G_L4};
const uint16_t number27_L4[7] = {_27A_L4, _27B_L4, _27C_L4, _27D_L4, _27E_L4, _27F_L4, _27G_L4};
const uint16_t number28_L4[7] = {_28A_L4, _28B_L4, _28C_L4, _28D_L4, _28E_L4, _28F_L4, _28G_L4};
const uint16_t number29_L4[7] = {_29A_L4, _29B_L4, _29C_L4, _29D_L4, _29E_L4, _29F_L4, _29G_L4};
const uint16_t number30_L4[7] = {_30A_L4, _30B_L4, _30C_L4, _30D_L4, _30E_L4, _30F_L4, _30G_L4};
const uint16_t number31_L4[7] = {_31A_L4, _31B_L4, _31C_L4, _31D_L4, _31E_L4, _31F_L4, _31G_L4};

const uint16_t number32_L4[7] = {_32A_L4, _32B_L4, _32C_L4, _32D_L4, _32E_L4, _32F_L4, _32G_L4};
const uint16_t number33_L4[7] = {_33A_L4, _33B_L4, _33C_L4, _33D_L4, _33E_L4, _33F_L4, _33G_L4};
const uint16_t number34_L4[7] = {_34A_L4, _34B_L4, _34C_L4, _34D_L4, _34E_L4, _34F_L4, _34G_L4};
const uint16_t number35_L4[7] = {_35A_L4, _35B_L4, _35C_L4, _35D_L4, _35E_L4, _35F_L4, _35G_L4};
const uint16_t number36_L4[7] = {_36A_L4, _36B_L4, _36C_L4, _36D_L4, _36E_L4, _36F_L4, _36G_L4};
const uint16_t number37_L4[7] = {_37A_L4, _37B_L4, _37C_L4, _37D_L4, _37E_L4, _37F_L4, _37G_L4};
const uint16_t number38_L4[7] = {_38A_L4, _38B_L4, _38C_L4, _38D_L4, _38E_L4, _38F_L4, _38G_L4};
const uint16_t number39_L4[7] = {_39A_L4, _39B_L4, _39C_L4, _39D_L4, _39E_L4, _39F_L4, _39G_L4};

/* WS1800 屏幕上的所有字母 */
/*------------------------------------------------------------------
              1 月  Jan    January
              2 月  Feb    February
              3 月  Mar    March
              4 月  Apr    April
              5 月  May    May
              6 月  Jun    June
              7 月  Jul    July
              8 月  Aug    August
              9 月  Sept   Septempber
             10 月  Oct    October
             11 月  Nov    November
             12 月  Dec    December

         a b c Dd e F J l Mm n Oo p r s t u v
--------------------------------------------------------------------*/
/*------------------------------------------------------------------
     '米'  a b r y n l g p t v c
--------------------------------------------------------------------*/
const uint16_t month_char1_L4[] =
    {
        _29A_L4, // a
        _29B_L4, // b
        _29C_L4, // c
        _29D_L4, // d
        _29E_L4, // e
        _29F_L4, // f
        _29G_L4, // g
        _29H_L4, // h

        _29I_L4, // j
        _29J_L4, // k
        _29K_L4, // l
        _29L_L4, // i

        _29M_L4, // m
        _29N_L4, // n
};

/*------------------------------------------------------------------
     '米'  a b r y n l g p t v c
--------------------------------------------------------------------*/
const uint16_t month_char2_L4[] =
    {
        _30A_L4, // a
        _30B_L4, // b
        _30C_L4, // c
        _30D_L4, // d
        _30E_L4, // e
        _30F_L4, // f
        _30G_L4, // g
        _30H_L4, // h

        _30I_L4, // j
        _30J_L4, // k
        _30K_L4, // i
        _30L_L4, // l

        _30M_L4, // m
        _30N_L4, // n
};

/*------------------------------------------------------------------
     '米'  a b r y n l g p t v c
--------------------------------------------------------------------*/
const uint16_t month_char3_L4[] =
    {
        _31A_L4, // a
        _31B_L4, // b
        _31C_L4, // c
        _31D_L4, // d
        _31E_L4, // e
        _31F_L4, // f
        _31G_L4, // g
        _31H_L4, // h

        _31I_L4, // j
        _31J_L4, // k
        _31K_L4, // l
        _31L_L4, // i

        _31M_L4, // m
        _31N_L4, // n
};

/*------------------------------------------------------------------
     '米'  a b r y n l g p t v c
--------------------------------------------------------------------*/
const uint16_t day_char1_L4[] =
    {
        _32A_L4, // a
        _32B_L4, // b
        _32C_L4, // c
        _32D_L4, // d
        _32E_L4, // e
        _32F_L4, // f
        _32G_L4, // g
        // _32K_L4,  //h

        // _32I_L4,   //j
        // _32J_L4,    //k
        // _32L_L4,    //l
        // _32H_L4,    //i

        // _32M_L4,    //m
        // _32N_L4,    //n
};

/*------------------------------------------------------------------
     '米'  a b r y n l g p t v c
--------------------------------------------------------------------*/
const uint16_t day_char2_L4[] =
    {
        _33A_L4, // a
        _33B_L4, // b
        _33C_L4, // c
        _33D_L4, // d
        _33E_L4, // e
        _33F_L4, // f
        _33G_L4, // g
        // _33K_L4,  //h

        // _33I_L4,   //j
        // _33J_L4,    //k
        // _33L_L4,    //l
        // _33H_L4,    //i

        // _33M_L4,    //m
        // _33N_L4,    //n
};

/* WS3900  */
const uint16_t wind_char1_L4[] =
    {
        _7A_L4,
        _7B_L4,
        _7C_L4,
        _7D_L4,
        _7E_L4,
        _7F_L4,
        _7G_L4,
        // Tnull_L4,

        // Tnull_L4,    //
        // Tnull_L4,    //
        // Tnull_L4,    //
        // Tnull_L4,    //

        // _7H_L4,
        // Tnull_L4,    //
};
const uint16_t wind_char2_L4[] =
    {
        _8A_L4,
        _8B_L4,
        _8C_L4,
        _8D_L4,
        _8E_L4,
        _8F_L4,
        _8G_L4,
        // Tnull_L4,

        // Tnull_L4,    //
        // Tnull_L4,    //
        // Tnull_L4,    //
        // Tnull_L4,    //

        // __27H,
        // Tnull_L4,    //
};
const uint16_t wind_char3_L4[] =
    {
        _9A_L4,
        _9B_L4,
        _9C_L4,
        _9D_L4,
        _9E_L4,
        _9F_L4,
        _9G_L4,
        // Tnull_L4,

        // Tnull_L4,    //
        // Tnull_L4,    //
        // Tnull_L4,    //
        // Tnull_L4,    //

        // __28H,
        // Tnull_L4,    //
};
