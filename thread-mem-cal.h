#include <pthread.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "list.h"


struct address_of_thread
{
    struct list_head list ;
    void *add ;
    size_t size ; // Bytes
};

struct thread_list{
    struct list_head list ;
    pthread_t tid ;
    size_t usage ; // Bytes
    struct address_of_thread address;
};

struct thread_list thread_mm_list ;
pthread_mutex_t thread_mm_list_mutex ;

/* FixMe Need to implement the generic way to populate thread mem usage somewhere in filesystem
 * or user specific area */
void write_to_proc ( unsigned long usage )
{
    char path[1024];
    char usage_str[1024];
    FILE *fp ;
    //equivalent to /proc/%d/task/%ld/us_mem_usage
    sprintf(path,"/tmp/%d/task/%ld/us_mem_usage",getpid(),syscall(SYS_gettid));
    sprintf(usage_str,"%ld",usage);
    if ( ( fp = fopen(path,"w+") ) == NULL )
    {
        perror("Error: ");
        return ;
    }
    fputs(usage_str,fp);
    fclose(fp);
}

int isEmpty(struct list_head *head)
{
    return (head->next == NULL && head->prev == NULL);
}
void *thread_malloc ( size_t size )
{
    void *address ;
    struct thread_list *tmp ;
    struct address_of_thread *address_tmp ;
    pthread_t tid = pthread_self();

    if ( ( address = malloc(size) ) == NULL )
        return NULL ;
    pthread_mutex_lock (&thread_mm_list_mutex);
    if ( !isEmpty(&(thread_mm_list.list)) )
    {
        list_for_each_entry(tmp, &(thread_mm_list.list), list){
            if ( tmp->tid == tid )
            {
                tmp->usage = tmp->usage + size ;
                //write_to_proc(tmp->usage);

                address_tmp = (struct address_of_thread *)malloc(sizeof(struct address_of_thread));
                address_tmp->add = address ;
                address_tmp->size = size ;
                list_add(&(address_tmp->list), &(tmp->address.list));

                pthread_mutex_unlock (&thread_mm_list_mutex);
                return address ;
            }
        }
    }
    else
    {
        INIT_LIST_HEAD(&(thread_mm_list.list));
    }
    //printf("gettid %d\n",syscall(SYS_gettid));
    tmp = (struct thread_list *)malloc(sizeof(struct thread_list));
    tmp->tid = tid ;
    tmp->usage = size ;
    //write_to_proc(tmp->usage);

    INIT_LIST_HEAD(&(tmp->address.list));
    address_tmp = (struct address_of_thread *)malloc(sizeof(struct address_of_thread));
    address_tmp->add = address ;
    address_tmp->size = size ;
    list_add(&(address_tmp->list), &(tmp->address.list));

    list_add(&(tmp->list), &(thread_mm_list.list));
    pthread_mutex_unlock (&thread_mm_list_mutex);
    return address ;
}

void thread_free(void *ptr)
{
    free(ptr);

    struct thread_list *tmp ;
    struct address_of_thread *address_tmp ;

    pthread_mutex_lock (&thread_mm_list_mutex);
    if (ptr != NULL)
    {
        list_for_each_entry(tmp, &(thread_mm_list.list), list){
            list_for_each_entry(address_tmp, &(tmp->address.list), list){
                if ( address_tmp->add == ptr ){
                    tmp->usage = tmp->usage - address_tmp->size ;
                    //write_to_proc(tmp->usage);
                    list_del(&(address_tmp->list));
                    free(address_tmp);
                    pthread_mutex_unlock (&thread_mm_list_mutex);
                    return ;
                }
            }
        }
    }
    pthread_mutex_unlock (&thread_mm_list_mutex);
}

void thread_memory_usage ( )
{
    struct thread_list *tmp ;
    list_for_each_entry(tmp, &thread_mm_list.list, list)
        printf("Thread ID ->%lu\tMemory Usage ->%lu\n",tmp->tid,tmp->usage);
}
