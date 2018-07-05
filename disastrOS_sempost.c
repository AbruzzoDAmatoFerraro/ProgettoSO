#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  
  int fd = running->syscall_args[0]; //The argument of the semaphore is file descriptor
  
  SemDescriptor* desc = SemDescriptorList_byFd(&running->sem_descriptors, fd); //Get the semaphore descriptor from fd
  
  if(!desc){
		running->syscall_retvalue=DSOS_ESEMAPHORENOFD;
		return;
	}

  printf("Pid %d requesting sempost on %d semaphore\n", running->pid, desc->semaphore->id);


  Semaphore* sem = desc->semaphore;
  
  if(!sem){
		running->syscall_retvalue= DSOS_ESEMAPHORENOAVAILABLE;
		return;
	 }

  
  sem->count++;
  
  SemDescriptorPtr* sem_des_ptr;
  
  if (sem->count <= 0) {
        
        sem_des_ptr = (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors, sem->waiting_descriptors.first);
        
        if(!sem_des_ptr){
            running->syscall_retvalue = DSOS_ERESOURCEOPEN;
            return; }
    
        List_detach(&waiting_list, (ListItem*)sem_des_ptr->descriptor->pcb); //pcb removed from waiting list

        List_insert(&ready_list, ready_list.last, (ListItem*)sem_des_ptr->descriptor->pcb); //add process to the ready list
        
	      List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) sem_des_ptr);
        

        sem_des_ptr->descriptor->pcb->status = Ready;


    }
    running->syscall_retvalue = 0;
   
	return;
}
