#*******************************************************************************
#  Copyright (c) 2015, 2017 logi.cals GmbH and others
#
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the Eclipse Public License v1.0
#  and Eclipse Distribution License v1.0 which accompany this distribution.
#
#  The Eclipse Public License is available at
#     http://www.eclipse.org/legal/epl-v10.html
#  and the Eclipse Distribution License is available at
#    http://www.eclipse.org/org/documents/edl-v10.php.
#
#  Contributors:
#     Rainer Poisel - initial version
#     Genis Riera Perez - Add support for building debian package
#*******************************************************************************/
#         多路 LoRA 无线通讯网关
# 1). 支持MQTT协议和私有的socket协议,
# 2). LoRa 数据传输采用DES加密算法,
# 3). 业务数据本地存储和自动上报.
# 4). 硬件平台采用IMX6ul 平台.
# 5). qt-prj 文件夹是ubuntu环境调试工程.
# 6). socket/CMakeList.txt 是源码结构文件
# Note: on OS X you should install XCode and the associated command-line tools

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("paho" C)

MESSAGE(STATUS "CMake version: " ${CMAKE_VERSION})
MESSAGE(STATUS "CMake system name: " ${CMAKE_SYSTEM_NAME})

SET(CMAKE_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake")
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS "-D_REENTRANT ${CMAKE_C_FLAGS}")
# reentrant
set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS "-Wcpp ${CMAKE_CXX_FLAGS}")

MESSAGE(STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS})
MESSAGE(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})

SET(CMAKE_SYSTEM_NAME Linux)

SET(CMAKE_C_COMPILER "arm-fsl-linux-gnueabi-gcc")
SET(CMAKE_CXX_COMPILER "arm-fsl-linux-gnueabi-g++")
SET(CMAKE_FIND_ROOT_PATH "/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin")

SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
MESSAGE(STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER})
MESSAGE(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
MESSAGE(STATUS "CMAKE_FIND_ROOT_PATH: " ${CMAKE_FIND_ROOT_PATH})

## build settings
SET(PAHO_VERSION_MAJOR 1)
SET(PAHO_VERSION_MINOR 2)
SET(PAHO_VERSION_PATCH 0)
SET(CLIENT_VERSION ${PAHO_VERSION_MAJOR}.${PAHO_VERSION_MINOR}.${PAHO_VERSION_PATCH})

STRING(TIMESTAMP BUILD_TIMESTAMP UTC)
MESSAGE(STATUS "Timestamp is ${BUILD_TIMESTAMP}")

# Uses the correct directory for libraries on Red Hat-based distributions.
SET(PAHO_LIBRARY_DIR_NAME "lib")
IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  IF(EXISTS "/etc/redhat-release")
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    	SET(PAHO_LIBRARY_DIR_NAME "lib64")
    ENDIF()
  ENDIF()
ENDIF()

# Uses the correct directory for libraries on Red Hat-based distributions.
SET(PAHO_LIBRARY_DIR_NAME "lib")
IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  IF(EXISTS "/etc/redhat-release")
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    	SET(PAHO_LIBRARY_DIR_NAME "lib64")
    ENDIF()
  ENDIF()
ENDIF()

IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -DWIN32_LEAN_AND_MEAN -MD)
ELSEIF(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  ADD_DEFINITIONS(-DOSX)
ENDIF()

## build options
SET(PAHO_WITH_SSL FALSE CACHE BOOL "Flag that defines whether to build ssl-enabled binaries too. ")
SET(PAHO_BUILD_STATIC FALSE CACHE BOOL "Build static library")
SET(PAHO_BUILD_DOCUMENTATION FALSE CACHE BOOL "Create and install the HTML based API documentation (requires Doxygen)")
SET(PAHO_BUILD_SAMPLES FALSE CACHE BOOL "Build sample programs")
SET(PAHO_BUILD_DEB_PACKAGE FALSE CACHE BOOL "Build debian package")


#ADD_SUBDIRECTORY(src)
IF(PAHO_BUILD_SAMPLES)
    ADD_SUBDIRECTORY(src/samples)
ENDIF()

IF(PAHO_BUILD_DOCUMENTATION)
    ADD_SUBDIRECTORY(doc)
ENDIF()

### packaging settings
IF (WIN32)
    SET(CPACK_GENERATOR "ZIP")
ELSEIF(PAHO_BUILD_DEB_PACKAGE)
    SET(CPACK_GENERATOR "DEB")
    CONFIGURE_FILE(${CMAKE_SCRIPTS}/CPackDebConfig.cmake.in
        ${CMAKE_BINARY_DIR}/CPackDebConfig.cmake @ONLY)
    SET(CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/CPackDebConfig.cmake)
    ADD_SUBDIRECTORY(debian)
ELSE()
    SET(CPACK_GENERATOR "TGZ")
ENDIF()

SET(CPACK_PACKAGE_VERSION_MAJOR ${PAHO_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PAHO_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PAHO_VERSION_PATCH})
INCLUDE(CPack)

#ENABLE_TESTING()
#ADD_SUBDIRECTORY(test)

##################  INCLUDE_DIRECTORIES(src)

SET (INCLUDE_DIRECTORIES 
          ${PROJECT_SOURCE_DIR}/src 
          ${PROJECT_SOURCE_DIR}/uart 
          ${PROJECT_SOURCE_DIR}/crc 
          ${PROJECT_SOURCE_DIR}/des 
          ${PROJECT_SOURCE_DIR}/socket)
          
#####  add source file
#ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(uart)
ADD_SUBDIRECTORY(crc)
ADD_SUBDIRECTORY(des)
ADD_SUBDIRECTORY(socket)
ADD_SUBDIRECTORY(bdb)
ADD_SUBDIRECTORY(util)

 