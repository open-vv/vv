
# Attempt to find gengetopt. If not found, compile it.
find_program(GENGETOPT gengetopt)
if(GENGETOPT STREQUAL "GENGETOPT-NOTFOUND")
  get_filename_component(CLITK_CMAKE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
  add_subdirectory(${CLITK_CMAKE_DIR}/../utilities/gengetopt ${CMAKE_CURRENT_BINARY_DIR}/gengetopt)
else(GENGETOPT STREQUAL "GENGETOPT-NOTFOUND")
  add_executable(gengetopt IMPORTED)
  set_property(TARGET gengetopt PROPERTY IMPORTED_LOCATION ${GENGETOPT})
endif(GENGETOPT STREQUAL "GENGETOPT-NOTFOUND")

macro(WRAP_GGO GGO_SRCS)
  foreach(GGO_FILE ${ARGN})
    get_filename_component(GGO_BASEFILENAME ${GGO_FILE} NAME_WE)
    get_filename_component(GGO_FILE_ABS ${GGO_FILE} ABSOLUTE)
    set(GGO_H ${GGO_BASEFILENAME}_ggo.h)
    set(GGO_C ${GGO_BASEFILENAME}_ggo.c)
    set(GGO_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${GGO_H} ${CMAKE_CURRENT_BINARY_DIR}/${GGO_C})
    add_custom_command(OUTPUT ${GGO_OUTPUT}
                       COMMAND gengetopt
                       ARGS < ${GGO_FILE_ABS}
                              --output-dir=${CMAKE_CURRENT_BINARY_DIR}
                              --arg-struct-name=args_info_${GGO_BASEFILENAME}
                              --func-name=cmdline_parser_${GGO_BASEFILENAME}
                              --file-name=${GGO_BASEFILENAME}_ggo
                              --unamed-opts
                              --conf-parser
                              --include-getopt
                       DEPENDS ${GGO_FILE_ABS}
                      )
    set(${GGO_SRCS} ${${GGO_SRCS}} ${GGO_OUTPUT})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
  endforeach(GGO_FILE)
  set_source_files_properties(${${GGO_SRCS}} PROPERTIES GENERATED TRUE)
  if(CMAKE_COMPILER_IS_GNUCXX)
    find_program(DEFAULT_GCC gcc)
    exec_program(${DEFAULT_GCC} ARGS "-dumpversion" OUTPUT_VARIABLE GCCVER)
    if("${GCCVER}" VERSION_GREATER "4.5.2")
      set_source_files_properties(${${GGO_SRCS}} PROPERTIES COMPILE_FLAGS "-Wno-unused-but-set-variable")
    endif("${GCCVER}" VERSION_GREATER "4.5.2")
  endif(CMAKE_COMPILER_IS_GNUCXX)
endmacro(WRAP_GGO)
