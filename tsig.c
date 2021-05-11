#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define NUM_CHILD 7

#ifdef WITH_SIGNALS

int interruptFlag = 0;

void signalHandler(int signalType, void(*signalHandler)(int)){

	struct sigaction sigAction;
	sigAction.sa_handler = signalHandler; 
	sigemptyset(&sigAction.sa_mask);			// don't block/add to mask any signal
	sigAction.sa_flags = 0;						// don't additionally modify the signal
	sigaction(signalType, &sigAction, NULL);

}

void parentSigintHandler(int signalType){

	if(signalType == SIGINT){
		printf("parent[%d]\treceived keyboard interrupt\n", getpid());
		interruptFlag = 1;
	}else{
		printf("parent[%d]\treceived a signal different from SIGINT\n", getpid());
	}

}

void childSigtermHandler(int signalType){

	if(signalType == SIGTERM){
		printf("child[%d]:\tprocess termination after keyboard interrupt\n", getpid());
	}else{
		printf("child[%d]:\treceived a signal different from SIGTERM\n", getpid());
	}

}

#endif

void terminate_children(pid_t *child_list, int list_size){

	printf("parent[%d]:\tsending SIGTERM signal to all child processes\n", getpid());
	int i;
	for(i = 0; i < list_size; i++){
		int kill_res = kill(child_list[i], SIGTERM);	// send SIGTERM signal to child process with ID child_list[i]
		if(kill_res == 0)printf("parent[%d]:\tsend SIGTERM to child[%d]\n", getpid(), child_list[i]);
	}

}

int main(){

	// create a list of all child processes (needed for eventual termination)
	pid_t child_list[NUM_CHILD];

	#ifdef WITH_SIGNALS
	// ignore all signals
	int j;
	for(j = 0; j < NSIG; j++){		// NSIG = total numbers of signals defined
		signal(j, SIG_IGN);
	}
	// restore default SIGCHLD signal
	signal(SIGCHLD,SIG_DFL);		// SIGCHLD - signal send by child process at it's exit or interruption
	// set up own interrupt handler
	signalHandler(SIGINT, parentSigintHandler);
	#endif


	// set up a child creating for loop
	int i;
	for(i = 0; i < NUM_CHILD; i++){

		#ifdef WITH_SIGNALS
		if(interruptFlag == 1){
			printf("parent[%d]:\tinterrupt of creation process\n", getpid());
			terminate_children(child_list,i);
			break;
		}
		#endif

		// create new child using fork
		pid_t fork_return = fork();
		if(fork_return == -1){				// fork_return return -1 if chiild creation failed
			printf("creating child process failed\n");
			// send termination signal to all child processes
			if(i != 0)terminate_children(child_list, i);
			return 1;
		}else if(fork_return == 0){			// fork_return returns 0 in child process
			#ifdef WITH_SIGNALS
			// ignore interrupt signal
			signal(SIGINT, SIG_IGN);
			// set up own SIGTERM handler
			signalHandler(SIGTERM, childSigtermHandler);
			#endif
			printf("child[%d]:\tcreated from parent[%d]\n", getpid(), getppid());
			sleep(10);
			printf("child[%d]:\texecution completion\n", getpid());
			return 0;
		}else{
			child_list[i] = fork_return;	// fork_return in parent returns the ID of created child
			if(i != NUM_CHILD-1){
				sleep(1);
			}else{
				printf("all child processes have been created\n");
			}
		}

	}

	int exit_count = 0;
	int status;

	// waiting for all processes to finish
	while(1){

		pid_t wait_ret = wait(&status);	// waiting for a child process to finish
		if(wait_ret > 0){				// wait returns ID of terminated child on success
			exit_count++;
		}else if(wait_ret == -1){		// wait returns -1 on failure i.e. when there are no child processes left
			printf("no more child processes are present\n");
			break;
		}

	}

	// restore old service handlers
	#ifdef WITH_SIGNALS
	for(j = 0; j < NSIG; j++){
		signal(j, SIG_DFL);
	}
	#endif
	printf("received %d child process exit codes\n", exit_count);
	return 0;

}
