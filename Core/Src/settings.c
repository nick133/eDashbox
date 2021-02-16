
#include "fatfs.h"
#include "settings.h"

ConfigSettingsT Config;


void GetSettings(void)
{
    FATFS fs;
    // FATFS *pfs;
    // FIL fil;
    char buffer[100];

    if(f_mount(&fs, "", 1) != FR_OK)
    {
    
    }

    if(f_mount(NULL, "", 1) != FR_OK)
    {
    
    }
}