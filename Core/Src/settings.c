
#include "fatfs.h"
#include "settings.h"


void GetSettings(void)
{
    FATFS fs;
    FATFS *pfs;
    FIL fil;
    FRESULT fres;
    DWORD fre_clust;
    uint32_t totalSpace, freeSpace;
    char buffer[100];

    if(f_mount(&fs, "", 1) != FR_OK)
    {
    
    }

    if(f_mount(NULL, "", 1) != FR_OK)
    {
    
    }
}

ConfigSettingsT config;