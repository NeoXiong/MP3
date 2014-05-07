/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: mfs_init.c$
* $Version : 3.8.34.0$
* $Date    : Jun-27-2012$
*
* Comments:
*
*   This file contains the functions that are used to initialize MFS
*   It also contains the MFS driver functions.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <fio.h>
//#include <io_prv.h>


#include "io_prv.h"
#include "mfs.h"
#include "mfs_prv.h"
#include "part_mgr.h"

extern _mem_pool_id _MFS_pool_id;
extern uint_32 _MFS_handle_pool_init;
extern uint_32 _MFS_handle_pool_grow;
extern uint_32 _MFS_handle_pool_max;


extern void MFS_free_drive_data( MFS_DRIVE_STRUCT_PTR drive_ptr, boolean all);
/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MFS_Open_Device
* Returned Value   : MQX_OK or an error
* Comments         : Opens and initializes MFS driver.
* 
*END*----------------------------------------------------------------------*/

int_32 MFS_Open_Device
    (
    MQX_FILE_PTR             fd_ptr,        /* [IN] the MFS file handle for the device being opened */
    MFS_DRIVE_STRUCT_PTR     drive_ptr
    )
{
    MQX_FILE_PTR dev_fd;
    uint_32     sector_size, k;
    int_32      error_code;

    dev_fd = drive_ptr->DEV_FILE_PTR;

    fd_ptr->DEV_DATA_PTR = NULL;
    drive_ptr->MFS_FILE_PTR = fd_ptr;

    /* Select partition, if desired */
    if (drive_ptr->DRV_NUM)
    {
        error_code = ioctl(dev_fd, IO_IOCTL_SEL_PART, &drive_ptr->DRV_NUM);
        if (error_code)
        {
            return error_code;
        }
    }

    /*
    ** obtain the buffer for configuration data and for storing general
    ** sector reads
    */
    error_code = _mfs_validate_device(dev_fd, &sector_size, &drive_ptr->BLOCK_MODE);

    if ( error_code )
    {
        /* Device isn't valid */
        drive_ptr->MFS_FILE_PTR = NULL;      
        return error_code;
    }

    _lwsem_wait(&drive_ptr->SEM);

    drive_ptr->BPB.SECTOR_SIZE = (uint_16) sector_size; 
    drive_ptr->DIR_SECTOR_PTR = MFS_mem_alloc_system_zero(sector_size);
    MFS_Invalidate_directory_sector(drive_ptr);

    if ( drive_ptr->DIR_SECTOR_PTR == NULL )
    {
        _lwsem_post(&drive_ptr->SEM);
        drive_ptr->MFS_FILE_PTR = NULL;      
        return MFS_INSUFFICIENT_MEMORY;
    }

    _mem_set_type(drive_ptr->DIR_SECTOR_PTR, MEM_TYPE_MFS_DIRECTORY_SECTOR); 
    _queue_init(&drive_ptr->HANDLE_LIST, 0);

    k = drive_ptr->BPB.SECTOR_SIZE;
    for ( drive_ptr->SECTOR_POWER = 0; !(k & 1);
        drive_ptr->SECTOR_POWER++ )
    {
        k>>=1;
    } 

    /*
    ** read boot sector and get the BIOS Parameter Block
    */
    error_code = MFS_Read_device_sector(drive_ptr, BOOT_SECTOR, drive_ptr->DIR_SECTOR_PTR);

    if ( error_code == MFS_NO_ERROR )
    {
        drive_ptr->DIR_SECTOR_NUMBER = BOOT_SECTOR;
        error_code = MFS_Mount_drive_internal(drive_ptr);
    }

    if ( !error_code )
    {
        /* Determine the real sector size */
        if ( sector_size != drive_ptr->BPB.SECTOR_SIZE )
        {
            _mem_free(drive_ptr->DIR_SECTOR_PTR);
            drive_ptr->DIR_SECTOR_PTR = MFS_mem_alloc_system_zero(drive_ptr->BPB.SECTOR_SIZE);
            MFS_Invalidate_directory_sector(drive_ptr);
            if ( drive_ptr->DIR_SECTOR_PTR == NULL )
            {
                error_code = MFS_INSUFFICIENT_MEMORY;
                drive_ptr->MFS_FILE_PTR = NULL;
            }
            else
            {
                _mem_set_type(drive_ptr->DIR_SECTOR_PTR, MEM_TYPE_MFS_DIRECTORY_SECTOR); 
            }
        }

        /* Calculate the free space on disk */
#if MFSCFG_CALCULATE_FREE_SPACE_ON_OPEN
        if ( !error_code )
        {
            MFS_Get_disk_free_space_internal(drive_ptr,(uint_32_ptr)&error_code);
        }
#endif
    }
    else {
    	printf(" open fs read secotor err+\n");
    	drive_ptr->MFS_FILE_PTR = NULL;
    	if(drive_ptr->DIR_SECTOR_PTR) {
    		_mem_free(drive_ptr->DIR_SECTOR_PTR);
    		drive_ptr->DIR_SECTOR_PTR = NULL;
    		
    	}
    	printf(" open fs read secotor err+\n");
    }

    _lwsem_post(&drive_ptr->SEM);

    return(error_code);
}  


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : MFS_Close_Device
* Returned Value   : MQX_OK or an error
* Comments         : Closes MFS driver.
* 
*END*----------------------------------------------------------------------*/

int_32 MFS_Close_Device
    (
    MQX_FILE_PTR             fd_ptr /* [IN] the MFS file handle for the device being closed */
    )
{
    MFS_DRIVE_STRUCT_PTR      drive_ptr;
    FILESYSTEM_INFO_DISK_PTR  fsinfo_ptr;
    int_32                    result = MFS_NO_ERROR;

#if !MFSCFG_READ_ONLY
#if MFSCFG_READ_ONLY_CHECK
    if (MFS_is_read_only (fd_ptr, NULL))
    {
        result = MFS_DISK_IS_WRITE_PROTECTED;
    }
#endif
    if (result != MFS_DISK_IS_WRITE_PROTECTED)
    {
        result = _io_ioctl(fd_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL); 
    }
#endif

    MFS_lock(fd_ptr, &drive_ptr);

#if !MFSCFG_READ_ONLY
    if (result != MFS_DISK_IS_WRITE_PROTECTED)
    {
        MFS_Flush_caches(drive_ptr);
    }
#endif

    if ( _queue_is_empty(&drive_ptr->HANDLE_LIST) )
    {
        if ( drive_ptr->FAT_TYPE == MFS_FAT32 )
        {
#if !MFSCFG_READ_ONLY
            if (result != MFS_DISK_IS_WRITE_PROTECTED)
            {
                fsinfo_ptr = (FILESYSTEM_INFO_DISK_PTR)drive_ptr->DATA_SECTOR_PTR;
                if ( fsinfo_ptr != NULL )
                {
                    htodl(fsinfo_ptr->LEAD_SIG,   FSI_LEADSIG);
                    htodl(fsinfo_ptr->STRUCT_SIG, FSI_STRUCTSIG);
                    htodl(fsinfo_ptr->FREE_COUNT, drive_ptr->FREE_COUNT);
                    htodl(fsinfo_ptr->NEXT_FREE,  drive_ptr->NEXT_FREE_CLUSTER);
                    htodl(fsinfo_ptr->TRAIL_SIG,  FSI_TRAILSIG);

                    MFS_Write_device_sector(drive_ptr, FSINFO_SECTOR, (char_ptr)fsinfo_ptr);
                }
            }
#endif
        }

        MFS_free_drive_data(drive_ptr, TRUE);

        drive_ptr->MFS_FILE_PTR = NULL;
        result = MFS_NO_ERROR;
    }
    else
    {
    	/* liutest add for get unclose handle */
    	MFS_HANDLE_PTR          next_handle;
        next_handle =  (MFS_HANDLE_PTR) _queue_head(&drive_ptr->HANDLE_LIST);
          while ( next_handle )
          {
        	  printf("unclose handle 0x%X\n",(uint_32)next_handle);
               next_handle =  (MFS_HANDLE_PTR) _queue_next(&drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) next_handle);
          }  
          
        result = MFS_SHARING_VIOLATION;
        /* MFS_free_drive_data(drive_ptr, TRUE);

         drive_ptr->MFS_FILE_PTR = NULL;
         result = MFS_NO_ERROR;*/
    }  

    MFS_unlock(drive_ptr,FALSE);

    return result;

}  





/**************************************************************************
*
* $FileName: mfs_handle.c$
* $Version : 3.8.12.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains functions related to using file handles.
*
*END************************************************************************/

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Get_handle
* Returned Value   :  a file handle
* Comments  :
*    Creates a new file handle and adds it the to queue of open handles.
*
*END*---------------------------------------------------------------------*/

MFS_HANDLE_PTR MFS_Get_handle
    (
    MFS_DRIVE_STRUCT_PTR   drive_ptr,       /*[IN] the drive on which to operate */
    MFS_DIR_ENTRY_PTR      dir_entry_ptr    /* [IN] pointer to the directory information for this file handle */
    )
{
    MFS_HANDLE_PTR    handle_ptr;

    handle_ptr = _partition_alloc_system_zero( drive_ptr->HANDLE_PARTITION );
    printf("alloc handle 0x%x\n", (uint_32)handle_ptr);

    /*
    ** initialize the handle
    */
    if ( handle_ptr != NULL )
    {

        _queue_enqueue( &drive_ptr->HANDLE_LIST, (QUEUE_ELEMENT_STRUCT_PTR) handle_ptr);
        printf("enqueue handle 0x%x\n", (uint_32)handle_ptr);

        handle_ptr->VALID = MFS_VALID;

        if ( dir_entry_ptr )
        {
            _mem_copy(dir_entry_ptr, &handle_ptr->DIR_ENTRY,sizeof(MFS_DIR_ENTRY));
        }
    }
    return(handle_ptr);
}  


