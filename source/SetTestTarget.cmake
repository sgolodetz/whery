#######################
# SetTestTarget.cmake #
#######################

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${whery_BINARY_DIR}/bin/tests/${targetname})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${whery_BINARY_DIR}/bin/tests/${targetname})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${whery_BINARY_DIR}/bin/tests/${targetname})
ADD_EXECUTABLE(${targetname} ${sources} ${headers} ${templates})
INCLUDE(${whery_SOURCE_DIR}/VCLibraryHack.cmake)

SET_TARGET_PROPERTIES(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}_d")