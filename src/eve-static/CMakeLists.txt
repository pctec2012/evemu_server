#
# CMake build system file for EVEmu.
#
# Author: Bloody.Rabbit
#

##############
# Initialize #
##############
SET( TARGET_NAME        "eve-static" )
SET( TARGET_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/src/${TARGET_NAME}" )
SET( TARGET_SOURCE_DIR  "${PROJECT_SOURCE_DIR}/src/${TARGET_NAME}" )

#########
# Files #
#########
SET( INCLUDE
     "${TARGET_INCLUDE_DIR}/eveStatic.h"
 )
SET( SOURCE
     "${TARGET_SOURCE_DIR}/eveStatic.cpp"
 )

SET( chr_INCLUDE
     "${TARGET_INCLUDE_DIR}/chr/ChrBloodline.h"
)
SET( chr_SOURCE
     "${TARGET_SOURCE_DIR}/chr/ChrBloodline.cpp"
)

SET( inv_INCLUDE
     "${TARGET_INCLUDE_DIR}/inv/InvType.h"
     "${TARGET_INCLUDE_DIR}/inv/InvGroup.h"
     "${TARGET_INCLUDE_DIR}/inv/InvCategory.h"
     "${TARGET_INCLUDE_DIR}/inv/InvBlueprintType.h"
)
SET( inv_SOURCE
     "${TARGET_SOURCE_DIR}/inv/InvType.cpp"
     "${TARGET_SOURCE_DIR}/inv/InvGroup.cpp"
     "${TARGET_SOURCE_DIR}/inv/InvCategory.cpp"
     "${TARGET_SOURCE_DIR}/inv/InvBlueprintType.cpp"
)

SET( sta_INCLUDE
     "${TARGET_INCLUDE_DIR}/blk/BlkShipType.h"
     "${TARGET_INCLUDE_DIR}/sta/StaStationType.h"
)
SET( sta_SOURCE
     "${TARGET_SOURCE_DIR}/blk/BlkShipType.cpp"
     "${TARGET_SOURCE_DIR}/sta/StaStationType.cpp"
)

SET( blk_INCLUDE
     "${TARGET_INCLUDE_DIR}/blk/BlkShipType.h"
)
SET( blk_SOURCE
     "${TARGET_SOURCE_DIR}/blk/BlkShipType.cpp"
)

########################
# Setup the executable #
########################
SOURCE_GROUP( "src"                FILES ${SOURCE} )
SOURCE_GROUP( "src\\inv"           FILES ${inv_SOURCE} )
SOURCE_GROUP( "src\\sta"           FILES ${sta_SOURCE} )
SOURCE_GROUP( "src\\blk"           FILES ${blk_SOURCE} )
SOURCE_GROUP( "src\\chr"           FILES ${chr_SOURCE} )

ADD_LIBRARY( "${TARGET_NAME}"
${SOURCE}
${inv_SOURCE}
${sta_SOURCE}
${blk_SOURCE}
${chr_SOURCE}
)

TARGET_INCLUDE_DIRECTORIES( "${TARGET_NAME}"
                            ${eve-common_INCLUDE_DIRS}
                            ${eve-core_INCLUDE_DIRS}
                            "${TARGET_INCLUDE_DIR}" )
TARGET_LINK_LIBRARIES( "${TARGET_NAME}"
                       ${UTILS_LIBRARIES}
                       "eve-core" )
TARGET_BUILD_PCH( "${TARGET_NAME}"
                  "${TARGET_INCLUDE_DIR}/eveStatic.h"
                  "${TARGET_SOURCE_DIR}/eveStatic.cpp" )
