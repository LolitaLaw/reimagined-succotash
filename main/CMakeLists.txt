idf_component_register(
SRCS
"main.c"
"BSP/bsp_uart.c"
"BSP/SPI/bsp_spi.c"
"BSP/SPI/bsp_spi2.c"
"BSP/SPI/bsp_soft_spi.c"
"BSP/bsp_lcd.c"
"BSP/bsp_key.c"
"BSP/GPL811X.c"
"BSP/bsp_wifi.c"

INCLUDE_DIRS
"." "BSP" "BSP/SPI" "BSP/LCD"
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-variable)