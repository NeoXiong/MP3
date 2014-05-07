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
 
#ifndef _PLAY_LIST_H_
#define _PLAY_LIST_H_

#include <mqx.h>

typedef enum{
    DIR_SEARCH_POLICY_ALPHA,
    DIR_SEARCH_POLICY_TIME,
    DIR_SEARCH_POLICY_RANDOM,
}DIR_SEARCH_POLICY_e;

typedef enum{
  DIR_SEARCH_DIRECTION_NEXT,
  DIR_SEARCH_DIRECTION_PREV,
}DIR_SEARCH_DIRECTION_e;

void set_search_policy(DIR_SEARCH_POLICY_e policy);
void set_file_filter(char **filter);
void set_search_direction(DIR_SEARCH_DIRECTION_e direction);
void set_cur_file(char_ptr name);
int get_next_file_name(char_ptr dir, char_ptr file_name);
int get_prev_file_name(char_ptr dir, char_ptr file_name);

#endif


/* EOF */
