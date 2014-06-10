# - Find ROOT instalation
# This module tries to find the ROOT installation on your system.
# It tries to find the root-config script which gives you all the needed information.
# If the system variable ROOTSYS is set this is straight forward.
# If not the module uses the pathes given in ROOT_CONFIG_SEARCHPATH.
# If you need an other path you should add this path to this varaible.
# The root-config script is then used to detect basically everything else.
# This module defines a number of key variables and macros.


message(STATUS "Looking for Root...")

set(ROOT_DEFINITIONS "")

set(ROOT_INSTALLED_VERSION_TOO_OLD FALSE)

set(ROOT_CONFIG_EXECUTABLE ROOT_CONFIG_EXECUTABLE-NOTFOUND)

find_program(ROOT_CONFIG_EXECUTABLE NAMES root-config PATHS)

if(${ROOT_CONFIG_EXECUTABLE} MATCHES "ROOT_CONFIG_EXECUTABLE-NOTFOUND")
  message( FATAL_ERROR "ROOT not installed in the searchpath and ROOTSYS is not set. Please
 set ROOTSYS or add the path to your ROOT installation in the Macro FindROOT.cmake in the
 subdirectory cmake/modules.")
else(${ROOT_CONFIG_EXECUTABLE} MATCHES "ROOT_CONFIG_EXECUTABLE-NOTFOUND")
		message(STATUS "root-config found")
  string(REGEX REPLACE "(^.*)/bin/root-config" "\\1" test ${ROOT_CONFIG_EXECUTABLE})
  set( ENV{ROOTSYS} ${test})
  set( ROOTSYS ${test})
endif(${ROOT_CONFIG_EXECUTABLE} MATCHES "ROOT_CONFIG_EXECUTABLE-NOTFOUND")


if(ROOT_CONFIG_EXECUTABLE)

  set(ROOT_FOUND FALSE)

  exec_program(${ROOT_CONFIG_EXECUTABLE} ARGS "--version" OUTPUT_VARIABLE ROOTVERSION)

  message(STATUS "Looking for Root... - found $ENV{ROOTSYS}/bin/root")
  message(STATUS "Looking for Root... - version ${ROOTVERSION} ")

  # we need at least version 5.00/00
  if(NOT ROOT_MIN_VERSION)
    set(ROOT_MIN_VERSION "5.00/00")
  endif(NOT ROOT_MIN_VERSION)

  # now parse the parts of the user given version string into variables
  string(REGEX REPLACE "^([0-9]+)\\.[0-9][0-9]+\\/[0-9][0-9]+" "\\1" req_root_major_vers "${ROOT_MIN_VERSION}")
  string(REGEX REPLACE "^[0-9]+\\.([0-9][0-9])+\\/[0-9][0-9]+.*" "\\1" req_root_minor_vers "${ROOT_MIN_VERSION}")
  string(REGEX REPLACE "^[0-9]+\\.[0-9][0-9]+\\/([0-9][0-9]+)" "\\1" req_root_patch_vers "${ROOT_MIN_VERSION}")

  # and now the version string given by qmake
  string(REGEX REPLACE "^([0-9]+)\\.[0-9][0-9]+\\/[0-9][0-9]+.*" "\\1" found_root_major_vers "${ROOTVERSION}")
  string(REGEX REPLACE "^[0-9]+\\.([0-9][0-9])+\\/[0-9][0-9]+.*" "\\1" found_root_minor_vers "${ROOTVERSION}")
  string(REGEX REPLACE "^[0-9]+\\.[0-9][0-9]+\\/([0-9][0-9]+).*" "\\1" found_root_patch_vers "${ROOTVERSION}")

  if(found_root_major_vers LESS 5)
    message( FATAL_ERROR "Invalid ROOT version \"${ROOTERSION}\", at least major version 4 is required, e.g. \"5.00/00\"")
  endif(found_root_major_vers LESS 5)

  # compute an overall version number which can be compared at once
  math(EXPR req_vers "${req_root_major_vers}*10000 + ${req_root_minor_vers}*100 + ${req_root_patch_vers}")
  math(EXPR found_vers "${found_root_major_vers}*10000 + ${found_root_minor_vers}*100 + ${found_root_patch_vers}")

  if(found_vers LESS req_vers)
    set(ROOT_FOUND FALSE)
    set(ROOT_INSTALLED_VERSION_TOO_OLD TRUE)
  else(found_vers LESS req_vers)
    set(ROOT_FOUND TRUE)
  endif(found_vers LESS req_vers)

endif(ROOT_CONFIG_EXECUTABLE)

#message("root found = "${ROOT_FOUND})

if(ROOT_FOUND)

  # ask root-config for the library dir
  # Set ROOT_LIBRARY_DIR

  exec_program( ${ROOT_CONFIG_EXECUTABLE}
    ARGS "--libdir"
    OUTPUT_VARIABLE ROOT_LIBRARY_DIR_TMP )

  if(EXISTS "${ROOT_LIBRARY_DIR_TMP}")
    set(ROOT_LIBRARY_DIR ${ROOT_LIBRARY_DIR_TMP} )
  else(EXISTS "${ROOT_LIBRARY_DIR_TMP}")
    message("Warning: ROOT_CONFIG_EXECUTABLE reported ${ROOT_LIBRARY_DIR_TMP} as library path,")
    message("Warning: but ${ROOT_LIBRARY_DIR_TMP} does NOT exist, ROOT must NOT be installed correctly.")
  endif(EXISTS "${ROOT_LIBRARY_DIR_TMP}")

  # ask root-config for the binary dir
  exec_program(${ROOT_CONFIG_EXECUTABLE}
    ARGS "--bindir"
    OUTPUT_VARIABLE root_bins )
  set(ROOT_BINARY_DIR ${root_bins})

  # ask root-config for the include dir
  exec_program( ${ROOT_CONFIG_EXECUTABLE}
    ARGS "--incdir"
    OUTPUT_VARIABLE root_headers )
  set(ROOT_INCLUDE_DIR ${root_headers})
      # CACHE INTERNAL "")

  # ask root-config for the library varaibles
  exec_program( ${ROOT_CONFIG_EXECUTABLE}
#    ARGS "--noldflags --noauxlibs --libs"
    ARGS "--glibs"
    OUTPUT_VARIABLE root_flags )

#  string(REGEX MATCHALL "([^ ])+"  root_libs_all ${root_flags})
#  string(REGEX MATCHALL "-L([^ ])+"  root_library ${root_flags})
#  REMOVE_FROM_LIST(root_flags "${root_libs_all}" "${root_library}")

  set(ROOT_LIBRARIES ${root_flags})

  # Make variables changeble to the advanced user
  mark_as_advanced( ROOT_LIBRARY_DIR ROOT_INCLUDE_DIR ROOT_DEFINITIONS)

  # Set ROOT_INCLUDES
  set( ROOT_INCLUDES ${ROOT_INCLUDE_DIR})

  set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${ROOT_LIBRARY_DIR})

  #######################################
  #
  #       Check the executables of ROOT
  #          ( rootcint )
  #
  #######################################

  find_program(ROOT_CINT_EXECUTABLE
    NAMES rootcint
    PATHS ${ROOT_BINARY_DIR}
    NO_DEFAULT_PATH
    )

endif(ROOT_FOUND)


#message("icici")

  ###########################################
  #
  #       Macros for building ROOT dictionary
  #
  ###########################################

macro(ROOT_GENERATE_DICTIONARY_OLD )

   set(INFILES "")

   foreach (_current_FILE ${ARGN})

     if(${_current_FILE} MATCHES "^.*\\.h$")
       if(${_current_FILE} MATCHES "^.*Link.*$")
         set(LINKDEF_FILE ${_current_FILE})
       else(${_current_FILE} MATCHES "^.*Link.*$")
         set(INFILES ${INFILES} ${_current_FILE})
       endif(${_current_FILE} MATCHES "^.*Link.*$")
     else(${_current_FILE} MATCHES "^.*\\.h$")
       if(${_current_FILE} MATCHES "^.*\\.cxx$")
         set(OUTFILE ${_current_FILE})
       else(${_current_FILE} MATCHES "^.*\\.cxx$")
         set(INCLUDE_DIRS ${INCLUDE_DIRS} -I${_current_FILE})
       endif(${_current_FILE} MATCHES "^.*\\.cxx$")
     endif(${_current_FILE} MATCHES "^.*\\.h$")

   endforeach (_current_FILE ${ARGN})

#  message("INFILES: ${INFILES}")
#  message("OutFILE: ${OUTFILE}")
#  message("LINKDEF_FILE: ${LINKDEF_FILE}")
#  message("INCLUDE_DIRS: ${INCLUDE_DIRS}")

   string(REGEX REPLACE "(^.*).cxx" "\\1.h" bla "${OUTFILE}")
#   message("BLA: ${bla}")
   set(OUTFILES ${OUTFILE} ${bla})

   add_custom_command(OUTPUT ${OUTFILES}
      COMMAND ${ROOT_CINT_EXECUTABLE}
      ARGS -f ${OUTFILE} -c -DHAVE_CONFIG_H ${INCLUDE_DIRS} ${INFILES} ${LINKDEF_FILE} DEPENDS ${INFILES})

#   message("ROOT_CINT_EXECUTABLE has created the dictionary ${OUTFILE}")

endmacro(ROOT_GENERATE_DICTIONARY_OLD)

  ###########################################
  #
  #       Macros for building ROOT dictionary
  #
  ###########################################

macro(ROOT_GENERATE_DICTIONARY INFILES LINKDEF_FILE OUTFILE INCLUDE_DIRS_IN)

  set(INCLUDE_DIRS)

  foreach (_current_FILE ${INCLUDE_DIRS_IN})
    set(INCLUDE_DIRS ${INCLUDE_DIRS} -I${_current_FILE})
  endforeach (_current_FILE ${INCLUDE_DIRS_IN})


#  message("INFILES: ${INFILES}")
#  message("OutFILE: ${OUTFILE}")
#  message("LINKDEF_FILE: ${LINKDEF_FILE}")
#  message("INCLUDE_DIRS: ${INCLUDE_DIRS}")

  string(REGEX REPLACE "^(.*)\\.(.*)$" "\\1.h" bla "${OUTFILE}")
#  message("BLA: ${bla}")
  set(OUTFILES ${OUTFILE} ${bla})

  add_custom_command(OUTPUT ${OUTFILES}
     COMMAND ${ROOT_CINT_EXECUTABLE}
     ARGS -f ${OUTFILE} -c -DHAVE_CONFIG_H ${INCLUDE_DIRS} ${INFILES} ${LINKDEF_FILE} DEPENDS ${INFILES})

endmacro(ROOT_GENERATE_DICTIONARY)

#message("la")
