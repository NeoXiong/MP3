/**HEADER********************************************************************
 *
 * Copyright (c) 2013 Freescale Semiconductor;
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

#include <string.h>
#include <mqx.h>
#include <fio.h>
#include <mfs.h>
#include <shell.h>
#include "play_list.h"

#define BUFFER_SIZE  		256
#define PLAY_LIST_MAX	32

#define AUDIO_ASSERT(x)	do{	if(!(x))	printf("Assertion %s failed. %s %s", #x, __FILE__, __LINE__);}while(0)
						
/*
 * Para:
 *		path_ptr should be an absolute directory.
 *  	file_types	list all the file extension to be listed
 */

int_32 list_files(char *path_ptr, char *file_types[], play_list_p list) {
	int_32 return_code = 0;
	int_32 len = 0;
	int_32 path_len;
	MQX_FILE_PTR fs_ptr;
	char_ptr mode_ptr;
	pointer dir_ptr;
	char_ptr buffer = NULL;
	int found;
	char_ptr c = NULL;
	char **ext = NULL;
	
	play_list_entry_p entry ;

	mode_ptr = "m";
	fs_ptr = _io_get_fs_by_name(path_ptr);
	/* check if filesystem is mounted */
	if (fs_ptr == NULL) {
		printf("Error, file system not mounted\n");
		return -2;
	}

	/* check whether the path refers to a directory and append wildcard to list its contents if applicable */
	if (ioctl(fs_ptr, IO_IOCTL_CHECK_DIR_EXIST,
			(pointer) path_ptr) == MFS_NO_ERROR) {
		path_len = strlen(path_ptr);
		if (path_len > 0 && path_ptr[path_len - 1] != '\\'
				&& path_ptr[path_len - 1] != '/') {
			path_ptr[path_len++] = '\\';
		}
		if (path_len >= PATHNAME_SIZE) {
			printf("Error, path too long.\n");
			return -3;
		}
        path_ptr[path_len++] = '*';
        path_ptr[path_len] = '\0';

		buffer = _mem_alloc(BUFFER_SIZE);
		if (buffer == NULL) {
			printf("Error, unable to allocate space.\n");
			return -4;
		}

		dir_ptr = _io_mfs_dir_open(fs_ptr, path_ptr, mode_ptr);
		if (dir_ptr == NULL) {
			printf("File not found.\n");
			return_code = -5;
		} else {
			while ((_io_is_fs_valid(fs_ptr))
					&& (len = _io_mfs_dir_read(dir_ptr, buffer, BUFFER_SIZE))
							> 0) {
				c = buffer;
				// to the end of the file info string which in the format as
				// 32K_16~1.WAV  1409004 10-04-2012 16:52:42      A 32k_16bit_2ch.wav\r\n
				while (*c != '\0')
					c++;
				while(*c != '\r') 
					c--;
				*c = '\0';
				while (*c != '.')
					c--;
				//get the extension of the file
				c++;
				found = 0;
				for (ext = file_types; *ext != NULL; ext++) {
					if (0 == strcmp(*ext, c)) {
						found = 1;
						break;
					}
				}

				if (found) {
					// get the file name
					while ((*c != '\t') && (*c != ' '))
						c--;
					c++;
					
					//play_list_entry_p entry = (play_list_entry_p) _mem_alloc(sizeof(play_list_entry_t));
					entry = (play_list_entry_p) _mem_alloc(sizeof(play_list_entry_t));
					if(entry == NULL){ 
						printf("Failed to allocate memory for list entry.\n");
						return_code = -6;
						goto exit_1;
					}
					path_ptr[strlen(path_ptr)-2] = '\0';	//Remove the "*"
					entry->path = (char_ptr)_mem_alloc(strlen(c) + strlen(path_ptr));
					if(NULL == entry->path){
						printf("Failed to allocate memory for file path.\n");
						return_code = -7;
						goto exit_1;
					}
					sprintf(entry->path, "%s%s", path_ptr, c);
					
					printf("%s\n", entry->path);
					
					if(list->num > PLAY_LIST_MAX){
						return_code = -8;
						goto exit_1;
					}
					
					PLAY_LIST_INSERT(list, entry);
				}
			}
exit_1: 
			_io_mfs_dir_close(dir_ptr);
		}

		_mem_free(buffer);
	}
	
	return return_code;
}
