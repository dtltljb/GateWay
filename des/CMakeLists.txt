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
#     Ian Craggs - update sample names
#*******************************************************************************/

# Note: on OS X you should install XCode and the associated command-line tools

##	src/crc config

## 	compilation/linkage settings

INCLUDE_DIRECTORIES(
    .
    ${CMAKE_SOURCE_DIR}/des
    ${CMAKE_BINARY_DIR}
    )

IF (WIN32)
  	ADD_DEFINITIONS(/DCMAKE_BUILD /D_CRT_SECURE_NO_DEPRECATE)
ENDIF()

# sample files c
SET(DIR_SRCS 	des_test.c	d3des.c )
#ADD_SUBDIRECTORY(./src/des)
#AUX_SOURCE_DIRECTORY(. DIR_SRCS)

#ADD_EXECUTABLE(destest des_test.c d3des.c)

#TARGET_LINK_LIBRARIES(VirtualPortMonitor pthread)
#TARGET_LINK_LIBRARIES(destest -lm)


