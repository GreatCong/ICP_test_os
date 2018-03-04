#ifndef __MMCFS_H
#define __MMCFS_H
#ifndef NULL
  #define NULL  _NULL
#endif /* NULL */
#include <stdio.h>
#include "ff.h"
int mmcfs_find_empty_fd( void );
int mmc_open(const char * filename, int mode);
int mmc_close(int handle);
size_t mmc_write(int handle, const void * buffer, size_t size);
size_t mmc_read(int handle, void * buffer, size_t size);
long mmc_lseek(int handle, long offset, int whence);
int mmcfs_init(void);
long mmc_file_len(int handle);
int mmcfs_feof(int handle);
#endif
