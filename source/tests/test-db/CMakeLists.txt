####################################
# CMakeLists.txt for tests/test-db #
####################################

############################
# Specify the project name #
############################

SET(targetname test-db)

#############################
# Specify the project files #
#############################

SET(sources
BTreeTest.cpp
FieldManipulatorTest.cpp
FieldTest.cpp
FreshTupleTest.cpp
IDAllocatorTest.cpp
InMemorySortedPageTest.cpp
PrefixTupleComparatorTest.cpp
ProjectedTupleTest.cpp
TestRunner.cpp
TupleManipulatorTest.cpp
)

SET(headers
Constants.h
)

#############################
# Specify the source groups #
#############################

SOURCE_GROUP(.cpp FILES ${sources})
SOURCE_GROUP(.h FILES ${headers})

###################################
# Specify the include directories #
###################################

INCLUDE_DIRECTORIES(${whery_SOURCE_DIR}/engine/include)

################################
# Specify the libraries to use #
################################

INCLUDE(${whery_SOURCE_DIR}/UseBoost.cmake)

#####################################
# Specify additional compiler flags #
#####################################

INCLUDE(${whery_SOURCE_DIR}/BoostTestCompilerFlags.cmake)

####################################
# Specify preprocessor definitions #
####################################

IF(MSVC_IDE)
	# Prevent the deprecation warnings that VC++ arbitrarily decides to produce for C Standard Library functions
	ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS (/D _SCL_SECURE_NO_WARNINGS)
ENDIF(MSVC_IDE)

##########################################
# Specify the target and where to put it #
##########################################

INCLUDE(${whery_SOURCE_DIR}/SetTestTarget.cmake)

#################################
# Specify the libraries to link #
#################################

TARGET_LINK_LIBRARIES(${targetname} whery)
INCLUDE(${whery_SOURCE_DIR}/LinkBoost.cmake)

###############################
# Specify the post-build step #
###############################

INCLUDE(${whery_SOURCE_DIR}/BoostTestPostBuild.cmake)

#############################
# Specify things to install #
#############################

INCLUDE(${whery_SOURCE_DIR}/InstallTest.cmake)