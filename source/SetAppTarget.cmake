######################
# SetAppTarget.cmake #
######################

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${whery_BINARY_DIR}/bin/apps/${targetname})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${whery_BINARY_DIR}/bin/apps/${targetname})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${whery_BINARY_DIR}/bin/apps/${targetname})
ADD_EXECUTABLE(${targetname} ${sources} ${headers} ${templates} ${others})
INCLUDE(${whery_SOURCE_DIR}/VCLibraryHack.cmake)

SET_TARGET_PROPERTIES(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}_d")