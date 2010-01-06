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
 	COMMAND gengetopt 
 	ARGS < ${GGO_FILE} --arg-struct-name=gengetopt_args_info_${GGO_NAME}  --file-name=${GGO_BASEFILENAME}_ggo -u --conf-parser
 	DEPENDS ${GGO_FILE}
 	)  
 ENDFOREACH(GGO_FILE)

#=========================================================
#Set a reasonable build mode default if the user hasn't set any
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_BUILD_TYPE)

#=========================================================
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/base)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/grid)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/gridtools)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/signal)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/pose)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/shearwarp)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/synergy)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/reconstruction)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/register)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/deformableregistration)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ilr/optim)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/common)
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tests)
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)
#=========================================================
