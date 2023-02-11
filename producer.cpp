#include <stdio.h>

#include <stdlib.h>

#include <iostream>

#include <sys/shm.h>

#include <string.h>

#include <queue>

#include <cstdlib>

#include <algorithm>

#include <bits/stdc++.h>

#include <math.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/sem.h>

#include <unistd.h>

#include <random>

#include <time.h>

#include <signal.h>

#include <fstream>

#include <iostream>

#include <chrono>

#include <ctime>

#include <time.h>

using namespace std;

typedef struct

{

	char name[10];

	float price;

}product;



class buff

{

	public:

		product buffer[45];

		int production_index;

		int consumption_index;

		int count;

		int size;

		int init=0;

	public:

		buff(int size);

		void push(product pr);

		

};



buff::buff(int sizee)

{

	size=sizee;

	if(init !=1)

	{

		consumption_index=0;

		production_index=0;

		count=0;

		init=1;

		printf("new buffer  initiallized\n");

	}

}

void buff::push(product pr)

{	

	buffer[production_index]=pr;

	count++;

	production_index=(production_index+1)%size;

}



double produce(float mean, float standarddev ){

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    		//(seed)

    		std::default_random_engine generator (seed); //gets a value from the normal distribution

  		std::normal_distribution<double> distribution(mean,standarddev);

		double price = distribution(generator);

		return price;	

}

volatile sig_atomic_t logflag = 0;



void loghandler(char str [])  // can be called asynchronously

{

    char current_time[100];

	struct timespec start;

	

    	if( clock_gettime( CLOCK_REALTIME, &start) == -1 )

    	{

      		perror( "clock gettime" );

      		exit( EXIT_FAILURE );

    	}

    time_t t=time(NULL);

    struct tm tm = *localtime(&t);

    fprintf(stderr, "[%d-%02d-%02d %02d:%02d:%02d.%ld] %s\n", tm.tm_mon +1,tm.tm_mday,tm.tm_year +1900, tm.tm_hour, tm.tm_min,

             tm.tm_sec ,start.tv_nsec,str);







}



int main(int argc, char** argv)

{	

	

	char name [40];

	strcpy(name ,argv[1]);

	double mean= stod(argv[2]);

	double sd= stod(argv[3]);

	int sleepdur= stoi(argv[4]);

	int buffsize= stoi(argv[5]);

	int mutex,full,wait;

	sleepdur=sleepdur/1000;

	union semun  

	   {

		int val;

		struct semid_ds *buf;

		ushort array [1];

	   }sem_attr;

	product x;

	//strcpy(x.name,"omar");

	//char str[1000];

	key_t s_key = ftok("./a",515);

	//printf("key to shared memory:%d\n",s_key);

	int shmid=shmget(s_key,100000, IPC_CREAT | 0666);

		if (shmid < 0)

		{

			perror("couldn't create shared memory prod");

			exit(1);

		}

	//printf("Shared memory id: %d\n",shmid);

	void* data=shmat(shmid,(void*)0,0);

	buff* b= new(data) buff(buffsize);

	//full semaphore

	key_t key1 =ftok ("./d", 6000); 

	if ((full = semget(key1, 1, 0666)) == -1) 

    		{

		perror("semget mutex");

		exit(1);

    		} 	

	//mutex semaphore

	key_t key2 =ftok ("./e", 7000);

    	if ((mutex = semget(key2, 1, 0666)) == -1) 

    		{

		perror("semget mutex");

		exit(1);

    		} 

    	//wait semaphore		

    	key_t key3 =ftok ("./f", 8000);  

    	if ((wait = semget(key3, 1, 0666)) == -1) 

    		{

		perror("semget mutex");

		exit(1);

    		} 	

	



	struct sembuf asem [1];

	asem [0].sem_num = 0;

	asem [0].sem_op = 0;

	asem [0].sem_flg = 0; 

	

	while(1){

		

		strcpy(x.name,name);

		x.price=produce(mean,sd);

		char str1 [40];

		snprintf(str1, sizeof(str1),"generating a new value %.02f",x.price);

		loghandler(str1);

	 	asem [0].sem_op = -1;

        if (semop (full, asem, 1) == -1) {

	    perror ("semop: buffer_count_sem"); exit (1);

        }

		char str2 [40];

		strcpy(str2,"trying to get mutex on shared buffer");

		loghandler(str2);

        asem [0].sem_op = -1;

        if (semop (mutex, asem, 1) == -1) {

	    perror ("semop: mutex_sem"); exit (1);

        }

		

		

		

		char str3 [40];

		snprintf(str3,sizeof(str3),"placing %.02f on shared buffer",x.price);

		loghandler(str3);



		

		if ( b->init != 1 )

		{			

			printf("buffer was initiallized twice\n");

			exit(1);

		}

		

	b->push(x);

	

	 asem [0].sem_op = 1;

        if (semop (mutex, asem, 1) == -1) {

	    perror ("semop: mutex_sem"); exit (1);

        }

	        asem [0].sem_op = 1;

        if (semop (wait, asem, 1) == -1) {

	    perror ("semop: spool_signal_sem"); exit (1);

        }

    

        // Take a nap

		char str4 [40];

		snprintf(str4,sizeof(str4),"sleeping for %d ms",sleepdur);

		loghandler(str4);

		 sleep (sleepdur);	



}

shmdt(b);



return 0;

}

