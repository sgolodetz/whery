#############################
# CMakeLists.txt for engine #
#############################

###########################
# Specify the target name #
###########################

SET(targetname whery)

#############################
# Specify the project files #
#############################

##
SET(db_base_sources
src/db/base/BackedTuple.cpp
src/db/base/DataPage.cpp
src/db/base/DoubleFieldManipulator.cpp
src/db/base/Field.cpp
src/db/base/FieldManipulator.cpp
src/db/base/FreshTuple.cpp
src/db/base/IntFieldManipulator.cpp
src/db/base/RangeEndpoint.cpp
src/db/base/RangeKey.cpp
src/db/base/TupleComparator.cpp
src/db/base/TupleManipulator.cpp
src/db/base/TupleProjection.cpp
src/db/base/ValueKey.cpp
)

SET(db_base_headers
include/whery/db/base/BackedTuple.h
include/whery/db/base/DataPage.h
include/whery/db/base/DoubleFieldManipulator.h
include/whery/db/base/Field.h
include/whery/db/base/FieldManipulator.h
include/whery/db/base/FreshTuple.h
include/whery/db/base/IntFieldManipulator.h
include/whery/db/base/RangeEndpoint.h
include/whery/db/base/RangeKey.h
include/whery/db/base/SortDirection.h
include/whery/db/base/Tuple.h
include/whery/db/base/TupleComparator.h
include/whery/db/base/TupleManipulator.h
include/whery/db/base/TupleProjection.h
include/whery/db/base/ValueKey.h
)

##
SET(util_sources
src/util/AlignmentTracker.cpp
)

SET(util_headers
include/whery/util/AlignmentTracker.h
)

#################################################################
# Collect the project files into sources, headers and templates #
#################################################################

SET(sources
${db_base_sources}
${util_sources}
)

SET(headers
${db_base_headers}
${util_headers}
)

SET(templates
)

#############################
# Specify the source groups #
#############################

##
SOURCE_GROUP(db\\base\\.cpp FILES ${db_base_sources})
SOURCE_GROUP(db\\base\\.h FILES ${db_base_headers})

##
SOURCE_GROUP(util\\.cpp FILES ${util_sources})
SOURCE_GROUP(util\\.h FILES ${util_headers})

###################################
# Specify the include directories #
###################################

INCLUDE_DIRECTORIES(${whery_SOURCE_DIR}/engine/include)

################################
# Specify the libraries to use #
################################

INCLUDE(${whery_SOURCE_DIR}/UseBoost.cmake)

####################################
# Specify preprocessor definitions #
####################################

IF(MSVC_IDE)
	# Prevent the deprecation warnings that VC++ arbitrarily decides to produce for C Standard Library functions
	#ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC_IDE)

##########################################
# Specify the target and where to put it #
##########################################

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${whery_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${whery_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${whery_BINARY_DIR}/lib)
ADD_LIBRARY(${targetname} STATIC ${sources} ${headers} ${templates})

SET_TARGET_PROPERTIES(${targetname} PROPERTIES DEBUG_OUTPUT_NAME "${targetname}_d")