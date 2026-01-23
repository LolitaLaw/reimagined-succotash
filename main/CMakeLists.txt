idf_component_register(
SRCS
"main.c"
"BSP/bsp_uart.c"
"BSP/spi/bsp_spi.c"
"BSP/spi/bsp_spi2.c"
"BSP/spi/bsp_soft_spi.c"
"BSP/bsp_key.c"
"BSP/GPL811X.c"
INCLUDE_DIRS
"." "BSP" "BSP/spi"
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-variable)