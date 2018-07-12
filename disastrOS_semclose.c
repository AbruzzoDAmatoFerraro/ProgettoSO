#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){

    int fd = running->syscall_args[0];
    SemDescriptor* sem_desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);

    if(!sem_desc){
        running->syscall_retvalue = DSOS_ESEMAPHORENOFD;
        return;
    }

    List_detach(&running->sem_descriptors, (ListItem*)sem_desc);
    Semaphore* sem = sem_desc->semaphore;
    
    if(!sem){
        running->return_value=DSOS_ESEMAPHORENOAVAILABLE;
        return;
    }

    SemDescriptorPtr* sem_desc_ptr = (SemDescriptorPtr*)List_detach(&(sem->descriptors), (ListItem*)(sem_desc->ptr));
    assert(sem_desc_ptr);

    if(sem->descriptors.size == 0 && sem->waiting_descriptors.size==0){
        List_detach(&semaphores_list, (ListItem*)sem);
        Semaphore_free(sem);
    }

    SemDescriptor_free(sem_desc);
    SemDescriptorPtr_free(sem_desc_ptr);
    running->syscall_retvalue = 0;
    return;
}
