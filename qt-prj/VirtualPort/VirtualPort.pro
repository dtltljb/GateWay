#QT += core
#QT -= gui

TARGET = VirtualPort
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread
CONFIG += -static



INCLUDEPATH +=\
            ../\
            ../../\

HEADERS += \
    build_configure.h


SOURCES += \
    ../../crc/crc16.c \
    ../../crc/crc32.c \
    ../../des/d3des.c \
    ../../uart/UartPacket.c \
    ../../uart/UartUnpacket.c \
    ../../uart/Cmd/emergency_open_pack.c \
    ../../uart/Cmd/remote_authorize_pack.c \
    ../../uart/Cmd/remote_config_pack.c \
    ../../uart/Cmd/remote_load_pack.c \
    ../../uart/Cmd/remote_open_pack.c \
    ../../uart/Cmd/report_log_pack.c \
    ../../uart/LinkLayer/UartOpration.c \
    ../../socket/Packet.c \
    ../../socket/VirtualPortMonitor.c\
    ../../socket/Unpacket.c \
    ../../socket/pub/epb_MmBp.c \
    ../../socket/pub/epb.c \
    ../../socket/pub/util_date.c \
    ../../socket/pub/util.c \
    ../../socket/protoBuf/epb_emergency_open_pack.c \
    ../../socket/protoBuf/epb_keep_alive_pack.c \
    ../../socket/protoBuf/epb_remote_authorize_pack.c \
    ../../socket/protoBuf/epb_remote_config_pack.c \
    ../../socket/protoBuf/epb_remote_load_pack.c \
    ../../socket/protoBuf/epb_remote_open_pack.c \
    ../../socket/protoBuf/epb_report_log_pack.c \
    ../../socket/AppLayer/keep_alive_handle.c \
    ../../socket/AppLayer/AysnchronousWork.c \
    ../../uart/WorkCommandAnalysis.c \
    ../../bdb/FileManager.c \
    ../../bdb/logManager.c \
    ../../bdb/forlder_util.c \
    ../../util/code_value_change.c \
    ../../bdb/local_ip_port_init.c \
    ../../uart/Cmd/u_set_lora_para_pack.c \
    ../../socket/protoBuf/epb_set_lora_param_pack.c \
    ../../socket/protoBuf/epb_remote_get_config_pack.c \
    ../../util/pthread_data_copy.c \
    ../../uart/socket_task_distributer.c \
    ../../uart/xxx_channel_send_thread.c \
    ../../uart/LinkLayer/lora_parament.c \
    ../../uart/LinkLayer/uart_listener_thread.c \
    ../../socket/socket_protocol_thread.c \
    ../../uart/ConfigCommandAnalysis.c \
    ../../uart/Cmd/register_log_pack.c \
    ../../uart/Cmd/remote_change_channel_pack.c \
    ../../uart/LinkLayer/first_listener_thread.c \
    ../../uart/LinkLayer/fourth_listener_thread.c \
    ../../uart/LinkLayer/third_listener_thread.c \
    ../../uart/LinkLayer/second_listener_thread.c \
    ../../bdb/link_table.c \
    ../../uart/Cmd/report_history_log_pack.c






