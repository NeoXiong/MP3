/*HEADER*********************************************************************
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
* $FileName: io_util.c$
* $Version : 3.8.12.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Contains misc io functions.
*
*END************************************************************************/

#include "mqx_inc.h"
#include "fio.h"
#include <ctype.h>
#include <string.h>

extern FS_TABLE_ENTRY _opened_fs_table[MAX_FS_INSTANCES];

/*!
 * \brief Gets pointer on the row where fs_name is stored.
 * 
 * \param[in] fs_name string that is being looked for.
 * 
 * \return Pointer to the row where the fs_name is stored.
 * \return NULL (failure)  
 */ 
MQX_FILE_PTR _io_get_fs_by_name(char_ptr fs_name)
{
   int i;
   char_ptr a;
   char_ptr b;

   if (*fs_name == '\0')
     return NULL;
   
   for(i=0;i<MAX_FS_INSTANCES;i++)
   {
      a = _opened_fs_table[i].FS_NAME;
      b = fs_name;

      while (*a && (*a == *b))
      {
         a++;
         b++;
      }

      // check whether name from the table is prefix of fs_name
      if ((*a == '\0') && (_opened_fs_table[i].FS_PTR != NULL))
      {
         return _opened_fs_table[i].FS_PTR;
      }
   }
   // filesystem not found in table
   return NULL;
}

