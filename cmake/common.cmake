#=========================================================
# Find ggo (gengetopt) files (http://www.gnu.org/software/gengetopt/)
FILE(GLOB ALL_GGO_FILES *.ggo)
#MESSAGE(${ALL_GGO_FILES})

FOREACH(GGO_FILE ${ALL_GGO_FILES})
  # MESSAGE(input=${GGO_FILE})  
    STRING(REGEX REPLACE "(.*).ggo" 
      "\\1" GGO_BASEFILENAME
      "${GGO_FILE}")  
  #GET_FILENAME_COMPONENT(GGO_BASEFILENAME ${GGO_FILE} NAME_WE)
  GET_FILENAME_COMPONENT(GGO_NAME ${GGO_BASEFILENAME} NAME)
  # MESSAGE( base= ${GGO_BASEFILENAME})
  # MESSAGE( name= ${GGO_NAME})
  SET(GGO_H ${GGO_BASEFILENAME}_ggo.h)
  #MESSAGE(${GGO_H})
  SET(GGO_C ${GGO_BASEFILENAME}_ggo.c)
  #MESSAGE(${GGO_C}) 
  SET(GGO_OUTPUT  ${GGO_H} ${GGO_C})
  ADD_CUSTOM_COMMAND(OUTPUT ${GGO_OUTPUT} 
 	COMMAND ${CLITK_GENGETOPT}/gengetopt
 	ARGS < ${GGO_FILE} --arg-struct-name=args_info_${GGO_NAME}  --file-name=${GGO_BASEFILENAME}_ggo -u --conf-parser --include-getopt
 	DEPENDS ${GGO_FILE}
 	)  
 ENDFOREACH(GGO_FILE)
#=========================================================
#Set a reasonable build mode default if the user hasn't set any
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

#=========================================================
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/common)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/signal)
#=========================================================
