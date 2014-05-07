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
#include <shell.h>
#include <mfs.h>
#include <string.h>
#include "lp_main.h"
#include "dir_r.h"
#include "fs_walker.h"

static char cur_file_name[13];
static char tmp_file_name[13];
static DIR_SEARCH_POLICY_e search_polocy = DIR_SEARCH_POLICY_ALPHA;
static DIR_SEARCH_DIRECTION_e search_direction = DIR_SEARCH_DIRECTION_NEXT;
static char **file_filter = NULL;

static char name_record[13];
static char path_name[256];

static int cb_dir(uint_32 dir_depth, MFS_SEARCH_DATA_PTR search_data) {
	int_32 i = 0;

	for (i = dir_depth; i > 0; i--) {
		printf("  ");
	}

	printf("DIR:\t%s\n", search_data->NAME);

	return 0;
}

static int cb_file(uint_32 dir_depth, MFS_SEARCH_DATA_PTR search_data) {
	char_ptr c = search_data->NAME;
	char **ext = file_filter;
	uint_32 found = 0;

	while (*c != '\0')
		c++;
	while (*c != '.')
		c--;
	c++;

	while (*ext != NULL) {
		if (0 == strcmp(c, *ext)) {
			found = 1;
			break;
		}
		ext++;
	}

	if (found) {
		if (search_direction == DIR_SEARCH_DIRECTION_NEXT) {
			//    printf("%s %s %s\n", search_data->NAME, cur_file_name, tmp_file_name);
			if (strcmp(search_data->NAME, cur_file_name) > 0) {
				if (tmp_file_name[0] == '\0') {
					strcpy(path_name, mfs_dir_get_path());
					strcpy(tmp_file_name, search_data->NAME);
				} else {
					if (strcmp(search_data->NAME, tmp_file_name) < 0) {
						strcpy(tmp_file_name, search_data->NAME);
						strcpy(path_name, mfs_dir_get_path());
					}
				}
			}
		} else {
			if (strcmp(search_data->NAME, cur_file_name) < 0) {
				if (tmp_file_name[0] == '\0') {
					strcpy(path_name, mfs_dir_get_path());
					strcpy(tmp_file_name, search_data->NAME);
				} else {
					if (strcmp(search_data->NAME, tmp_file_name) > 0) {
						strcpy(tmp_file_name, search_data->NAME);
						strcpy(path_name, mfs_dir_get_path());
					}
				}
				//printf("%s %s %s\n", search_data->NAME, cur_file_name, tmp_file_name);
			}
		}
	}

	return 0;
}

void set_search_policy(DIR_SEARCH_POLICY_e policy) {
	search_polocy = policy;
}

void set_file_filter(char **filter) {
	file_filter = filter;
}

void set_search_direction(DIR_SEARCH_DIRECTION_e direction) {
	search_direction = direction;
}

void set_cur_file(char_ptr name){
	strcpy(cur_file_name, name);
}

/*
 * Get next short file name in alphabatic order.
 * 
 * file_name: 	full path name, will be update if file found, 
 *				it is expected that the caller should clear it before the calling. 
 */
int get_next_file_name(char_ptr dir, char_ptr file_name) {
	MQX_FILE_PTR fs_ptr = NULL;

	mfs_dir_set_callback(NULL, cb_file);
	set_search_direction(DIR_SEARCH_DIRECTION_NEXT);

	fs_ptr = _io_get_fs_by_name(dir);

	mfs_dir_r(fs_ptr, dir);

	if (tmp_file_name[0] != '\0') {
		if (0 != strcmp(cur_file_name, tmp_file_name)) { //update cur_file_name
			strcpy(cur_file_name, tmp_file_name);
		}
		// reset the tmp_file_name for next search
		tmp_file_name[0] = '\0';
	}

	if ((cur_file_name[0] != '\0') && // something found, there is possibility 
									   //that the cur_file_name was not updated.
			(0 == strcmp(name_record, cur_file_name))) { //to the end

		// reset cur_file_name for a new search from the beginning
		cur_file_name[0] = '\0';
		mfs_dir_r(fs_ptr, dir); // re-search from the beginning

		if (tmp_file_name[0] != '\0') {
			if (0 != strcmp(cur_file_name, tmp_file_name)) {
				strcpy(cur_file_name, tmp_file_name);
			}
			tmp_file_name[0] = '\0';
		}
	}

	if (cur_file_name[0] != '\0') { //someting found
		strcpy(name_record, cur_file_name);
		strcpy(file_name, path_name); // path_name is like "a:\*"
		file_name[strlen(file_name) - 1] = '\0'; // remove the "*"
		strcat(file_name, cur_file_name);
	}

	return 0;
}

int get_prev_file_name(char_ptr dir, char_ptr file_name) {
	MQX_FILE_PTR fs_ptr = NULL;

	mfs_dir_set_callback(NULL, cb_file);
	set_search_direction(DIR_SEARCH_DIRECTION_PREV);

	fs_ptr = _io_get_fs_by_name(dir);

	mfs_dir_r(fs_ptr, dir);

	if (tmp_file_name[0] != '\0') {
		if (0 != strcmp(cur_file_name, tmp_file_name)) {
			strcpy(cur_file_name, tmp_file_name);
		}
		tmp_file_name[0] = '\0';
	}

	if ((cur_file_name[0] != '\0') && // something found
			(0 == strcmp(name_record, cur_file_name))) { //to the beginning
		// reset cur_file_name to a max value for a new search from the ending
		cur_file_name[0] = 0xFF;
		mfs_dir_r(fs_ptr, dir); // re-search from the beginning

		if (tmp_file_name[0] != '\0') {
			if (0 != strcmp(cur_file_name, tmp_file_name)) {
				strcpy(cur_file_name, tmp_file_name);
			}
			tmp_file_name[0] = '\0';
		}
	}

	if (cur_file_name[0] != '\0' && cur_file_name[0] != 0xFF) { //someting found
		strcpy(name_record, cur_file_name);

		strcpy(file_name, path_name); // path_name is like "a:\*"
		file_name[strlen(file_name) - 1] = '\0'; // remove the "*"
		strcat(file_name, cur_file_name);
	}

	return 0;
}

/*
 * Test routine.
 */
extern char *ext_wantted[];

int_32 Shell_dir_r(int_32 argc, char_ptr argv[]) {
	boolean print_usage, shorthelp = FALSE;
	TIME_STRUCT time, time_new;
	char full_name[255];
	
	print_usage = Shell_check_help_request(argc, argv, &shorthelp);

	/* Check if help should be printed out */
	if (print_usage) {
		if (shorthelp) {
			printf("%s dir \n", argv[0]);
		} else {
			printf("Usage: %s dir \n", argv[0]);
			printf("\tList all the files in the directory.\n");
			printf("   dir        = directory to be listed.\n");
		}
	}

	set_file_filter(ext_wantted);

	_time_get(&time);
	if (0 == strcmp(argv[2], "next"))
		get_next_file_name(argv[1], full_name);
	else if (0 == strcmp(argv[2], "prev"))
		get_prev_file_name(argv[1], full_name);
	_time_get(&time_new);

	printf("%s\n", full_name);
	/*       
	 if(NULL != fopen(full_name, "r")){
	 printf("The file %s opened.\n", full_name);
	 }
	 */
	/*
	 printf("Taken %dms to list %d directories and %d files.\n", 
	 (time_new.SECONDS - time.SECONDS) * 1000 + (time_new.MILLISECONDS - time.MILLISECONDS),
	 dir_listed,
	 file_listed
	 );
	 */

	return (SHELL_EXIT_SUCCESS);
}

