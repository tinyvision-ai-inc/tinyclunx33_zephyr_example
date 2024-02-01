cmake_minimum_required(VERSION 3.20.0)

set(CONF_FILE prj.conf litex.conf)
set(DTC_OVERLAY_FILE app.overlay litex.overlay)
set(BOARD litex_vexriscv)
set(BOARD_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)
target_sources(app PRIVATE src/main.c)
