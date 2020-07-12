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

#SET(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/uart)
#set(CMAKE_LIBRARY_PATH "lib_path")
#MESSAGE(STATUS "CMAKE_INCLUDE_PATH:  ${CMAKE_INCLUDE_PATH}")

INCLUDE_DIRECTORIES(
    .
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    )
    
#MESSAGE(STATUS "socket INCLUDE_DIRECTORIES:  ${INCLUDE_DIRECTORIES}")

IF (WIN32)
  	ADD_DEFINITIONS(/DCMAKE_BUILD /D_CRT_SECURE_NO_DEPRECATE)
ENDIF()


SET(SOCKET_PROTO_SRC
	VirtualPortMonitor.c 
	socket_protocol_thread.c
	Unpacket.c
	Packet.c
	pub/epb.c
	pub/util.c
	pub/util_date.c
	pub/epb_MmBp.c
	AppLayer/keep_alive_handle.c
	AppLayer/AysnchronousWork.c
	protoBuf/epb_keep_alive_pack.c
	protoBuf/epb_emergency_open_pack.c
	protoBuf/epb_report_log_pack.c
	protoBuf/epb_remote_open_pack.c
	protoBuf/epb_remote_load_pack.c
	protoBuf/epb_remote_config_pack.c
	protoBuf/epb_remote_get_config_pack.c
	protoBuf/epb_remote_authorize_pack.c
	protoBuf/epb_set_lora_param_pack.c
	../crc/crc32.c
	../crc/crc16.c
	../des/d3des.c
	../util/code_value_change.c
	../util/pthread_data_copy.c
	../uart/UartPacket.c
	../uart/UartUnpacket.c
	../uart/socket_task_distributer.c
	../uart/xxx_channel_send_thread.c
	../uart/WorkCommandAnalysis.c
	../uart/ConfigCommandAnalysis.c
	../uart/Cmd/report_log_pack.c
	../uart/Cmd/remote_open_pack.c							  
	../uart/Cmd/remote_load_pack.c
	../uart/Cmd/remote_config_pack.c
	../uart/Cmd/remote_authorize_pack.c
	../uart/Cmd/emergency_open_pack.c
	../uart/Cmd/u_set_lora_para_pack.c
	../uart/Cmd/remote_change_channel_pack.c
	../uart/Cmd/register_log_pack.c
	../uart/Cmd/report_history_log_pack.c
	../uart/LinkLayer/UartOpration.c
	../uart/LinkLayer/lora_parament.c
	../uart/LinkLayer/uart_listener_thread.c
	../uart/LinkLayer/first_listener_thread.c
	../uart/LinkLayer/second_listener_thread.c
	../uart/LinkLayer/third_listener_thread.c
	../uart/LinkLayer/fourth_listener_thread.c							  
	../bdb/FileManager.c
	../bdb/logManager.c
	../bdb/forlder_util.c
	../bdb/link_table.c
    )
    
MESSAGE(STATUS "SRC: " ${SOCKET_PROTO_SRC})

ADD_EXECUTABLE(GW-v0.95 ${SOCKET_PROTO_SRC})

#../src/samples/MQTTClient_subscribe.c
#TARGET_LINK_LIBRARIES(MQTTAsync_subscribe paho-mqtt3a)
#TARGET_LINK_LIBRARIES(MQTTAsync_publish paho-mqtt3a)
#TARGET_LINK_LIBRARIES(MQTTClient_subscribe paho-mqtt3c)
#TARGET_LINK_LIBRARIES(MQTTClient_publish paho-mqtt3c)
#TARGET_LINK_LIBRARIES(MQTTClient_publish_async paho-mqtt3c)

TARGET_LINK_LIBRARIES(GW-v0.95 pthread)


