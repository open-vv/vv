#=========================================================
MACRO (DD in)
    MESSAGE(${in}=${${in}})
ENDMACRO(DD)
#=========================================================
# Process ggo (gengetopt) files (http://www.gnu.org/software/gengetopt/)
MACRO (WRAP_GGO GGO_SRCS)
    FOREACH(GGO_FILE ${ARGN})
        GET_FILENAME_COMPONENT(GGO_BASEFILENAME ${GGO_FILE} NAME_WE)
        GET_FILENAME_COMPONENT(GGO_FILE_ABS ${GGO_FILE} ABSOLUTE)
        SET(GGO_H ${GGO_BASEFILENAME}_ggo.h)
        SET(GGO_C ${GGO_BASEFILENAME}_ggo.c)
        SET(GGO_OUTPUT ${GGO_H} ${GGO_C})
        ADD_CUSTOM_COMMAND(OUTPUT ${GGO_OUTPUT}
                           COMMAND ${CLITK_GENGETOPT}/gengetopt
                           ARGS < ${GGO_FILE_ABS}
                                  --output-dir=${CMAKE_CURRENT_BINARY_DIR}
                                  --arg-struct-name=args_info_${GGO_BASEFILENAME}
                                  --func-name=cmdline_parser_${GGO_BASEFILENAME}
                                  --file-name=${GGO_BASEFILENAME}_ggo
                                  --unamed-opts
                                  --conf-parser
                                  --include-getopt
                           DEPENDS ${GGO_FILE}
                          )  
	SET(${GGO_SRCS} ${${GGO_SRCS}} ${CMAKE_CURRENT_BINARY_DIR}/${GGO_BASEFILENAME}_ggo.c)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})  #For _ggo.h includes
    ENDFOREACH(GGO_FILE)
ENDMACRO (WRAP_GGO)

# Reproduce old mechanism with new macro for ggo files (for the time being)
FILE(GLOB ALL_GGO_FILES *.ggo)
WRAP_GGO(ALL_GGO_SRCS ${ALL_GGO_FILES})
#=========================================================
#Set a reasonable build mode default if the user hasn't set any
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)
#=========================================================
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/common)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/tools)         #For _ggo.h includes from other directories
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/segmentation)  #For _ggo.h includes from other directories
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/registration)  #For _ggo.h includes from other directories
#=========================================================
