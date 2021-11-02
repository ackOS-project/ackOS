#include "kernel/boot_protocols/modules.h"
#include "kernel/fs/ramdisk.h"
#include "kernel/sys/panic.h"

#include <cstring>
#include <string>

void boot_modules_intialise(uniheader* uheader)
{
    for(int i = 0; i < uheader->module_count; i++)
    {
        uniheader_module& mod = uheader->modules[i];
        
        if(strcmp(mod.command_line_args, "tar_ramdisk") == 0)
        {
            ramdisk_mount("/", mod.start, RAMDISK_TYPE_TAR);
        }
        else if(strcmp(mod.command_line_args, "zip_ramdisk") == 0)
        {
            kpanic("ZIP ramdisks aren't supported yet :(");
            //ramdisk_mount("/", mod.start, RAMDISK_TYPE_ZIP);
        }
        else
        {
            kpanic((std::string("unknown boot module detected: ") + mod.command_line_args).c_str());
        }
    }
}
