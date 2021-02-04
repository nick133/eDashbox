

/*******************************************************************************
 * FatFs
 ******************************************************************************/
  if(f_mount(&fs, "", 1) != FR_OK)
  {
    ErrorError = 1;
  }
    
       /* Open file to write */
if(f_open(&fil, "Hello_Mootherfucker-static.v11.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
{
    ErrorError = 2;
}

        /* Check freeSpace space */
if(f_getfree("", &fre_clust, &pfs) != FR_OK)
{
    ErrorError = 3;
}

        totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
        freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);

        /* free space is less than 1kb */
        if(freeSpace < 1)
  {
    ErrorError = 4;
  }

        /* Writing text */
        f_puts("Save the world!!! Some test text - 0000000000000 veeery long!", &fil);

        /* Close file */
        if(f_close(&fil) != FR_OK)
     {
    ErrorError = 5;
  }

  f_open(&fil, "Another-test-file-very-loong-fn.v12.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
  f_puts("Version 2. Save the world!!! Some test text - 0000000000000 veeery long!", &fil);
  f_close(&fil);

        /* Open file to read */
        if(f_open(&fil, "Hello_Mootherfucker-static.v11.txt", FA_READ) != FR_OK)
     {
    ErrorError = 6;
  }

  f_gets(buffer, sizeof(buffer), &fil);
  //      while(f_gets(buffer, sizeof(buffer), &fil))
        // {
        //         /* SWV output */
        //         sprintf("%s", buffer);
        //         fflush(stdout);
        // }

        /* Close file */
        if(f_close(&fil) != FR_OK)
     {
ErrorError = 7;
  }

        /* Unmount SDCARD */
        if(f_mount(NULL, "", 1) != FR_OK)
        {
ErrorError = 8;
  }
