#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  
    int fd = running->syscall_args[0]; 
    SemDescriptor* desc = SemDescriptorList_byFd(&running->sem_descriptors, fd); 
  
    if(!desc){
        running->syscall_retvalue=DSOS_ERRSEMDESBYFD;
	return;
    }

    Semaphore* sem = desc->semaphore;
    if(!sem){
    	running->syscall_retvalue= DSOS_ERRNOTSEM;
	return;
    }

    sem->count++;
    SemDescriptorPtr* sem_des_ptr;
  
    if (sem->count <= 0) {
        sem_des_ptr=(SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);
	PCB* pcb = sem_des_ptr->descriptor->pcb;  
        List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_des_ptr);
        List_detach(&waiting_list, (ListItem*) pcb);
	List_insert(&ready_list, (ListItem*) ready_list.last, (ListItem*) pcb);
	pcb->status = Ready; 
    }
    running->syscall_retvalue = 0;
    return;
}
