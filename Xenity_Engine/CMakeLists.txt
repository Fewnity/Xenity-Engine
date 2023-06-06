cmake_minimum_required(VERSION 3.0)
SET(MODE "mode" CACHE STRING "Some user-specified option")

set(APP_NAME "Xenity Engine")

if(MODE STREQUAL "psvita")
message(STATUS "BUILD PSVITA")

## This includes the Vita toolchain, must go before project definition
# It is a convenience so you do not have to type
# -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is
# highly recommended that you include this block for all projects.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

endif()

project(hello)


# INCLUDE_DIRECTORIES(../include/glm)
# LINK_DIRECTORIES(../include/glm)
# TARGET_LINK_LIBRARIES(glm)

# Add any additional include paths here
include_directories(
    include/
)

# Spécifiez le répertoire contenant vos fichiers source
set(SOURCES_DIR "Source")

# Recherchez tous les fichiers .cpp dans le répertoire et ses sous-répertoires
file(GLOB_RECURSE SOURCES "${SOURCES_DIR}/*.cpp" "${SOURCES_DIR}/*.c")

# Ajoutez les fichiers source à l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES})


if(MODE STREQUAL "psp")
message(STATUS "BUILD PSP")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fexceptions")

include_directories(${PSPDEV}/psp/include/freetype2)

target_link_libraries(${PROJECT_NAME} PRIVATE
    png
    jpeg
    pspgu
    pspgum
    pspctrl
    pspdebug
    pspdisplay
    m
    pspge
    stdc++
    freetype
    bz2
    z
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${APP_NAME}
)
elseif(MODE STREQUAL "psvita")
message(STATUS "BUILD PSVITA")

# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)
include_directories(${VITASDK}/arm-vita-eabi/include/freetype2)

## Configuration options for this app
# Display name (under bubble in LiveArea)
#set(VITA_APP_NAME "Xenity Engine")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "FEWN00006")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "01.00")

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

include_directories(${VITASDK}/arm-vita-eabi/include)

# Library to link to (drop the -l prefix). This will mostly be stubs.
target_link_libraries(${PROJECT_NAME}
freetype
png
  z
  vitaGL 
  vita2d 
  SceLibKernel_stub 
  SceAppMgr_stub 
  SceAppUtil_stub 
  mathneon
  c 
  SceCommonDialog_stub 
  m 
  SceGxm_stub 
  SceDisplay_stub 
  ScePgf_stub 
  SceSysmodule_stub
  SceUsbSerial_stub
  SceUsbd_stub 
  zip 
  vitashark 
  SceShaccCgExt 
  taihen_stub SceShaccCg_stub
  SceKernelDmacMgr_stub 
  SceCtrl_stub 
  stdc++
  pthread
)

## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${APP_NAME}
  #FILE sce_sys/icon0.png sce_sys/icon0.png
  #FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  #FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  #FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)
else()
message(STATUS "BUILD WINDOWS")
endif()

