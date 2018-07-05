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

  printf("Pid %d requesting sempost on %d semaphore\n", running->pid, desc->semaphore->id);


  Semaphore* sem = desc->semaphore;
  sem->count++;
  
  SemDescriptorPtr* desc_ptr;
  
  if (sem->count <= 0) {
        
        List_insert(&ready_list, ready_list.last, (ListItem*) running);
        desc_ptr = (SemDescriptorPtr*) List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);
        List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) desc_ptr);
        List_detach(&waiting_list, (ListItem*) desc_ptr->descriptor->pcb);
        running->status = Ready;  // Set ready process state
        running = desc_ptr->descriptor->pcb;  
    }

    running->syscall_retvalue = 0;
    return;
}
