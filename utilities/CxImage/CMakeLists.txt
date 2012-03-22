INCLUDE(../../cmake/common.cmake)

ADD_LIBRARY(clitkCxImage STATIC
    ximaenc.cpp
    ximage.cpp
    ximagif.cpp
    ximainfo.cpp
    ximalpha.cpp
    ximalyr.cpp
    ximapal.cpp
    ximasel.cpp
    xmemfile.cpp
    )
INCLUDE_DIRECTORIES(.)

