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
#ifndef _DIR_R_H_
#define _DIR_R_H

#include <mqx.h>
#include <fio.h>
#include <shell.h>
#include <mfs.h>

typedef enum{
  CALLBACK_TYPE_DIR,
  CALLBACK_TYPE_FILE,
}callback_type_e;

typedef int (*fn_call_back)(uint_32 dir_depth, MFS_SEARCH_DATA_PTR search_data);

void mfs_dir_set_callback(fn_call_back dir_cb, fn_call_back file_cb);
int_32 mfs_dir_r(MQX_FILE_PTR fs_ptr, char *path_ptr);
char_ptr mfs_dir_get_path(void);
void mfs_dir_reset(void);

#endif