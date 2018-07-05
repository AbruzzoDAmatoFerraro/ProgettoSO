#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){

int fd = running->syscall_args[0];
SemDescriptor* desc = SemDescriptorList_byFd(&running->sem_descriptors, fd);
if (!desc) {
        running->syscall_retvalue = DSOS_ESEMAPHORENOFD;
        return;
    }
    
    printf("Pid %d requesting semwait on %d semaphore\n", running->pid, desc->semaphore->id);

    SemDescriptorPtr* desc_ptr; = desc->ptr;
    if(!desc_ptr){
        running->syscall_retvalue = DSOS_ESEMAPHOREDESC;
        return;
    }
    
    Semaphore* sem = desc->semaphore;
    if (!sem) {
        running->syscall_retvalue = DSOS_ESEMAPHORENOTAVAIBLE;
        return;
    }
    
    PCB* p;
    sem->count--;

    if(sem->count < 0){
        List_detach(&sem->descriptors, (ListItem*) desc_ptr);
        List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*) desc->ptr);
        List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
        running->status = Waiting;
        p = (PCB*) List_detach(&ready_list, (ListItem*) ready_list.first);
        running = (PCB*)p;
    }
    
    running->syscall_retvalue=0;
    return;
}
