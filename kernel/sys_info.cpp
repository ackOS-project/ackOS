#include "kernel/sys_info.h"

#define ACKOS_NAME_STRING "ackOS"
#define ACKOS_VERSION_STRING "0.02"
#define ACKOS_RELEASE_STRING "v0.02-dev"
#define ACKOS_COPYRIGHT_STRING "Copyright (c) 2020 - 2021 - The ackOS project."

ackos::system_info get_system_info()
{
    ackos::system_info sys_info;

    sys_info.os_name = ACKOS_NAME_STRING;
    sys_info.os_version = ACKOS_VERSION_STRING;
    sys_info.os_release = ACKOS_RELEASE_STRING;
    sys_info.build_date = BUILD_TIME;
    sys_info.copyright = ACKOS_COPYRIGHT_STRING;

    return sys_info;
}
