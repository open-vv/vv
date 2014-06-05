include(../../cmake/common.cmake)

add_library(clitkCxImage STATIC
    ximaenc.cpp
    ximatran.cpp
    ximath.cpp
    ximaint.cpp
    ximage.cpp
    ximagif.cpp
    ximainfo.cpp
    ximalpha.cpp
    ximalyr.cpp
    ximapal.cpp
    ximasel.cpp
    xmemfile.cpp
    )
include_directories(.)

