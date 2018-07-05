#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"
#include "disastrOS_constants.h"

void internal_semOpen(){

    int id = running->syscall_args[0];
    int count = running->syscall_args[1];
    
    Semaphore* sem = SemaphoreList_byId(semaphores_list, id);

    if (count < 0){
        running->syscall_retvalue = DSOS_ESEMAPHOREOPEN;
        return;
    }
    
    if (!sem) {
        sem=Semaphore_alloc(id, count);
        List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);
    }
    
    int fd = running->last_sem_fd; 
    SemDescriptor* des = SemDescriptor_alloc(fd, sem, running);
    
    if (!des) {
        running->syscall_retvalue = DSOS_ESEMDESCALLOC;
        return;
    }
    
    running->last_sem_fd++;
    List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) des);
    
    SemDescriptorPtr* ptr_desc = SemDescriptorPtr_alloc(des);
    if (ptr_desc) {
        running->syscall_retvalue = DSOS_ESEMAPHOREDESC;
        return;
    }
    
    des->ptr=ptr_desc;
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) ptr_desc);

    running->syscall_retvalue = des->fd;
    return;
}
