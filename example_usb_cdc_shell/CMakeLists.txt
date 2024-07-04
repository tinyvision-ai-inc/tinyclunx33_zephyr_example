cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)
target_sources(app PRIVATE src/main.c)
zephyr_code_relocate(FILES src/main.c LOCATION RAM)
#zephyr_code_relocate(FILES src/main.c LOCATION RAM_DATA)
zephyr_code_relocate(LIBRARY drivers__usb__udc LOCATION RAM)

