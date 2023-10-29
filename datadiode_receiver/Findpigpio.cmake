# Findpigpio.cmake
find_library(PIGPIO_LIBRARY NAMES pigpio)
find_path(PIGPIO_INCLUDE_DIR NAMES pigpio.h)

if (PIGPIO_LIBRARY AND PIGPIO_INCLUDE_DIR)
    set(PIGPIO_FOUND TRUE)
else()
    set(PIGPIO_FOUND FALSE)
endif()

mark_as_advanced(PIGPIO_LIBRARY PIGPIO_INCLUDE_DIR)