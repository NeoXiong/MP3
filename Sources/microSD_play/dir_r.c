/**HEADER********************************************************************
 *
 * Copyright (c) 20013 Freescale Semiconductor;
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
 *END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include <mfs.h>
#include "dir_r.h"

//#define LONG_FILE_NAME_SUPPORTED

#define MAX_PATH	256
#define MAX_DIR_DEPTH        8

static uint_32 dir_depth = 0;
static char maxPath[MAX_PATH];
#ifdef LONG_FILE_NAME_SUPPORTED
static char lfn[PATHNAME_SIZE];
static MFS_GET_LFN_STRUCT lfn_data;
#endif

static fn_call_back cb[2] = { NULL, NULL };

#ifdef _DEBUG_
static void dump_search_data(MFS_SEARCH_DATA_PTR search_data)
{
	printf("NAME: %s\n", search_data->NAME);
	printf("INTERNAL_SEARCH_DATA.CURRENT_CLUSTER: 0x%x\n", search_data->INTERNAL_SEARCH_DATA.CURRENT_CLUSTER);
	printf("INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX: 0x%x\n", search_data->INTERNAL_SEARCH_DATA.DIR_ENTRY_INDEX);
	printf("INTERNAL_SEARCH_DATA.PREV_CLUSTER: 0x%x\n", search_data->INTERNAL_SEARCH_DATA.PREV_CLUSTER);
	printf("INTERNAL_SEARCH_DATA.FULLNAME: 0x%08x\n", search_data->INTERNAL_SEARCH_DATA.FULLNAME);
	printf("INTERNAL_SEARCH_DATA.SRC_PTR: 0x%08x\n", search_data->INTERNAL_SEARCH_DATA.SRC_PTR);
	printf("INTERNAL_SEARCH_DATA.FILENAME: 0x%08x\n", search_data->INTERNAL_SEARCH_DATA.FILENAME);
}
#endif

void mfs_dir_set_callback(fn_call_back dir_cb, fn_call_back file_cb) {
	cb[CALLBACK_TYPE_DIR] = dir_cb;
	cb[CALLBACK_TYPE_FILE] = file_cb;
}

char_ptr mfs_dir_get_path(void) {
	return maxPath;
}

void mfs_dir_reset(void) {
	maxPath[0] = '\0';
}

/*
 * This function is not task safe, it is expected it should not be called by two or more
 * task s
 * fs_ptr:      file system pointer
 * path_ptr:    short name expected     
 */
int_32 mfs_dir_r(MQX_FILE_PTR fs_ptr, char *path_ptr) {
	MFS_SEARCH_PARAM_PTR search_para = NULL;
	MFS_SEARCH_DATA_PTR search_data = NULL;
	char_ptr c = NULL;
	uint_32 rc = 0, error = 0;
	uint_32 pos = 0;

	// reset the path
	if (dir_depth == 0) {
		maxPath[0] = '\0';
	}

	dir_depth++;
	
	// 'push' the name of the sub-foler to the 'stack'
	strcat(maxPath, path_ptr);
	// add '\' if needed, and also add wildcat 
	pos = strlen(maxPath);
	if ((maxPath[pos - 1] != '\\') && (maxPath[pos - 1] != '/')) {
		maxPath[pos] = '\\';
		maxPath[pos + 1] = '\0';
	}
	pos = strlen(maxPath);
	maxPath[pos] = '*';
	maxPath[pos + 1] = '\0';
	
	// Return if the max nest level gotten. After all, MCU's RAM is not big. 
	if (dir_depth >= MAX_DIR_DEPTH){
		dir_depth--;	// pop the dir depth
		return -2;
	}
	
	/*
	 * Located some variable in heap to void big stack of the task, after all, we can release the 
	 * buffers later.
	 */
	search_para = (MFS_SEARCH_PARAM_PTR) _mem_alloc(sizeof(MFS_SEARCH_PARAM));
	if (NULL == search_para) {
		rc = -1;
		goto clean_up;
	}

	search_data = (MFS_SEARCH_DATA_PTR) _mem_alloc(sizeof(MFS_SEARCH_DATA));
	if (NULL == search_data) {
		rc = -1;
		goto clean_up;
	}
	search_para->ATTRIBUTE = 0x80;//match all
	search_para->SEARCH_DATA_PTR = search_data;
	search_para->WILDCARD = maxPath;

	error = ioctl(fs_ptr, IO_IOCTL_FIND_FIRST_FILE, (pointer) search_para);

	while (MFS_NO_ERROR == error) {

		if (((search_data->ATTRIBUTE & MFS_ATTR_DIR_NAME) == MFS_ATTR_DIR_NAME)
				&& (0 != strcmp(".", search_data->NAME)) && // Skip if the directory name is . or ..
				(0 != strcmp("..", search_data->NAME))) {
			pos = strlen(maxPath);
			// Remove the '*' added by this function, and append new directory pathname.
			maxPath[pos - 1] = '\0';

			// put dir call back here
			if (NULL != cb[CALLBACK_TYPE_DIR]) {
				cb[CALLBACK_TYPE_DIR](dir_depth, search_data);
			}

			//go into the sub-dir, the maxPath is like as "a:\"
			mfs_dir_r(fs_ptr, search_data->NAME);

			// come back from the sub-dir, the maxPath is like as "a:\dir1\*" here, just like a 'stack-pop'
			c = maxPath;
			while (*c != '\0')
				c++;
			while ((*c != '\\') && (*c != '/'))
				c--;
			c--;
			while ((*c != '\\') && (*c != '/'))
				c--;
			c++;
			*c = '*';
			c++;
			*c = '\0';
			//maxPath is "a:\*" here
		}

		if ((search_data->ATTRIBUTE & MFS_ATTR_DIR_NAME) != MFS_ATTR_DIR_NAME) {
			// put file call-back here
			if (NULL != cb[CALLBACK_TYPE_FILE]) {
				cb[CALLBACK_TYPE_FILE](dir_depth, search_data);
			}
#ifdef LONG_FILE_NAME_SUPPORTED                 
			lfn_data.PATHNAME = search_data->NAME;
			lfn_data.LONG_FILENAME = (char_ptr)lfn;
			lfn_data.SEARCH_DATA_PTR = search_data;

			error = ioctl(fs_ptr, IO_IOCTL_GET_LFN, (pointer) (&lfn_data));
			if(MFS_NO_ERROR == error) lfn_file_listed++;

			for(i = dir_depth; i>0; i--) {
				printf("  ");
			}

			if(MFS_NO_ERROR == error ) {
				printf("%s\n", lfn_data.LONG_FILENAME);
			}
#endif                
		}

		error = ioctl(fs_ptr, IO_IOCTL_FIND_NEXT_FILE, (pointer) search_data);
	}

	clean_up: if (NULL != search_para)
		_mem_free(search_para);
	if (NULL != search_data)
		_mem_free(search_data);

	dir_depth--;

	return rc;
}
