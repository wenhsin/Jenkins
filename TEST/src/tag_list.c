#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "tag_list.h"
#include "dcenter.h"
#include "file_warp.h"

T_TAG *list;
pthread_mutex_t list_mutex;

void tag_list_init(void)
{
	list = NULL;
	pthread_mutex_init(&list_mutex, NULL);
}

void tag_list_destroy(void)
{
	T_TAG *curr;
	while(list != NULL)
	{
		curr = list;
		list = list->next;
		lprintf("free tag:%s\n", curr->id);
		free(curr);
	}
	pthread_mutex_destroy(&list_mutex);
}

T_TAG* create(char *tag_id)
{
	T_TAG *tag = (T_TAG*) malloc(sizeof(T_TAG));
	if(tag == NULL)  
	{
		lprintf("allocate memory fail\n");
		return NULL;
	}
	
	memset(tag, 0, sizeof(T_TAG));
	strncpy(tag->id, tag_id, sizeof(tag->id));
	tag->detect_cnt = 1;
	tag->enable = true;
	//tag->cold_time = 0;
	tag->next = NULL;
	return tag;
}

void insert(char *tag_id)
{
	T_TAG *newTag;
	newTag = create(tag_id);
	if(newTag != NULL)
	{
		pthread_mutex_lock(&list_mutex);
		//add into list head
		newTag->next = list;
		list = newTag;
		pthread_mutex_unlock(&list_mutex);
	}	
}

void del(T_TAG *a)
{
	T_TAG* curr = list;
	if(list == a) 
	{
		pthread_mutex_lock(&list_mutex);
		list = list->next;
		lprintf("free tag:%s\n", a->id);
		free(a);
		pthread_mutex_unlock(&list_mutex);
		return;
	}
	
	while(curr)
	{
		if(curr->next == a)
		{
			pthread_mutex_lock(&list_mutex);
			curr->next = a->next;
			lprintf("free tag:%s\n", a->id);
			free(a);
			pthread_mutex_unlock(&list_mutex);
			return;
		}
		curr = curr->next;
	}
}

int get_list_length(void)
{
	int len = 0;
	T_TAG *p =  list;
	
	while(p != NULL)
	{	
		len++;
//debug
		lprintf("DB: tag=%s, cnt=%d, cold=%d\n", p->id, p->detect_cnt, p->cold_time);
		p = p->next;
	}
	return len;
}

void tag_add_list(char *tag_id)
{
	//traversal all list, if not exist than insert
	unsigned char process = false;
	T_TAG *curr =  list;	
	while(curr != NULL)
	{	
		if(strcmp(tag_id, curr->id) == 0)
		{
			pthread_mutex_lock(&list_mutex);
			(curr->detect_cnt)++;
			curr->cold_time = 0;
			process = true;
			pthread_mutex_unlock(&list_mutex);
            if((curr->detect_cnt)%TAG_VALID_TIMES == 0)
            {
                dcenter_send_tag(curr->id, strlen(curr->id));
            }
			break;
		}
		curr = curr->next;
	}
	
	if(process == false) 
	{
		insert(tag_id);
	}
}

void tag_remove_list(void)
{
	//if cold time expired than delete or cold_time++
	T_TAG *curr =  list, *temp;
		
	while(curr != NULL)
	{	
		if(curr->cold_time >= COLD_TIME)
		{
			temp = curr;
			curr = curr->next;
			del(temp);
		}
		else 
		{
			(curr->cold_time)++;
			curr = curr->next;
		}
	}
	
}
