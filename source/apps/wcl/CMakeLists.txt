###############################
# CMakeLists.txt for apps/wcl #
###############################

###########################
# Specify the target name #
###########################

SET(targetname wcl)

#############################
# Specify the project files #
#############################

##
SET(wcl_sources main.cpp)

#################################################################
# Collect the project files into sources, headers and templates #
#################################################################

SET(sources
${wcl_sources}
)

SET(headers
)

SET(templates
)

#############################
# Specify the source groups #
#############################

##
SOURCE_GROUP(.cpp FILES ${wcl_sources})

###################################
# Specify the include directories #
###################################

INCLUDE_DIRECTORIES(${whery_SOURCE_DIR}/engine/include)

################################
# Specify the libraries to use #
################################

INCLUDE(${whery_SOURCE_DIR}/UseBoost.cmake)

##########################################
# Specify the target and where to put it #
##########################################

INCLUDE(${whery_SOURCE_DIR}/SetAppTarget.cmake)

#################################
# Specify the libraries to link #
#################################

TARGET_LINK_LIBRARIES(${targetname} whery)
INCLUDE(${whery_SOURCE_DIR}/LinkBoost.cmake)

#############################
# Specify things to install #
#############################

INCLUDE(${whery_SOURCE_DIR}/InstallApp.cmake)