#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"

#define CICLI 5
#define NPRODUCER 2

void produce(int prod, int cons){
    for (int i = 0; i < CICLI; i++){
        disastrOS_semWait(prod);
        printf("-----PRODOTTO-----\n");
        disastrOS_semPost(cons);
    }
}

void consume(int prod,int cons){
    for (int i = 0; i < CICLI; i++){
        disastrOS_semWait(cons);
        printf("-----CONSUMATO-----\n");
        disastrOS_semPost(prod);
    }
}

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);

  printf("-----APRO I SEMAFORI-----\n");  

  int prod = disastrOS_semOpen(1,NPRODUCER);
  int cons = disastrOS_semOpen(2,0);
  
  printf("-----ASPETTO PRIMA DI INIZIARE-----\n");
  disastrOS_sleep(20);
  
  if (disastrOS_getpid() == 3) {
      printf("-----PROCESSO NUMERO 3 PRODUCE-----\n");
      produce(prod, cons);
  }

  if (disastrOS_getpid() == 4){
      printf("-----PROCESSO NUMERO 4 CONSUMA-----\n");
      consume(prod, cons);
  }
  
  printf("-----PID: %d, TERMINA-----\n", disastrOS_getpid());
  printf("-----CHIUDO I SEMAFORI-----\n");
  disastrOS_semClose(prod);
  disastrOS_semClose(cons);
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  
  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<3; ++i) { //MODIFICATO NUMERO
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    //disastrOS_printStatus();
    //printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	//   pid, retval, alive_children);
    --alive_children;
  }
  disastrOS_printStatus();
  printf("shutdown!");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}

