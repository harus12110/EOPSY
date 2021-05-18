#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

// future key for the set of semaphores
int semaphores_key;
// variable that stores semctl and semop return value to check for errors
int error_check;

void grab_forks(int left_fork_id){

	struct sembuf sem_operations[2];
	sem_operations[0].sem_num = left_fork_id;	// choose two forks to pick up
	sem_operations[1].sem_num = (left_fork_id+1) % 5;
	sem_operations[0].sem_op = -1;	// "get hold" of semaphores
	sem_operations[1].sem_op = -1;	// by changing the value to 0
	sem_operations[0].sem_flg = 0;	// set no additional flags
	sem_operations[1].sem_flg = 0;
	// perform subtraction of 1 operation on 2 semaphores in the set assosiated with semaphores_key
	int error_check = semop(semaphores_key, sem_operations, 2);
	if(error_check == -1){

		printf("grab_forks semop error number %d: %s\n", errno, strerror(errno));

	}

}

void put_away_forks(int left_fork_id){

	struct sembuf sem_operations[2];
	sem_operations[0].sem_num = left_fork_id;	// choose two forks to pick up
	sem_operations[1].sem_num = (left_fork_id+1) % 5;
	sem_operations[0].sem_op = 1;	// "release" semaphores
	sem_operations[1].sem_op = 1;	// by changing the value back to 1
	sem_operations[0].sem_flg = 0;	// set no additional flags
	sem_operations[1].sem_flg = 0;
	// perform addition of 1 operation on 2 semaphores in the set assosiated with semaphores_key
	int error_check = semop(semaphores_key, sem_operations, 2);
	if(error_check == -1){

		printf(" put_away_forks semop error number %d: %s\n", errno, strerror(errno));

	}

}

int philosopher(int philosopher_id){

	int eat = 0;	// counting has many times has a philosopher eaten

	while(1){

		// Think
		printf("philosopher[%d]: thinking\n", philosopher_id);
		sleep(4);
		// Pick up forks
		printf("philosopher[%d]: trying to pick up the forks\n", philosopher_id);
		grab_forks(philosopher_id);
		// Eat
		printf("philosopher[%d]: eating\n", philosopher_id);
		sleep(1);
		eat++;
		// Put down fork
		printf("philoospher[%d]: putting down forks. Ate %d times\n", philosopher_id, eat);
		put_away_forks(philosopher_id);

	}

}

int main(){

	// create a new set of 5 semaphores
	semaphores_key = semget(IPC_PRIVATE, 5, 0x1ff);
	// check if creation was successful
	if(semaphores_key == -1){

		printf("semget error number %d: %s\n", errno, strerror(errno));

	}
	// set value of semaphore set to 1
	int i;
	for(i = 0; i < 5; i++){

		error_check = semctl(semaphores_key, i, SETVAL, 1);
		// check if value change was successful
		if(error_check == -1){

			printf("semctl error number %d: %s\n", errno, strerror(errno));

		}

	}
	// create 5 processes, one for each philospher
	for(i = 0; i < 5; i++){

		pid_t fork_return = fork();

		if(fork_return == -1){

			printf("creating child process failed\n");
			return 1;

		}else if(fork_return == 0){

			return philosopher(i);

		}

	}

	int status;
	while(1){

		pid_t wait_ret = wait(&status); // wait for child process end

	}
	return 0;

}
