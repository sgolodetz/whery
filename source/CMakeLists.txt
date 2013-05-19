######################################
# Top-level CMakeLists.txt for whery #
######################################

cmake_minimum_required(VERSION 2.8)

PROJECT(whery)

ADD_SUBDIRECTORY(apps)
ADD_SUBDIRECTORY(engine)

OPTION(BUILD_TESTS "Build the tests?" ON)
IF(BUILD_TESTS)
	ADD_SUBDIRECTORY(tests)
ENDIF(BUILD_TESTS)