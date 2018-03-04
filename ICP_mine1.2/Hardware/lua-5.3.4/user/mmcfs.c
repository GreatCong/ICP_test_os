/**
 *************************************************************************************
 * @file 
 * @author liucongjun
 * @version
 * @date 2017.11.23
 * @brief test "fopen" "fread" "fwrite"
 *************************************************************************************
*/

#include <string.h>
#include <stdio.h>
#include "ff.h"
#include "mmcfs.h"
#include <rt_sys.h>
#define MMCFS_MAX_FDS   4
static FIL mmcfs_fd_table[ MMCFS_MAX_FDS ];
static int mmcfs_num_fd;
static FATFS mmc_fs;
static FIL mmc_fileObject;
#define PATH_BUF_SIZE   40
static char mmc_pathbuf[PATH_BUF_SIZE];
int mmcfs_find_empty_fd( void )
{
  int i;

  for (i = 0; i < MMCFS_MAX_FDS; i ++)
    if (mmcfs_fd_table[i].fs == NULL)
      return i;
  return -1;
}

int mmc_open(const char * filename, int mode)
{

 
  int fd;
  int mmc_mode;
	
  if (mmcfs_num_fd == MMCFS_MAX_FDS)
  {
    return -1;
  }
  mmc_pathbuf[0] = 0;
  if (strchr(filename, '/') == NULL)
    strcat(mmc_pathbuf, "/");
  strcat(mmc_pathbuf, filename);
	switch(mode)
	{
		case OPEN_R://r
			mmc_mode=FA_OPEN_EXISTING|FA_READ;
		  break;		
		case OPEN_R|OPEN_PLUS://r+
			mmc_mode=FA_OPEN_EXISTING|FA_READ|FA_WRITE;
		  break;	    
		case OPEN_R|OPEN_B|OPEN_PLUS://rb+
			mmc_mode=FA_OPEN_ALWAYS|FA_READ;
		  break;	    
		case OPEN_W://w
			mmc_mode=FA_CREATE_ALWAYS|FA_WRITE;	
		  break;		
		case OPEN_W|OPEN_B://wb
			mmc_mode=FA_OPEN_ALWAYS|FA_WRITE;
		  break;	    
		case OPEN_W|OPEN_PLUS://w+
			mmc_mode=FA_CREATE_ALWAYS|FA_READ|FA_WRITE;
		  break;	    
		case OPEN_W|OPEN_B|OPEN_PLUS://wb+
			mmc_mode=FA_OPEN_ALWAYS|FA_READ|FA_WRITE;
		  break;		
		case OPEN_A://a
			mmc_mode=FA_OPEN_ALWAYS|FA_WRITE;
		  break;		
		case OPEN_A|OPEN_PLUS://a+
			mmc_mode=FA_OPEN_ALWAYS|FA_READ|FA_WRITE;
		  break;		 
		case OPEN_A|OPEN_B|OPEN_PLUS://ab+
			mmc_mode=FA_OPEN_ALWAYS|FA_READ|FA_WRITE;
		  break;		 
		case 18: //at+
			mmc_mode=FA_OPEN_ALWAYS|FA_READ|FA_WRITE;
		  break;		 
	}

 
  // Open the file for reading
  if (f_open(&mmc_fileObject, mmc_pathbuf, mmc_mode) != FR_OK)
  {
    return -1;
  }
	
  if (mode & OPEN_A){
    //mmc_fileObject.fptr = mmc_fileObject.fsize;
		f_lseek(&mmc_fileObject,mmc_fileObject.fsize);
	}
	
  fd = mmcfs_find_empty_fd();
	if (fd<0){
	   printf(" find_empty_fd error\r\n");
	}
//	else{
//	  printf(" fd=%d\r\n",fd);
//	}
  memcpy(mmcfs_fd_table + fd, &mmc_fileObject, sizeof(FIL));
  mmcfs_num_fd ++;
  return fd; 	 
}
int mmc_close(int handle)
{
	FRESULT res;
  FIL* pfile = mmcfs_fd_table + handle;
  res=f_close( pfile );
	if(res==FR_OK)
	{
	
		memset(pfile, 0, sizeof(FIL));
		mmcfs_num_fd --;
		}
  return 0;
}
size_t mmc_write(int handle, const void * buffer, size_t size)
{
  UINT bytes_written;
  if (f_write(mmcfs_fd_table + handle,  buffer, (UINT)size, &bytes_written) != FR_OK)
  {
    return 0;
  }
  return (size_t) bytes_written;
}
size_t mmc_read(int handle, void * buffer, size_t size)
{
  UINT bytes_read;
	
  if (f_read(mmcfs_fd_table + handle, buffer, (UINT)size, &bytes_read) != FR_OK)
  {
    return 0;
  } 
//	xprintf("mmc_read=%s\r\n",buffer);
  return (size_t) bytes_read;
}
long mmc_lseek(int handle, long offset, int whence)
{
  
  FIL* pfile = mmcfs_fd_table + handle;
  long newpos = 0;

  switch( whence )
  {
    case SEEK_SET:
      // seek from beginning of file
      newpos = offset;
      break;

    case SEEK_CUR:
      // seek from current position
      newpos = pfile->fptr + offset;
      break;

    case SEEK_END:
      // seek from end of file
      newpos = pfile->fsize + offset;
      break;

    default:
      return -1;
  }
  if (f_lseek (pfile, newpos) != FR_OK)
    return -1;
  return newpos;
}
long mmc_file_len(int handle)
{
	FIL* pfile = mmcfs_fd_table + handle;
	return pfile->fsize;
}
int mmcfs_init(void)
{
  // Mount the MMC file system using logical disk 0
  if ( f_mount(&mmc_fs, "", 0) != FR_OK )
    return -1;
  else 
    return 0;
}

int mmcfs_feof(int handle){
  if (f_eof(mmcfs_fd_table + handle))//end
		return 1;
	else
		return 0;
}
