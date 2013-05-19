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
SET(db_sources
src/db/DataPage.cpp
src/db/DoubleFieldManipulator.cpp
src/db/Field.cpp
src/db/FieldManipulator.cpp
src/db/FieldTupleComparator.cpp
src/db/FreshRecord.cpp
src/db/IntFieldManipulator.cpp
src/db/Record.cpp
src/db/RecordManipulator.cpp
src/db/RecordProjection.cpp
)

SET(db_headers
include/whery/db/DataPage.h
include/whery/db/DoubleFieldManipulator.h
include/whery/db/Field.h
include/whery/db/FieldManipulator.h
include/whery/db/FieldTuple.h
include/whery/db/FieldTupleComparator.h
include/whery/db/FreshRecord.h
include/whery/db/IntFieldManipulator.h
include/whery/db/Record.h
include/whery/db/RecordManipulator.h
include/whery/db/RecordProjection.h
)

##
SET(util_sources
src/util/AlignmentTracker.cpp
)

SET(util_headers
include/whery/util/AlignmentTracker.h
include/whery/util/SortDirection.h
)

#################################################################
# Collect the project files into sources, headers and templates #
#################################################################

SET(sources
${db_sources}
${util_sources}
)

SET(headers
${db_headers}
${util_headers}
)

SET(templates
)

#############################
# Specify the source groups #
#############################

##
SOURCE_GROUP(db\\.cpp FILES ${db_sources})
SOURCE_GROUP(db\\.h FILES ${db_headers})

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