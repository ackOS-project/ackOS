#pragma once

enum ramdisk_type
{
    RAMDISK_TYPE_TAR,
    RAMDISK_TYPE_ZIP
};

void ramdisk_mount(const char* mount_point, void* addr, ramdisk_type type);
