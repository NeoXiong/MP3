/**HEADER********************************************************************
 *
 * Copyright (c) 2008 Freescale Semiconductor;
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
 * $FileName: sh_audio.c$
 * $Version : 3.8.6.0$
 * $Date    : Sep-25-2012$
 *
 * Comments:
 *
 *   This include file is used to provide information needed by
 *   applications using the SAI I/O functions.
 *
 *END************************************************************************/

#include <mqx.h>
#include <fio.h>
#include "sh_audio.h"
#include <shell.h>
#include <string.h>
#include "lp_main.h"
#include "fs_walker.h"

extern LWEVENT_STRUCT player_event;
extern char full_path[];

/*FUNCTION****************************************************************
 * 
 * Function Name    : Shell_play
 * Returned Value   : SHELL_EXIT_SUCCESS if everything is ok,
 *                    SHELL_EXIT_ERROR otherwise
 * Comments         :
 *    Shell function for playing wave files 
 *
 *END*********************************************************************/

int_32 Shell_play(int_32 argc, char_ptr argv[]) {
	boolean print_usage, shorthelp = FALSE;
	int_32 return_code = SHELL_EXIT_ERROR;
	char_ptr c = NULL;
	
	print_usage = Shell_check_help_request(argc, argv, &shorthelp);

	/* Check if help should be printed out */
	if (print_usage) {
		if (shorthelp) {
			printf("%s <device> <filename>\n", argv[0]);
		} else {
			printf("Usage: %s <device> <filename>\n", argv[0]);
			printf("   device        = playback device (i.e. \"sai:\")\n");
			printf("   filename      = file(mp3/wma/wav) to play\n");
		}
		return (SHELL_EXIT_SUCCESS);
	}
	
	/* Check if there is enough parameters */
	if (argc < 3) {
		printf("  Error: Not enough parameters.\n");
		return (SHELL_EXIT_ERROR);
	}
	
	
	strcpy(full_path, argv[2]);
	// get the short file name
	c = argv[2];
	while(*c != '\0') c++;
	while((*c != '\\') && (*c !='/')) c--;
	c++;
	// update the current file name for file searching.
	set_cur_file(c);
	
	_lwevent_set(&player_event, PLAYER_EVENT_MSK_SHELL_COMMAND);

	return (return_code);
}
int_32 Shell_record(int_32 argc, char_ptr argv[]) {
	return 0;
}
/* EOF */
