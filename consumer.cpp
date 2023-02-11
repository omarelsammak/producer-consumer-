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
using namespace std;
typedef struct

{

	char name[10];

	float price;

}product;

typedef struct
{
	char name[10];
	float current_price;
	float current_average;
	float previous_prices[4];
	int count;
	float previous_price;
	float previous_average;
	int price_index;
	int average_index;
}Commodities;





Commodities* Commodities_initialize(char name[])

{

	Commodities* c=(Commodities *)malloc(sizeof(Commodities));

	strcpy(c->name,name);

	c->current_price=0.0;

	c->current_average=0.0;

	c->count=-1;

	c->previous_price=0.0;

	c->previous_average=0.0;	

	c->price_index=c->average_index=0;

	return c;

}





class buff

{

	public:

		product buffer[45];

		int production_index;

		int consumption_index;

		int count;

		int size;

		int init;

	public:

		buff(int size);

		product pop();

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



	}

}





product buff::pop()

{

		if (count>0){

			product pr=buffer[consumption_index];

			consumption_index=(consumption_index+1)%(size);

			count--;

			return pr;

			}

}





void Commodity_consumption(float price , Commodities* c){

	float sum=0.0;

	float avg;

	if (c->	current_price<price){

		c->price_index=1;

	}

	else if (c->current_price>price){

		c->price_index=-1;

	}

	else if (c->current_price==price){

		c->price_index=0;

	}

	for (int i=0;i<=3 && i<=c->count; i++){

			

		sum+= c->previous_prices[i];

	}

	sum+=price;

	if (c->count > 3)

	{

		avg=sum/5;

	}

	else

	{

		avg=sum/(c->count+2);

	}	

	if (c->current_average < avg)

	{

		c->average_index=1;

	}

	else if(c->current_average > avg)

	{

		c->average_index=-1;

	}

	else if(c->current_average == avg)

	{

		c->average_index=0;

	}

	c->count++;

	c->previous_average=c->current_average;

	c->current_average=avg;

	c->previous_price=c->current_price;

	c->current_price=price;

	c->previous_prices[(c->count)%4]=price;

	//printf("PRICE FLAG = %d     AVG FLAG = %d",c->price_index,c->average_index);

	



}





int get_commodity(Commodities* comm[],product x)

{

for(int i=0; i<11 ; i++)

		{

			if(!strcmp(comm[i]->name,x.name))

			{

				return i;

				

			}

		}



}



void display_commodities(Commodities* comm[]){

	char b[20];

	printf("\e[1;1H\e[2J");

	printf("+-------------------------------------+\n");

	printf("| Currency      |  Price  |  AvgPrice |\n");

	printf("+-------------------------------------+\n");

	for(int i=0; i<=10;i++){

		printf("|");

		printf("%-14s |",comm[i]->name);

		if (comm[i]->price_index==0){

			//printf("  ");

			printf("\033[;34m%7.2lf\033[0m",comm[i]->current_price);

		}

		if (comm[i]->price_index==1){

			//printf("  ");

			printf("\033[;32m%6.2lf↑\033[0m",comm[i]->current_price);

		}

		if (comm[i]->price_index==-1){

			//printf("  ");

			printf("\033[1;31m%6.2lf↓\033[0m",comm[i]->current_price);

		}

		printf("  | ");

		if (comm[i]->average_index==0){

			//printf("  ");

			printf("\033[;34m%7.2lf\033[0m",comm[i]->current_average);

		}

		if (comm[i]->average_index==1){

			//printf("  ");

			printf("\033[;32m%6.2lf↑\033[0m",comm[i]->current_average);

		}

		if (comm[i]->average_index==-1){

			//printf("  ");

			printf("\033[1;31m%6.2lf↓\033[0m",comm[i]->current_average);

		}

		printf("   |\n");

		



	

	}

printf("+-------------------------------------+\n");

}









int main(int argc, char** argv)

{	


	Commodities* GOLD=Commodities_initialize("GOLD");
	Commodities* SILVER=Commodities_initialize("SILVER");
	Commodities* CRUDEOIL=Commodities_initialize("CRUDEOIL");
	Commodities* NATURALGAS=Commodities_initialize("NATURALGAS");
	Commodities* ALUMINIUM=Commodities_initialize("ALUMINIUM");
	Commodities* COPPER=Commodities_initialize("COPPER");
	Commodities* NICKEL=Commodities_initialize("NICKEL");
	Commodities* LEAD=Commodities_initialize("LEAD");
	Commodities* ZINC=Commodities_initialize("ZINC");
	Commodities* MENTHAOIL=Commodities_initialize("MENTHAOIL");
	Commodities* COTTON=Commodities_initialize("COTTON");
	Commodities* Commodities_array[]={ALUMINIUM,COPPER,COTTON,CRUDEOIL,GOLD,LEAD,MENTHAOIL,NATURALGAS,NICKEL,SILVER,ZINC};

	

	int mutex,full,wait;

	int z;

	int buffsize=atoi(argv[1]);

	union semun  

	   {

		int val;

		struct semid_ds *buf;

		ushort array [1];

	   }sem_attr;

	   

	key_t key1 =ftok ("./d", 6000);

	if ((full= semget (key1, 1, IPC_CREAT | IPC_EXCL | 0666)) == -1) {

        	perror ("semget"); exit (1);

    			}

        // Giving initial value. 

	sem_attr.val = buffsize;        // unlocked

	 if (semctl (full, 0, SETVAL, sem_attr) == -1) {

		perror ("semctl SETVAL"); exit (1);

	    }

	key_t key2 =ftok ("./e", 7000);

	if ((mutex= semget (key2, 1, IPC_CREAT | IPC_EXCL | 0666)) == -1) {

		perror ("semget"); exit (1);

	    }

	    // Giving initial value. 

	 sem_attr.val = 1;        // unlocked

	 if (semctl (mutex, 0, SETVAL, sem_attr) == -1) {

		perror ("semctl SETVAL"); exit (1);

	    }

	 key_t key3 =ftok ("./f", 8000);		

	 if ((wait= semget (key3, 1, IPC_CREAT | IPC_EXCL | 0666)) == -1) {

		perror ("semget"); exit (1);

	    }

	    // Giving initial value. 

	    sem_attr.val = 0;        // unlocked

	 if (semctl (wait, 0, SETVAL, sem_attr) == -1) {

		perror ("semctl SETVAL"); exit (1);

	 	}

	

	key_t s_key = ftok("./a",515);

	struct sembuf asem [1];

	asem [0].sem_num = 0;

	asem [0].sem_op = 0;

	asem [0].sem_flg = 0;

	while (1){

	

	

	asem [0].sem_op = -1;

        if (semop (wait, asem, 1) == -1) {

	    perror ("semop: buffer_count_sem"); exit (1);

        }

        asem [0].sem_op = -1;

        if (semop (mutex, asem, 1) == -1) {

	    perror ("semop: mutex_sem"); exit (1);

        }

	int shmid=shmget(s_key,100000,0666);

		if (shmid < 0)

		{

			perror ("shared memory cannot be created con");

			exit (1);

		}

		void* data=shmat(shmid,(void*)0,0);

		buff* b= new(data) buff(buffsize);  

		

		if(b->init !=1)

		{

			printf("Buffer was not initialized\n");

			exit(1);

		}

	product x=b->pop();

	shmdt(b);

	z=get_commodity(Commodities_array,x);

	Commodities* c=Commodities_array[z];

	

	Commodity_consumption(x.price,c);

	display_commodities(Commodities_array);

	

	 asem [0].sem_op = 1;

        if (semop (mutex, asem, 1) == -1) {

	    perror ("semop: mutex_sem"); exit (1);

        }

	        asem [0].sem_op = 1;

        if (semop (full, asem, 1) == -1) {

	    perror ("semop: wait"); exit (1);

        }

		

        // Take a nap

        sleep (1);		



}







return 0;

}

