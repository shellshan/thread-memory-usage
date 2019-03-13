#include <pthread.h>
#include <stdlib.h>
#include "list.h"
struct thread_list{
								struct list_head list ;	
								pthread_t tid ;
								size_t usage ; // Bytes
};

struct thread_list thread_mm_list ;
pthread_mutex_t thread_mm_list_mutex ;

void *thread_malloc ( size_t size ) 
{
								void *address ; 
								struct thread_list *tmp ;
								pthread_t tid = pthread_self();

								if ( ( address = malloc(size) ) == NULL )
																return NULL ; 
        pthread_mutex_lock (&thread_mm_list_mutex);
								if ( !isEmpty(&(thread_mm_list.list)) )
								{
																list_for_each_entry(tmp, &thread_mm_list.list, list){
																								if ( tmp->tid == tid )
																								{
																																tmp->usage = tmp->usage + size ; 
                                pthread_mutex_unlock (&thread_mm_list_mutex);
																																return address ;
																								}
																}
								}
								else
								{
																INIT_LIST_HEAD(&thread_mm_list.list);
								}
								tmp = (struct thread_list *)malloc(sizeof(struct thread_list));
								tmp->tid = tid ; 
								tmp->usage = size ;
								list_add(&(tmp->list), &(thread_mm_list.list));
        pthread_mutex_unlock (&thread_mm_list_mutex);
								return address ; 
}

void thread_memory_usage ( )
{
								struct thread_list *tmp ;
								list_for_each_entry(tmp, &thread_mm_list.list, list)
																printf("Thread ID ->%lu\tMemory Usage ->%lu\n",tmp->tid,tmp->usage); 
}

int isEmpty(struct list_head *head)
{       
								return (head->next == NULL && head->prev == NULL);     
}       

