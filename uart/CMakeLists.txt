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
#     Ian Craggs (IBM Corp.) - merge master
#*******************************************************************************/

# Note: on OS X you should install XCode and the associated command-line tools

## compilation/linkage settings

INCLUDE_DIRECTORIES(
    .
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )


SET(UART_PROTOCOL_SRC

    )
   
MESSAGE(STATUS "PROTOCOL_SRC:  ${UART_PROTOCOL_SRC}")


IF (WIN32)
    SET(LIBS_SYSTEM ws2_32)
ELSEIF (UNIX)
    IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
        SET(LIBS_SYSTEM c dl pthread)
    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        SET(LIBS_SYSTEM compat pthread)
    ELSE()
        SET(LIBS_SYSTEM c pthread)
    ENDIF()
ENDIF()


## common compilation 
#ADD_LIBRARY(common_uart OBJECT ${UART_PROTOCOL_SRC})
#SET_PROPERTY(TARGET common_uart PROPERTY POSITION_INDEPENDENT_CODE ON)



