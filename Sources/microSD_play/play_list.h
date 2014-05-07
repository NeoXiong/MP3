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

struct play_list_entry_struct{
	struct play_list_entry_struct *next;
	struct play_list_entry_struct *prev;
	char *path;
};
typedef struct play_list_entry_struct play_list_entry_t;
typedef struct play_list_entry_struct *play_list_entry_p;

typedef struct{
	play_list_entry_p head;
	uint_32 num;
}play_list_t, *play_list_p;

#define PLAY_LIST_INIT(l)	{(l)->head->next = (l)->head; (l)->head->prev = (l)->head;(l)->num = 0;}
#define PLAY_LIST_INSERT(l, e)	\
		do{	\
			play_list_entry_p p = (l)->head;	\
			uint_32 num = (l)->num;	\
			while(num--){	\
				p = p->next;	\
			}	\
			(e)->next = p->next;	\
			p->next->prev = (e);	\
			p->next = (e);	\
			(e)->prev = p;	\
			((l)->num)++;	\
		}while(0)	

#define PLAY_LIST_REMOVE(l, e)	\
	do{	\
		if((l)->num == 0) break;	\
			(e)->next->prev = e->prev;	\
			(e)->prev->next = e->next;	\
			((l)->num)--;	\
	}while(0)

#define PLAY_LIST_NEXT(e)	(e)->next
#define PLAY_LIST_PREV(e)	(e)->prev
	
int_32 list_files(char *path_ptr, char *file_types[], play_list_p list);

#endif


/* EOF */
